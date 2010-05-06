//
// C++ Implementation: NVBDirModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBDirModel.h"
#include "NVBFileInfo.h"
#include "NVBProgress.h"
#include "NVBColumnDialog.h"
#include "NVBFileFactory.h"
#include <QtAlgorithms>
#include <QFileSystemWatcher>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#define DataSortRole Qt::UserRole

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
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentMap>
#endif

NVBDirEntry::NVBDirEntry( ):QObject(),parent(0),populated(true),loaded(true),type(NoContent) {;}

NVBDirEntry::NVBDirEntry( NVBDirEntry * _parent, QString _label) :
  /*QObject(),*/ parent(_parent),label(_label),populated(true),loaded(true),type(NoContent) {;}

NVBDirEntry::NVBDirEntry( NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive) :
  /*QObject(),*/ parent(_parent),label(_label),dir(_dir),populated(false),loaded(false),type(recursive ? AllContent : FileContent)
{

//   if (recursive) recurseFolders();
}

NVBDirEntry::~ NVBDirEntry( )
{
  while (!files.isEmpty()) delete files.takeFirst().info;
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
    }

//   emit endOperation();
}

void NVBDirEntry::invalidateFiltered() {
	filtered.clear();
	for (int i = 0; i < files.count(); i++)
		if (!files.at(i).filtered_out)
			filtered << i;
}

int NVBDirEntry::estimatedFileCount() const
{
  if (type == NoContent)
    return 0;
  if ( dir.exists() )
    return dir.count();
  else
    return 0;
}

void NVBDirEntry::notifyLoading(int start, int end)
{
	for (i=start; i<=end; i++) {
		QLinkedList::iterator newpos = qLowerBound(files.begin(),files.end(),fileLoader->future().resultAt(i),NVBDirModel::NVBFileInfoLessThan)
		files.insert(newpos,fileLoader->future().resultAt(i));
		emit newFile(this,newpos.);
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
	
	populated = true;

	if ( dir.exists() ) {
		if (isRecursive()) {   
			QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);
			
			emit beginOperation(this,0,subfolders.size(),FolderInsert);
			
			foreach(QFileInfo folder, subfolders) {
				QDir rdir(dir);
				rdir.setPath(folder.absoluteFilePath());
				folders << new NVBDirEntry(this,folder.fileName(),rdir,true);
				}
				
			emit endOperation();
			}
    
//     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//     QApplication::sendEvent(
//       qApp->property("progressBar").value<QObject*>(),
//       new NVBProgressStartEvent(
//         QString("Processing %1").arg(dir.absolutePath()),
//         infos.size()
//         )
//       );

		QList<NVBAssociatedFilesInfo> associations;
		fileFactory->associatedFiles(dir,associations);

#if QT_VERSION < 0x040400
		#error "You can't compile Novembre > 0.0.4 on Qt < 4.4, sorry"
#else
		fileLoader = new QFutureWatcher<NVBFileInfo*>(this);
		fileLoader->setFuture(QtConcurrent::mapped(associations,&NVBAssociatedFilesInfo::loadFileInfo));
		connect(fileLoader,SIGNAL(resultsReadyAt(int,int)),this,SLOT(notifyLoading(int,int)));
		connect(fileLoader,SIGNAL(finished()),this,SLOT(setLoaded()));
#endif


/*    QApplication::sendEvent(
      qApp->property("progressBar").value<QObject*>(),
      new NVBProgressStopEvent()
      );

    QApplication::restoreOverrideCursor();*/
    }
	else {
		NVBOutputError("NVBDirEntry::populate",QString("Directory %1 does not exist").arg(dir.absolutePath()));
		}
}

bool NVBDirEntry::refresh(NVBFileFactory * fileFactory)
{
  if ( dir.exists() ) {

    QFileInfoList infos = dir.entryInfoList();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (isRecursive()) {
      QFileInfoList subfolders = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name);

//      QApplication::sendEvent(
//        qApp->property("progressBar").value<QObject*>(),
//        new NVBProgressStartEvent(
//          QString("Refreshing %1").arg(dir.absolutePath()),
//          infos.size() + subfolders.size()
//          )
//        );

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
          emit endOperation();
          }
//        QApplication::sendEvent(
//          qApp->property("progressBar").value<QObject*>(),
//          new NVBProgressContinueEvent()
//          );
        }

      for (int i=confirmed.size()-1;i>=0;i--) {
        if (!confirmed.at(i)) {
          emit beginOperation(this,i,1,FolderRemove);
          delete folders.takeAt(i);
          emit endOperation();
          }
        }

      }
    else {
//      QApplication::sendEvent(
//        qApp->property("progressBar").value<QObject*>(),
//        new NVBProgressStartEvent(
//          QString("Refreshing %1").arg(dir.absolutePath()),
//          infos.size()
//          )
//        );
      }

///////// FIXME -- switch to AssociatedFiles
    QVector<bool> confirmed(files.size());
    confirmed.fill(false);

    foreach (QFileInfo fInfo, infos) {

      int index = indexOf(fInfo.fileName());

      if ( index < 0 ) {
        NVBDirFileEntry e(fInfo);
        e.info = fileFactory->getFileInfo(e.filePath);
        if (e.info) {
          emit beginOperation(this,files.size(),1,FileInsert);
          files.append(e);
          emit endOperation();
          }
        else  NVBOutputError("NVBDirEntry::refresh",QString("Could not load file %1").arg(e.fileName));
        }
      else {
        confirmed[index] = true;
//         if ( files.at(index)->fileInfo.size() != fInfo.size() || files.at(index)->fileInfo.lastModified() != fInfo.lastModified() ) {
//           files[index]->fileInfo.refresh();
//           // ??? fileFactory->fileModifiedOnDisk()
//           // ??? Model has to emit dataChanged()
//           } // FIXME we do not watch for changes
        }

//      QApplication::sendEvent(
//        qApp->property("progressBar").value<QObject*>(),
//        new NVBProgressContinueEvent()
//        );

      }

    for (int i=confirmed.size()-1;i>=0;i--) {
      if (!confirmed.at(i)) {
        emit beginOperation(this,i,1,FileRemove);
        delete files.takeAt(i).info;
        emit endOperation();
        }
      }

//    QApplication::sendEvent(
//      qApp->property("progressBar").value<QObject*>(),
//      new NVBProgressStopEvent()
//      );
//    QApplication::restoreOverrideCursor();

    return true;
    }
  else {
    NVBOutputError("NVBDirEntry::populate",QString("Directory %1 ceased to exist").arg(dir.absolutePath()));
    // TODO be more user-friendly. Display an error message
    return false;
    }
}

int NVBDirEntry::indexOf( QString name )
{
  for (int i=0;i<folders.count();i++)
    if (folders[i]->label == name)
      return i;
  name = QDir::cleanPath(name);
  for (int i=0;i<files.count();i++)
    if (files[i].fileName == name || files[i].filePath == name )
      return i;
  return -1;
}

NVBDirModel::NVBDirModel(NVBFileFactory * _fileFactory, QObject * parent)
 : QAbstractItemModel( parent ), fileFactory(_fileFactory)
{
  head = new NVBDirEntry(0,QString());
  connectEntry(head);
  columns = new NVBDirModelColumns();
  watcher = new QFileSystemWatcher(this);
  connect(watcher,SIGNAL(directoryChanged(const QString &)),this,SLOT(watchedDirChanged(const QString &)));
  connect(&timerToEntryMap,SIGNAL(mapped(QObject*)),this,SLOT(refreshWatchedDir(QObject*)));
}


NVBDirModel::~NVBDirModel()
{
  if (head) delete head;
  if (watcher) delete watcher;
}

QModelIndex NVBDirModel::addFolderItem( QString label, QString path, bool recursive, const QModelIndex & parent )
{
  NVBDirEntry * _parent = indexToEntry(parent);
  NVBDirEntry * entry;

  if (path.isEmpty())
    entry = new NVBDirEntry(_parent,label);
  else
    entry = new NVBDirEntry(_parent,label,QDir(path, fileFactory->getDirFilter()),recursive);

  _parent->addFolder(entry);

  return createIndex(_parent->folders.size()-1,0,_parent);  
}

int NVBDirModel::rowCount( const QModelIndex & parent ) const
{ 
  if (parent.column() > 0)
    return 0;
  
  if (isAFile(parent))
    return 0;
  
  NVBDirEntry * entry;
  entry = indexToEntry(parent);
    
  if (!entry->isPopulated())
    try {
      entry->populate(fileFactory);
      connectEntry(entry);
      watcher->addPath(entry->dir.path());
      }
    catch (int err) {
      NVBOutputError("NVBDirModel::rowCount","DirEntry population failed : Error #%d",err);
      }

  return entry->folders.size()+entry->files.size();

}

int NVBDirModel::columnCount( const QModelIndex & parent ) const
{
  if (parent.column() > 0)
    return 0;
/*  
  if (parent == QModelIndex())
    return 1 + ?1:0;
  else
*/

//   if (parent.isValid())
//     if (indexToEntry(parent)->files.count() > 0)
  return columns->rowCount()+1;
//   return 1;
}

Qt::ItemFlags NVBDirModel::flags( const QModelIndex & index ) const
{
  if (!index.isValid() || index.column())
    return QAbstractItemModel::flags(index);
//   if (isAFile(index))
  if (isAFile(index) && indexToFileEntry(index).loaded && !indexToFileEntry(index).info)
    return QAbstractItemModel::flags(index) & ~Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  else
    return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//   return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant NVBDirModel::data( const QModelIndex & index, int role ) const
{
  if (!index.isValid() || (role != Qt::DisplayRole && role != DataSortRole && role != Qt::ToolTipRole))
    return QVariant();
  if (isAFile(index)) {
    NVBDirFileEntry fEntry = indexToFileEntry(index);
/*    if (!fInfo) {
      NVBOutputError("NVBDirModel::data","File info not found");
      return QVariant();
      }*/
    if (index.column() == 0) {
      if (role == Qt::DisplayRole || role == DataSortRole)
        return fEntry.fileName;
      else if (role == Qt::ToolTipRole)
        return fEntry.filePath;
      }
    else if (index.column() <= columns->rowCount() && fEntry.info) { 
      if (role == Qt::DisplayRole)
        return fEntry.info->getInfoAsString(columns->key(index.column()-1));
      else if (role == DataSortRole)
        return QVariant::fromValue(fEntry.info->getInfo(columns->key(index.column()-1)));
      else
        return QVariant();
      }
    else
      return QVariant();
    }
  else {
    NVBDirEntry * entry = indexToEntry(index);
    if (index.column() != 0) return QVariant();
    if (role == Qt::DisplayRole)
      return entry->label;
    else if (!entry->isContainer())
      return entry->dir.path();
    return QVariant();
    }
  return QVariant();
}

QVariant NVBDirModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole)
      return section ? columns->name(section-1) : "Name";
    else if (role == NVBColKeyRole)
      return section ? QVariant::fromValue(columns->key(section-1)) : QVariant();
    else if (role == NVBColStrKeyRole)
      return section ? QVariant::fromValue(columns->sourceKey(section-1)) : QVariant();
    }
  return QVariant();
}

NVBDirEntry * NVBDirModel::indexToParentEntry( const QModelIndex & index ) const
{
  if (!index.isValid())
    return NULL;
  return (NVBDirEntry*) index.internalPointer();
}

NVBDirEntry * NVBDirModel::indexToEntry( const QModelIndex & index ) const
{
  if (!index.isValid())
    return head;
  if (!isAFile(index))
    return ((NVBDirEntry*) index.internalPointer())->folders.at(index.row());
  return NULL;
}

bool NVBDirModel::isAFile( const QModelIndex & index ) const
{
  if (!(index.isValid()))
    return false; // it's root
/*   
   {
    NVBOutputError("NVBDirModel::isAFile","Invalid index : can lead to all sorts of behavior");
    throw nvberr_invalid_input;
    }
*/
  NVBDirEntry * entry = indexToParentEntry(index);
    return (index.row() >= entry->folders.size());
}

bool NVBDirModel::hasChildren( const QModelIndex & parent ) const
{
  if (parent.column() > 0)
    return false;
  return !isAFile(parent);
}

QModelIndex NVBDirModel::index( int row, int column, const QModelIndex & parent ) const
{
/* // This leads to overpopulation
   if (!hasIndex(row, column, parent))
    return QModelIndex();*/

  if (column >= columnCount())
    return QModelIndex();

  if (isAFile(parent))
    return QModelIndex();

  NVBDirEntry* entry = indexToEntry(parent);
  if (!entry)
    return QModelIndex();

  return createIndex(row,column,entry);
}

QModelIndex NVBDirModel::parent( const QModelIndex & index ) const
{
  if (!index.isValid())
    return QModelIndex();
    
  NVBDirEntry * entry = indexToParentEntry(index);
  if (entry->parent)
    return createIndex(entry->parent->folders.indexOf(entry),0,entry->parent);
  else
    return QModelIndex();
}

NVBDirFileEntry NVBDirModel::indexToFileEntry( const QModelIndex & index ) const
{
//  if (!index.isValid()) return NULL;
  if (!isAFile(index)) return NVBDirFileEntry();
  NVBDirEntry* entry = indexToParentEntry(index);

  if (!entry->isPopulated())
    try {
      entry->populate(fileFactory);
      connectEntry(entry);
      watcher->addPath(entry->dir.path());
      }
    catch (int err) {
      NVBOutputError("NVBDirModel::rowCount","DirEntry population failed : Error #%d",err);
      return NVBDirFileEntry();
      }

  return entry->files.at(index.row()-entry->folders.size());
}

bool NVBDirModel::removeRows( int row, int count, const QModelIndex & parent )
{
  if (!isAFile(parent)) {
    NVBDirEntry * entry = indexToEntry(parent);
    if (row < entry->folders.size()) {
      if (row+count > entry->folders.size())
        count = entry->folders.size()-row;
      beginRemoveRows(parent,row,row+count-1);
      entry->setDirWatch(false);
      for(int i = 0; i < count; i++)
        delete entry->folders.takeAt(row);
      endRemoveRows();
      return true;
      }
    }
  NVBOutputError("NVBDirMode::removeRows","Trying to remove files from list");
  return false;
}

bool NVBDirModel::removeItem( const QModelIndex & index )
{
  if (!index.isValid()) {
    NVBOutputError("NVBDirModel::removeItem","Trying to remove root. Never!");
    // TODO Implement undo to allow removing root
    return false;
    }

  return removeRows( index.row(), 1, index.parent() );
}

QString NVBDirModel::getFullPath( const QModelIndex & index )
{
  if (!index.isValid())
    return QString();
  if (isAFile(index)) {
    return indexToFileEntry(index).filePath;
    }
  else {
    NVBDirEntry * entry = indexToEntry(index);
    if (entry->isContainer())
      return QString();
    else
      return entry->dir.absolutePath();
    }
}

int NVBDirModel::folderCount( const QModelIndex & parent, bool treatRecursiveSeparately) const
{
 if (isAFile(parent)) return 0;
 if (treatRecursiveSeparately && indexToEntry(parent)->isRecursive()) return -1;
 return indexToEntry(parent)->folderCount();
}

int NVBDirModel::fileCount( const QModelIndex & parent ) const
{
  if (isAFile(parent)) return 0;
  NVBDirEntry * entry = indexToEntry(parent);
  if (!entry->isPopulated())
    try {
      entry->populate(fileFactory);
      connectEntry(entry);
      watcher->addPath(entry->dir.path());
      }
    catch (int err) {
      NVBOutputError("NVBDirModel::rowCount","DirEntry population failed : Error #%d",err);
      }
  return entry->files.size();
}

void NVBDirModel::addColumn(QString name, NVBTokens::NVBTokenList key)
{
  int i = 1;
  int nclmn = columns->rowCount();
  beginInsertColumns(QModelIndex(),nclmn,nclmn);
  
  foreach(QModelIndex ix, folderIndexes(head)) {
    NVBDirEntry * entry = indexToEntry(ix);
    if ( entry && entry->isPopulated()) {
      beginInsertColumns(ix,nclmn,nclmn);
      i++;
      }
    }
  columns->addColumn(name,key);
  while (i--) endInsertColumns();
}

template <typename T, typename NonEqual>
inline void myUniquify(QList<int> & mults, QList<T> & list, NonEqual nonEqual)
{
  mults.clear();

  if (list.size() == 0) return;

  mults << 1;

  if (list.size() == 1) return;

  T value = list.at(0);

  for (int i = 1; i<list.size();) {
    if (nonEqual(list.at(i),value)) {
      value = list.at(i);
      mults << 1;
      i++;
      }
    else {
      list.removeAt(i);
      mults.last()++;
      }
    }

}

// bool variantLessThan(const NVBVariant & l, const NVBVariant & r) { return l < r; }


bool NVBDirModel::lessThan(const QModelIndex & left, const QModelIndex & right) const
{
//   NVBOutputDMsg("Comparison underway");

  QApplication::sendEvent(
    qApp->property("progressBar").value<QObject*>(),
    new NVBProgressContinueEvent()
    );

// We cannot process events at this point. Unfortunately, this function
// is called from the QSoftFilterProxyModel after the columnRemoved signal
// Processing events at this point pauses the process before index mappings
// are updated, and proceeds to update the header view of QTreeView, that
// takes weird columns with no data and crashes the program
//
//   QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

  if (left.column() != right.column()) return false;
  if (left.parent() != right.parent()) return false;
	if (isAFile(left) && isAFile(right)) {
		if (!indexToParentEntry(left)->isLoaded())
      return left.row() < right.row();
    if (right.data(DataSortRole).userType() == QMetaType::Void)
      return true;
    QVariant lsortdata = left.data(DataSortRole);
    if (lsortdata.userType() == QMetaType::Void)
      return false;
    if (lsortdata.userType() == QMetaType::type("NVBVariant")) {
      if (!lsortdata.value<NVBVariant>().isAList())
        return lsortdata.value<NVBVariant>() < right.data(DataSortRole).value<NVBVariant>();
      NVBVariantList lv = lsortdata.value<NVBVariant>().toList();
      int i = 0;
      while(i < lv.size())
        if (!lv.at(i).isValid())
          lv.removeAt(i);
        else
          i += 1;
      if (lv.isEmpty()) return true;
      NVBVariantList rv = right.data(DataSortRole).value<NVBVariant>().toList();
      i = 0;
      while(i < rv.size())
        if (!rv.at(i).isValid())
          rv.removeAt(i);
        else
          i += 1;
      if (rv.isEmpty()) return false;
  
			qSort(lv.begin(),lv.end(),NVBDirModel::variantGreaterOrEqualTo);
			qSort(rv.begin(),rv.end(),NVBDirModel::variantGreaterOrEqualTo);
      QList<int> lvm,rvm;
			myUniquify(lvm,lv,NVBDirModel::variantLessThan);
			myUniquify(rvm,rv,NVBDirModel::variantLessThan);
  
      int imax = qMin(lv.size(),rv.size());
      for (int i = 0; i<imax; i++) {
        if (lv.at(i) != rv.at(i)) return lv.at(i) < rv.at(i);
        else if (lvm.at(i) != rvm.at(i)) return lvm.at(i) < rvm.at(i);
        }

      return lv.size() < rv.size();
      }
    else {
      if (lsortdata.userType() != QMetaType::QString)
			NVBOutputError("NVBDirModel::lessThan",QString("Data for sorting role is not of type NVBVariant, but %1").arg(QMetaType::typeName(lsortdata.userType())));
      return lsortdata.toString() < right.data(DataSortRole).toString();
      return false;
      }
/*
    if (left.data(DataSortRole).type() != QVariant::List)
      return variantLessThan(left.data(DataSortRole),right.data(DataSortRole));
    QVariantList lv = left.data(DataSortRole).toList();
    int i = 0;
    while(i < lv.size())
      if (!lv.at(i).isValid())
        lv.removeAt(i);
      else
        i += 1;
    if (lv.isEmpty()) return true;
    QVariantList rv = right.data(DataSortRole).toList();
    i = 0;
    while(i < rv.size())
      if (!rv.at(i).isValid())
        rv.removeAt(i);
      else
        i += 1;
    if (rv.isEmpty()) return false;

		qSort(lv.begin(),lv.end(),NVBDirModel::variantGreaterOrEqualTo);
		qSort(rv.begin(),rv.end(),NVBDirModel::variantGreaterOrEqualTo);
    QList<int> lvm,rvm;
		myUniquify(lvm,lv,NVBDirModel::variantLessThan);
		myUniquify(rvm,rv,NVBDirModel::variantLessThan);

    int imax = qMin(lv.size(),rv.size());
    for (int i = 0; i<imax; i++) {
      bool b = variantLessThan(lv.at(i),rv.at(i));
      if (b || variantLessThan(rv.at(i),lv.at(i))) return b;
      else if (lvm.at(i) != rvm.at(i)) return lvm.at(i) < rvm.at(i);
        }
      return lv.size() < rv.size();
    return variantLessThan(lv,rv);
    */
    }
  return false;
}

void NVBDirModel::removeColumn(int index)
{
  if (index == 0) {
    NVBOutputError("NVBDirModel::removeColumn","Trying to remove the filename column");
    return;
    }

  int i = 1;
  beginRemoveColumns(QModelIndex(),index,index);
  foreach(QModelIndex ix, folderIndexes(head)) {
    NVBDirEntry * entry = indexToEntry(ix);
    if ( entry && entry->isPopulated()) {
      beginRemoveColumns(ix,index,index);
      i++;
    }
    }
  columns->removeColumn(index-1);
  while (i--) endRemoveColumns();
}

bool NVBDirModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
	if (isAFile(index(source_row,0,source_parent))) {
    if (filters.isEmpty()) return true;
    bool accept = true;
    foreach (NVBFileFilter f, filters) {
			bool filter_pass = index(source_row,f.column+1,source_parent).data(Qt::DisplayRole).toString().contains(f.match);
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
  else
    return true;
}

QList< QModelIndex > NVBDirModel::folderIndexes( NVBDirEntry * entry )
{
  QList< QModelIndex > p;
  for (int i = 0; i < entry->folders.size(); i++) {
    p << createIndex(i,0,entry);
    p << folderIndexes(entry->folders.at(i));
    }
  return p;
}

int NVBDirModel::visibleRows(NVBDirEntry * entry) const
{
	if (!entry) entry = head;
	int n = 0;
	foreach(NVBDirEntry * sub, entry->folders) {
		n += 1;
		if (sub->isPopulated())
			n += visibleRows(sub);
		}
	return n + entry->files.size();
}

void NVBDirModel::sortingStarted(int estimated) const
{
//   NVBOutputDMsg("Layout about to be changed");
	if (estimated < 0)
		estimated = visibleRows()*(int)(log(visibleRows()));

  QApplication::sendEvent(
    qApp->property("progressBar").value<QObject*>(),
    new NVBProgressStartEvent(
      "Sorting",
      estimated
      )
    );
}

void NVBDirModel::sortingFinished() const
{
//   NVBOutputDMsg("Layout changed");
  QApplication::sendEvent(
    qApp->property("progressBar").value<QObject*>(),
    new NVBProgressStopEvent()
    );
}

int NVBDirModel::estimatedRowCount(const QModelIndex & parent) const
{
  NVBDirEntry * entry =  indexToEntry(parent);
    
  if (entry->isPopulated())
    return rowCount(parent);
  else
    return entry->folders.size()+entry->estimatedFileCount();

}

void NVBDirModel::showFilterDialog()
{
	NVBFileFilterDialog * dialog = new NVBFileFilterDialog( columns, filters );
  if ( dialog->exec() == QDialog::Accepted ) {
    filters = dialog->getFilterList();
    updateFilters();
    }
  delete dialog;
}

void NVBDirModel::showColumnDialog()
{
	NVBColumnDialog * dialog = new NVBColumnDialog( columns->clmnDataList() );
  if ( dialog->exec() == QDialog::Accepted ) {
    QList<NVBColumnAction> actions = dialog->getActions();
		QList<NVBColumnDescriptor> dlg_columns = dialog->getColumns();
    foreach(NVBColumnAction a, actions) {
      switch (a.action) {
        case NVBColumnAction::Added : {
					addColumn(dlg_columns.at(a.index));
          break;
          }
        case NVBColumnAction::Deleted : {
					removeColumn(a.index + 1);
          break;
          }
        }
      }

		for (int i = 0; i < dlg_columns.size(); i++) {
			if ( columns->name(i) != dlg_columns.at(i).name || columns->key(i).sourceString() != dlg_columns.at(i).contents.sourceString()) {
				updateColumn(i+1,dlg_columns.at(i));
        }
      }

//     filters = dialog->getFilterList();
//     updateFilters();
    }
  delete dialog;
}

void NVBDirModel::updateFilters( )
{
  sortingStarted();
// FIXME update filters, really
  sortingFinished();
}

void NVBDirModel::addColumn(NVBColumnDescriptor column)
{
  addColumn(column.name,column.contents);
}

void NVBDirModel::updateColumn(int index, NVBColumnDescriptor column)
{
  columns->updateColumn(index-1,column);
  emit headerDataChanged(Qt::Horizontal,index,index);
}

void NVBDirModel::refresh()
{
  head->refreshSubfolders(fileFactory);
}

void NVBDirModel::watchedDirChanged(const QString & path)
{
  NVBDirEntry * entry = findEntryByPathFrom(head,path);
  if (entry) setEntryRefreshTimer(entry);
}

void NVBDirModel::setEntryRefreshTimer(NVBDirEntry * entry) {
  QTimer * t = qobject_cast<QTimer*>(timerToEntryMap.mapping(entry));
  if (t) t->start(1000);
  else {
    t = new QTimer(this);
    timerToEntryMap.setMapping(t, entry);
    t->start(1000);
    }
}

void NVBDirModel::refreshWatchedDir( QObject * o_entry )
{
  NVBDirEntry * entry = qobject_cast<NVBDirEntry*>(o_entry);
  if (!entry) return;
  bool exists = entry->refresh(fileFactory);
  if (!exists) {
    int row = entry->parent->folders.indexOf(entry);
    if (entry->parent->isPopulated())
      emit beginRemoveRows(entryToIndex(entry->parent),row,row+1);
    entry->parent->folders.removeAt(row);
    delete entry;
    if (entry->parent->isPopulated())
      emit endRemoveRows();
    }
  delete timerToEntryMap.mapping(entry);
}

NVBDirEntry * NVBDirModel::findEntryByPathFrom(const NVBDirEntry * entry, const QString & path) const
{
  foreach (NVBDirEntry * e, entry->folders) {
    if (e->isPopulated()) { // If it isn't populated, one can safely assume that the children are not populated either
      if (e->dir.path() == path)
        return e;
      NVBDirEntry * t = findEntryByPathFrom(e,path);
      if (t) return t;
      }
    }
  return 0;
}

QModelIndex NVBDirModel::entryToIndex(const NVBDirEntry * entry) const
{
  if (!entry || !entry->parent)
    return QModelIndex();
  return createIndex(entry->parent->folders.indexOf(const_cast<NVBDirEntry*>(entry)),0,entry->parent);
}

void NVBDirModel::connectEntry(NVBDirEntry * entry) const
{
  connect(entry,SIGNAL(beginOperation(const NVBDirEntry*, int, int, NVBDirEntry::ContentChangeType)),this,SLOT(beginEntryOperation(const NVBDirEntry*, int, int, NVBDirEntry::ContentChangeType)));
  connect(entry,SIGNAL(endOperation()),this,SLOT(endEntryOperation()));
  connect(entry,SIGNAL(filesLoaded(const NVBDirEntry *, int, int)),this,SLOT(  notifyFilesLoaded(const NVBDirEntry*,int,int)));


/*  if (entry->isRecursive())
    foreach(NVBDirEntry * e, entry->folders)
      connectEntry(e);*/
}

void NVBDirModel::beginEntryOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type)
{
  entryOperationStack.push(NVBDirEntryOperation(entry,row,count,type));
  if (entry && entry->isPopulated())
    switch (type) {
      case NVBDirEntry::FolderInsert : {
        beginInsertRows(entryToIndex(entry),row,row+count-1);
        break;
        }
      case NVBDirEntry::FolderRemove : {
        beginRemoveRows(entryToIndex(entry),row,row+count-1);
        break;
        }
      case NVBDirEntry::FileInsert : {
        beginInsertRows(entryToIndex(entry),entry->folders.size() + row, entry->folders.size() + row+count-1);
        break;
        }
      case NVBDirEntry::FileRemove : {
        beginRemoveRows(entryToIndex(entry),entry->folders.size() + row, entry->folders.size() + row+count-1);
        break;
        }
      }
}

void NVBDirModel::endEntryOperation()
{
  const NVBDirEntryOperation o = entryOperationStack.pop();
  if (o.e && o.e->isPopulated())
    switch (o.t) {
      case NVBDirEntry::FolderInsert :
/*        for (int i = o.r; i < o.r + o.c; i++)
          connectEntry(o.e->folders.at(i));*/
        // Intentionally no break;
      case NVBDirEntry::FileInsert : {
        endInsertRows();
        break;
        }
      case NVBDirEntry::FolderRemove :
      case NVBDirEntry::FileRemove : {
        endRemoveRows();
        break;
        }
      }
}

void NVBDirEntry::addFolder(NVBDirEntry * folder)
{
  if (folders.indexOf(folder) == -1 ) {
    emit beginOperation(this,folders.size(),1,FolderInsert);
    folders.append(folder);
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


void NVBDirModel::exportData( const QModelIndex & index, NVBDirExportOptions options ) const
{
  if (!options.valid) return;
  QFile f(options.fileName);
  if (!f.open(QIODevice::WriteOnly)) return;
  QTextStream t(&f);
  
  int c;
  for(c = 0; c < columnCount(index) - 1; c++) {
		t << headerData(c,Qt::Horizontal).toString() << "\t";
    }
	t << headerData(c,Qt::Horizontal).toString() << endl;
  
  if (isAFile(index))
    exportItemData(&t,index.parent(),index.row(),options.fullFileNames);
  else
    exportItemFiles(&t,index,options);
  
  f.close();
}

void NVBDirModel::exportItemFiles( QTextStream * file, const QModelIndex & index, NVBDirExportOptions options) const
{
  for(int i = 0; i<fileCount(index); i++) {
     exportItemData(file,index,i+folderCount(index),options.fullFileNames);
     }
  if (options.recursiveExport) {
    for(int i = 0; i<folderCount(index);i++) {
      QModelIndex child = index.child(i,0);
			if (indexToEntry(child)->isPopulated() || options.loadOnExport)
        exportItemFiles(file,child,options);
      }
    }
}

void NVBDirModel::exportItemData(QTextStream * file, const QModelIndex & index, int row, bool fullName ) const
{
  int c;
  
  QStringList strings;
  
  strings << index.child(row,0).data(fullName ? Qt::ToolTipRole : Qt::DisplayRole).toString().replace('\n',' ').replace('\t',' ');
  
  for(c = 1; c < columnCount(index); c++) {
    QVariant value = index.child(row,c).data(DataSortRole);   
    if (value.userType() == qMetaTypeId<NVBVariant>()) {
      NVBVariant nvalue = value.value<NVBVariant>();
      if (nvalue.isAList() && !nvalue.toList().isEmpty() ) nvalue = nvalue.toList().first(); // first page
      if (nvalue.isAList() && nvalue.toList().size() == 1 ) nvalue = nvalue.toList().first(); // the only token
      if (nvalue.userType() == qMetaTypeId<NVBPhysValue>())
        strings << QString::number(nvalue.toPhysValue().getValue());
      else
        strings << nvalue.toString().replace('\n',' ').replace('\t',' ');
      }
    else if (value.userType() == qMetaTypeId<NVBPhysValue>()) {
      strings << QString::number(value.value<NVBPhysValue>().getValue());
      }
    else
      strings << value.toString().replace('\n',' ').replace('\t',' ');
    }
  *file << strings.join("\t") << endl;
}

void NVBDirModel::notifyFilesLoaded(const NVBDirEntry * entry, int fstart, int fend)
{
  emit dataChanged(index(fstart+entry->folderCount(),1,entryToIndex(entry)),index(fend+entry->folderCount(),columns->rowCount(),entryToIndex(entry)));
}

bool NVBDirModel::NVBFileInfoLessThan(const NVBFileInfo * fi1, const NVBFileInfo * fi2)
{
	// FIXME different sort parameters
	return fi1->fileInfo.name < fi2->fileInfo.name;
}
