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
	if (!fi1) {
		NVBOutputError("Got NULL NVBFileInfo");
		return false;
		}
	if (!fi2) {
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


NVBDirEntry::NVBDirEntry( ):QObject(),parent(0),populated(true),loaded(true),type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label) :
	/*QObject(),*/ parent(_parent),label(_label),populated(true),loaded(true),type(NoContent) {;}

NVBDirEntry::NVBDirEntry(NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive) :
	/*QObject(),*/ parent(_parent),label(_label),dir(_dir),populated(false),loaded(false),type(recursive ? AllContent : FileContent)
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

void NVBDirEntry::notifyLoading(int start, int end)
{
	for (int i=start; i<end; i++)
	   insertFile(fileLoader->future().resultAt(i));
}

void NVBDirEntry::insertFile(NVBFileInfo* file)
{
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
	if (type == NoContent)
		return;

	if (isPopulated()) {
		refresh(fileFactory);
		return;
		}

	qApp->setOverrideCursor(Qt::BusyCursor);

	populated = true;

	if ( dir.exists() ) {
		if (isRecursive()) {
			QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

			emit beginOperation(this,0,subfolders.size(),FolderInsert);

			foreach(QFileInfo folder, subfolders) {
				QDir rdir(dir);
				rdir.setPath(folder.absoluteFilePath());
				folders << new NVBDirEntry(this,folder.fileName(),rdir,true);
				folders.last()->sort(sorter);
				}

			emit endOperation();
			}

		QList<NVBAssociatedFilesInfo> associations = fileFactory->associatedFilesFromDir(dir);

#if QT_VERSION < 0x040400
		#error "You can't compile Novembre > 0.0.4 on Qt < 4.4, sorry"
#else
		fileLoader = new QFutureWatcher<NVBFileInfo*>(this);
		fileLoader->setFuture(QtConcurrent::mapped(associations,&NVBAssociatedFilesInfo::loadFileInfo));
		connect(fileLoader,SIGNAL(resultsReadyAt(int,int)),SLOT(notifyLoading(int,int)),Qt::QueuedConnection);
		connect(fileLoader,SIGNAL(finished()),SLOT(setLoaded()),Qt::QueuedConnection);
#endif

	}
	else {
		NVBOutputError(QString("Directory %1 does not exist").arg(dir.absolutePath()));
		qApp->restoreOverrideCursor();
		}
}

bool NVBDirEntry::refresh(NVBFileFactory * fileFactory)
{
  if ( dir.exists() ) {

    QFileInfoList infos = dir.entryInfoList();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (isRecursive()) {
      QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

      QVector<bool> confirmed(folders.size());
      confirmed.fill(false);


      foreach(QFileInfo folder, subfolders) {
        int index = indexOf(folder.fileName()); // The labels in recursive are created this way
        if (index >= 0) {
          confirmed[index] = true;
          }
        else {
          QDir rdir(dir);
          rdir.setPath(folder.absoluteFilePath());
          emit beginOperation(this,folders.size(),1,FolderInsert);
					folders << new NVBDirEntry(this,folder.fileName(),rdir,true);
					folders.last()->sort(sorter);
          emit endOperation();
          }
        }

      for (int i=confirmed.size()-1;i>=0;i--) {
        if (!confirmed.at(i)) {
          emit beginOperation(this,i,1,FolderRemove);
          delete folders.takeAt(i);
          emit endOperation();
          }
        }

      }

		QList<int> ixrm;

		QList<NVBAssociatedFilesInfo> * old_associations = new QList<NVBAssociatedFilesInfo>();
		foreach(NVBFileInfo * fi, files)
			old_associations->append(fi->files);

		QList<NVBAssociatedFilesInfo> associations = fileFactory->associatedFilesFromDir(dir,old_associations,&ixrm);

		delete old_associations;

		qSort(ixrm.begin(),ixrm.end(),qGreater<int>());

		foreach(int k, ixrm) {
			emit beginOperation(this,k,1,FileRemove);
			delete files.takeAt(k);
			emit endOperation();
			}

#if QT_VERSION < 0x040400
		#error "You can't compile Novembre > 0.0.4 on Qt < 4.4, sorry"
#else
		fileLoader = new QFutureWatcher<NVBFileInfo*>(this);
		fileLoader->setFuture(QtConcurrent::mapped(associations,&NVBAssociatedFilesInfo::loadFileInfo));
		connect(fileLoader,SIGNAL(resultsReadyAt(int,int)),this,SLOT(notifyLoading(int,int)));
		connect(fileLoader,SIGNAL(finished()),this,SLOT(setLoaded()));
#endif

    return true;
    }

  else {
		NVBOutputError(QString("Directory %1 ceased to exist").arg(dir.absolutePath()));
		qApp->restoreOverrideCursor();
		// TODO be more user-friendly. Display an error message
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

void NVBDirEntry::refreshSubfolders(NVBFileFactory * fileFactory)
{
  foreach (NVBDirEntry * e, folders)
    if (e->isPopulated()) {
      e->refresh(fileFactory);
      e->refreshSubfolders(fileFactory);
      }
}

