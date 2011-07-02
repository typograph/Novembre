#include "NVBDirModelHelpers.h"
#include "NVBFileInfo.h"
#include "NVBFileFactory.h"
#include <QtCore/QtAlgorithms>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>

#if QT_VERSION < 0x040400
#include <QThread>
class NVBDirPopulationThread : public QThread {
  Q_OBJECT
  private:
  public:
    NVBDirPopulationThread(NVBDirEntry e);
  protected:
    void run();
};
#else
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentMap>
#endif

bool NVBDirModelFileInfoLessThan::operator()(const NVBFileInfo* fi1, const NVBFileInfo* fi2) const {
	if (!fi1 || !fi2) {
		NVBOutputError("Got NULL NVBFileInfo");
		return true;
		}
	switch (sortOrder) {
		case Qt::AscendingOrder :
			return fi1->getInfo(sortKey) < fi2->getInfo(sortKey);
		case Qt::DescendingOrder :
			return fi1->getInfo(sortKey) > fi2->getInfo(sortKey);
		default :
			return false;
		}
}

bool NVBDirModelFileInfoFilter::operator()(const NVBFileInfo * fi) const {

	if (!fi) {
		NVBOutputError("Got NULL NVBFileInfo");
		return false;
		}

	if (!columns)
		return true;
	if (filters.isEmpty())
		return true;

	bool accept = true;
	foreach (NVBFileFilter f, filters) {
		bool filter_pass = fi->getInfoAsString(columns->key(f.column)).contains(f.match);
		switch (f.binding) {
			case NVBFileFilter::And : {
				accept &= filter_pass;
				break;
				}
			case NVBFileFilter::AndNot : {
				accept &= !filter_pass;
				break;
				}
			case NVBFileFilter::Or : {
				accept |= filter_pass;
				break;
				}
			case NVBFileFilter::OrNot : {
				accept |= !filter_pass;
				break;
				}
			}
		}
	return accept;
	}


NVBDirEntryLoader::NVBDirEntryLoader(NVBDirEntry* entry): QRunnable(), e(entry)
{
	setAutoDelete(true);
	fileFactory = qApp->property("filesFactory").value<NVBFileFactory*>();
}

NVBDirEntryLoader::~NVBDirEntryLoader()
{

}

void NVBDirEntryLoader::run()
{
	NVBOutputPMsg("Hi!");
	
	if (!e) return;
	
	if (e->type == NVBDirEntry::NoContent) {
		QMetaObject::invokeMethod(e,"setLoaded",Qt::AutoConnection);
		return;
		}

	QDir dir = e->dir;
	
	if (!dir.exists()) {
		NVBOutputError(QString("Directory %1 does not exist").arg(dir.absolutePath()));
		QMetaObject::invokeMethod(e,"errorOnLoad",Qt::AutoConnection);
		return;
		}

	bool wasEmpty = e->status == NVBDirEntry::Virgin;

	e->status = NVBDirEntry::Loading;

// 	QApplication::setOverrideCursor(Qt::BusyCursor);

	if (e->isRecursive()) {
		QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

		int index;
		QList<NVBDirEntry * > newentries;
		
		foreach(QFileInfo folder, subfolders) {
			if (e->folderCount() && (index = e->indexOf(folder.fileName())) >= 0)
				e->removeFolderAt(index);
			else {
				QDir rdir(dir);
				rdir.setPath(folder.absoluteFilePath());
				newentries << new NVBDirEntry(e,folder.fileName(),rdir,true);
				newentries.last()->moveToThread(QCoreApplication::instance()->thread());
				}
			}
		
		e->addFolders(newentries);
		}

	if (!fileFactory) {
		NVBOutputError("Cannot access the file factory");
		QMetaObject::invokeMethod(e,"errorOnLoad",Qt::AutoConnection);
		return;
		}

	QStringList filenames = dir.entryList(fileFactory->getDirFilters(),QDir::Files,QDir::Name);
	
	if (filenames.isEmpty()) {
		QMetaObject::invokeMethod(e,"setLoaded",Qt::AutoConnection);
		return;
		}
	
	QString dirStr = QString("%1/%2").arg(e->dir.absolutePath());
	
	for(QStringList::iterator it = filenames.begin(); it != filenames.end(); it++)
		*it = dirStr.arg(*it);

	// Remove already loaded files from the list
	// We assume the list is OK, i.e. there're no two infos using the same file
	bool deleted; // Not the most elegant solution, I admit.
	for (QList<NVBFileInfo*>::const_iterator it = e->files.constBegin(); it != e->files.constEnd(); ) {
		deleted = false;
		for (int ni = 0; ni < (*it)->files.count(); ni += 1)
			if (!filenames.removeOne((*it)->files.at(ni))) { // backtrace
				for (ni -= 1; ni >= 0; ni -= 1)
					filenames.append((*it)->files.at(ni));
				e->removeOrigFileAt(it - e->files.constBegin());
				deleted = true;
				break;
				}
		if (!deleted)
			it++;
		}

	QList<NVBAssociatedFilesInfo>	files;

	while (filenames.count() > 0) {
		NVBAssociatedFilesInfo info = fileFactory->associatedFiles(filenames.first());
		if (info.isEmpty()) {
			NVBOutputError(QString("Couldn't load associated files for %1").arg(filenames.takeFirst()));
			continue;
			}
		foreach(QString filename, info) {
//			QStringList::const_iterator j = qBinaryFind(filenames,filename);
//			if (j != filenames.end())
//				filenames.removeAt(j - filenames.begin());
			int j = filenames.indexOf(filename);
			if (j != -1)
				filenames.removeAt(j);
			else // Somebody has it already -- cross-check with list
				for(int i=0; i<e->files.count(); i++)
					if (e->files.at(i)->files.contains(filename)) {
						if (e->files.at(i)->files.count() >= info.count()) // There's already one good file
							goto skip_file;	
						else {
							foreach(QString fname, e->files.at(i)->files)
								filenames.prepend(fname);
							filenames.removeOne(filename);
							e->removeOrigFileAt(i);
							break;
							}
						}
			}
		files.append(info);
// Yeah, I know goto's are evil, but how else can I solve this thing with breaking 2 loops?
skip_file: ;
		}
		
	NVBOutputPMsg(QString("Loading started : %1 files to load").arg(files.count()));
	QFutureWatcher<NVBFileInfo*> fileLoader(0);
	fileLoader.connect(&fileLoader,SIGNAL(resultsReadyAt(int,int)),e,SLOT(notifyLoading(int,int)));
	fileLoader.connect(&fileLoader,SIGNAL(finished()),e,SLOT(setLoaded()));
	fileLoader.setFuture(QtConcurrent::mapped(files,&NVBAssociatedFilesInfo::loadFileInfo));

	fileLoader.waitForFinished();
	
	NVBOutputPMsg("Loading finished");
}


NVBDirEntry::NVBDirEntry( ):QObject(),parent(0),status(NVBDirEntry::Populated),type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label) :
	/*QObject(),*/ parent(_parent),label(_label),status(NVBDirEntry::Populated),type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive) :
	/*QObject(),*/ parent(_parent),label(_label),dir(_dir),status(NVBDirEntry::Virgin),type(recursive ? AllContent : FileContent)
{

//   if (recursive) recurseFolders();
}

NVBDirEntry::~ NVBDirEntry( )
{
	while (!files.isEmpty()) delete files.takeFirst();
  while (!folders.isEmpty()) delete folders.takeFirst();
}

int NVBDirEntry::folderCount( ) const
{
	if (isRecursive() && !isPopulated())
		return estimatedFolderCount();
	return folders.count();
}

int NVBDirEntry::estimatedFolderCount( ) const
{
	if (isRecursive() && !isPopulated())
		return dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name).count();
	return folders.count();
}

void NVBDirEntry::recurseFolders()
{
  if (!isRecursive()) return;

  QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

//   emit beginOperation(this,0,subfolders.size(),FolderInsert);

  foreach(QFileInfo folder, subfolders) {
    QDir rdir(dir);
    rdir.setPath(folder.absoluteFilePath());
		folders << new NVBDirEntry(this,folder.fileName(),rdir,true);
		folders.last()->sort(sorter);
    }

//   emit endOperation();
}

int NVBDirEntry::estimatedFileCount() const
{
  if (type == NoContent)
    return 0;
  if ( isPopulated() )
	 return fileCount();
  if ( dir.exists() )
    return dir.count();
  return 0;
}

/// remove file at mapped index
void NVBDirEntry::removeFileAt(int i) {
  if (i >= fileCount()) {
	 NVBOutputError("Index out of bounds");
	 return;
	 }
  emit beginOperation(this,i,1,NVBDirEntry::FileRemove);
  delete files.takeAt(indexMap.at(i));
  for (int j = i; j < fileCount()-1; j++)
	 indexMap[j] = indexMap.at(j+1)-1;
  indexMap.removeLast();
  emit endOperation();
}

/// remove file at non-mapped index
void NVBDirEntry::removeOrigFileAt(int i) {
  if (i >= files.count()) {
	 NVBOutputError("Index out of bounds");
	 return;
	 }

  QList<int>::iterator mi = qLowerBound(indexMap.begin(),indexMap.end(),i);

  if ( mi == indexMap.end() || *mi != i ) {
	 // FIXME possible situation, when some thread tries
	 // to access the files by index during this operation
	 delete files.takeAt(i);
	 for (; mi != indexMap.end(); mi++)
		*mi -= 1;
	 }
  else {
	 emit beginOperation(this,mi-indexMap.begin(),1,NVBDirEntry::FileRemove);
	 delete files.takeAt(i);

	 for (; mi != indexMap.end(); mi++)
		*mi = *(mi+1)-1;
	 indexMap.removeLast();
	 emit endOperation();
	 }
}

void NVBDirEntry::removeFolderAt(int i)
{
		emit beginOperation(this,i,1,NVBDirEntry::FolderRemove);
		delete folders.takeAt(i);
		emit endOperation();
}

void NVBDirEntry::addFolders(QList< NVBDirEntry* > es)
{
	emit beginOperation(this,folders.count(),es.count(),NVBDirEntry::FolderInsert);
	foreach(NVBDirEntry * e, es) {
		folders << e;
		e->sort(sorter);
		}
	emit endOperation();	
}

void NVBDirEntry::notifyLoading(int start, int end)
{
	qDebug() << start << end;
	for (int i=start; i<end; i++)
	   insertFile(fileLoader->future().resultAt(i));
}

void NVBDirEntry::insertFile(NVBFileInfo* file)
{
	qDebug() << file << file->files.name();
	
	if (file) {
		QList<NVBFileInfo*>::iterator newpos = qLowerBound(files.begin(),files.end(),file,sorter);
		int ix = newpos - files.begin();
		// We can't use an iterator here, since it doesn't survive appending items
		int im = qLowerBound(indexMap.begin(),indexMap.end(),ix) - indexMap.begin();
		if (accepted(file)) {
			emit beginOperation(this,im,1,FileInsert);
			files.insert(newpos,file);
			indexMap.append(0);
			for(int jm = indexMap.count()-1; jm > im; jm--)
				indexMap[jm] = indexMap.at(jm-1)+1;
			indexMap[im] = ix;
			emit endOperation();
			}
		else {
			files.insert(newpos,file);
			for(; im < indexMap.count(); im++)
				indexMap[im] += 1;
			}
	}
}


void NVBDirEntry::populate(NVBFileFactory * fileFactory)
{
	if (status != Loading) {
		QApplication::setOverrideCursor(Qt::BusyCursor);
		QThreadPool::globalInstance()->start(new NVBDirEntryLoader(this));
		}
	else
		NVBOutputError("populate called while populating");
}

bool NVBDirEntry::refresh(NVBFileFactory * fileFactory)
{
	if (status != Loading) {
		QApplication::setOverrideCursor(Qt::BusyCursor);
		QThreadPool::globalInstance()->start(new NVBDirEntryLoader(this));
		return true;
		}
	else {
		NVBOutputError("refresh called while populating");
		return false;
		}
}

void NVBDirEntry::sort(const NVBDirModelFileInfoLessThan & lessThan) {
	sorter = lessThan;
	if (isPopulated()) {
		foreach(NVBDirEntry * e, folders)
			e->sort(lessThan);
		qSort(files.begin(),files.end(),lessThan);
		refilter();
		}
}

void NVBDirEntry::filter(const NVBDirModelFileInfoFilter & acceptFile) {
	accepted = acceptFile;

	// Check if we have subfolders

	if (!isPopulated())	return;

	foreach(NVBDirEntry * e, folders)
		e->filter(acceptFile);

	// Check if we have files to filter

	if (files.isEmpty()) return;

	// Clear filtered indexes

	emit beginOperation(this,0,indexMap.count(),NVBDirEntry::FileRemove);
	indexMap.clear();
	emit endOperation();

	// Put new indexes in

	for(int i = 0; i < files.count(); i++)
		if (accepted(files.at(i))) {
			emit beginOperation(this,indexMap.count(),1,NVBDirEntry::FileInsert);
			indexMap << i;
			emit endOperation();
			}
}

void NVBDirEntry::refilter() {
	if (isPopulated() && files.count() > 0) {
		indexMap.clear();
		for(int i = 0; i < files.count(); i++)
			if (accepted(files.at(i))) {
				indexMap << i;
				}
		}
}

int NVBDirEntry::indexOf( QString name )
{
  for (int i=0;i<folders.count();i++)
    if (folders[i]->label == name)
      return i;
  name = QDir::cleanPath(name);
  for (int i=0;i<files.count();i++)
		if (files[i]->files.name() == name || files[i]->files.contains(name) )
      return i;
  return -1;
}

void NVBDirEntry::addFolder(NVBDirEntry * folder)
{
  if (folders.indexOf(folder) == -1 ) {
    emit beginOperation(this,folders.size(),1,FolderInsert);
    folders.append(folder);
		folder->sort(sorter);
    emit endOperation();
    }
}

void NVBDirEntry::insertFolder(int index, NVBDirEntry *folder) {
	if (folders.indexOf(folder) == -1 ) {
		emit beginOperation(this,index,1,FolderInsert);
		folders.insert(index,folder);
		folder->sort(sorter);
		emit endOperation();
		}
}

void NVBDirEntry::refreshSubfolders(NVBFileFactory * fileFactory)
{
  foreach (NVBDirEntry * e, folders)
    if (e->isPopulated()) {
      e->refresh(fileFactory);
      e->refreshSubfolders(fileFactory);
      }
}

