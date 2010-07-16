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
#include "NVBColumnDialog.h"
#include "NVBFileFactory.h"
#include "NVBLogger.h"
#include <QtCore/QtAlgorithms>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>

#define DataSortRole Qt::UserRole

NVBDirModel::NVBDirModel(NVBFileFactory * _fileFactory, QObject * parent)
 : QAbstractItemModel( parent ), fileFactory(_fileFactory)
{
	columns = new NVBDirModelColumns();
	sortOrder = Qt::AscendingOrder;
	sortColumn = 0;

	head = new NVBDirEntry(0,QString());
	head->sort(NVBDirModelFileInfoLessThan(columns->key(0),Qt::AscendingOrder));
  connectEntry(head);

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
    
	if (!entry->isPopulated()) {
      entry->populate(fileFactory);
      connectEntry(entry);
      watcher->addPath(entry->dir.path());
      }

  return entry->folders.size()+entry->fileCount();

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
	return columns->rowCount();
//   return 1;
}

Qt::ItemFlags NVBDirModel::flags( const QModelIndex & index ) const
{
  if (!index.isValid() || index.column())
    return QAbstractItemModel::flags(index);
	return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant NVBDirModel::data( const QModelIndex & index, int role ) const
{
  if (!index.isValid() || (role != Qt::DisplayRole && role != DataSortRole && role != Qt::ToolTipRole))
    return QVariant();
  if (isAFile(index)) {
		const NVBFileInfo * fInfo = indexToInfo(index);
		if (!fInfo) {
			NVBOutputError(QString("File info not found at %1").arg(index.row()));
      return QVariant();
			}
		else if (index.column() < columns->rowCount()) {
      if (role == Qt::DisplayRole)
				return fInfo->getInfoAsString(columns->key(index.column()));
      else if (role == DataSortRole)
				return QVariant::fromValue(fInfo->getInfo(columns->key(index.column())));
			else if (role == Qt::ToolTipRole)
				return fInfo->files.join("\n");
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
			return columns->name(section);
    else if (role == NVBColKeyRole)
			return QVariant::fromValue(columns->key(section));
    else if (role == NVBColStrKeyRole)
			return QVariant::fromValue(columns->sourceKey(section));
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
		NVBOutputError("Invalid index : can lead to all sorts of behavior");
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

const NVBFileInfo * NVBDirModel::indexToInfo( const QModelIndex & index ) const
{
	if (!isAFile(index)) return 0;

  NVBDirEntry* entry = indexToParentEntry(index);

	if (!entry->isPopulated()) {
		entry->populate(fileFactory);
		connectEntry(entry);
		watcher->addPath(entry->dir.path());
		}

  return entry->fileAt(index.row()-entry->folders.size());
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
	NVBOutputError("Trying to remove files from list");
  return false;
}

bool NVBDirModel::removeItem( const QModelIndex & index )
{
  if (!index.isValid()) {
		NVBOutputError("Trying to remove root. Never!");
    // TODO Implement undo to allow removing root
    return false;
    }

  return removeRows( index.row(), 1, index.parent() );
}

QString NVBDirModel::getFullPath( const QModelIndex & index )
{
	if (!index.isValid() || isAFile(index))
    return QString();

	NVBDirEntry * entry = indexToEntry(index);
	if (entry->isContainer())
		return QString();
	else
		return entry->dir.absolutePath();
}

NVBAssociatedFilesInfo NVBDirModel::getAllFiles(const QModelIndex & index) {
	if (isAFile(index))
		return indexToInfo(index)->files;
	return NVBAssociatedFilesInfo();
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
	if (!entry->isPopulated()) {
		entry->populate(fileFactory);
		connectEntry(entry);
		watcher->addPath(entry->dir.path());
		}
  return entry->fileCount();
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

	// Reminder : since the column is added at the end of list,
	// it's not necessary to change sorting or filters -- they are unaffected.
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

void NVBDirModel::removeColumn(int index)
{
  if (index == 0) {
		NVBOutputError("Trying to remove the filename column");
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
	columns->removeColumn(index);
  while (i--) endRemoveColumns();

	if (index == sortColumn) {
		sortColumn = 0;
		sortOrder = Qt::AscendingOrder;
		}
	else if (index < sortColumn)
		sortColumn -= 1;

	// Deleting columns might invalidate the filter
	// Of course, just deleting invalid filters will change the global meaning.
	// but it is impossible not to do that.

	bool filtersChanged = false;

	for (int i = 0; i < filters.count(); ) {
		if (filters.at(i).column == index) {
			filters.removeAt(i);
			filtersChanged = true;
			}
		else
			i += 1;
		}

	if (filtersChanged)
		head->filter(NVBDirModelFileInfoFilter(filters,columns));

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
	return n + entry->fileCount();
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
    head->filter(NVBDirModelFileInfoFilter(filters,columns));
    }
  delete dialog;
}

void NVBDirModel::showColumnDialog()
{
	NVBColumnDialog * dialog = new NVBColumnDialog( columns->clmnDataList() );
	dialog->disableEntry(0);
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
					removeColumn(a.index);
          break;
          }
        }
      }

		for (int i = 0; i < dlg_columns.size(); i++) {
			if ( columns->name(i) != dlg_columns.at(i).name || columns->key(i).sourceString() != dlg_columns.at(i).contents.sourceString()) {
				updateColumn(i,dlg_columns.at(i));
        }
      }

//     filters = dialog->getFilterList();
//     updateFilters();
    }
  delete dialog;
}

void NVBDirModel::addColumn(NVBColumnDescriptor column)
{
  addColumn(column.name,column.contents);
}

void NVBDirModel::updateColumn(int index, NVBColumnDescriptor column)
{
	columns->updateColumn(index,column);
	emit headerDataChanged(Qt::Horizontal,index,index);
	if (index == sortColumn)
		sort(index,sortOrder);
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
	if (entry && entry->isPopulated() && count > 0)
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
	if (o.e && o.e->isPopulated() && o.c > 0)
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

void NVBDirModel::exportData( const QModelIndex & index, NVBDirExportOptions options ) const
{
  if (!options.valid) return;
  QFile f(options.fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		NVBOutputFileError(&f);
		return;
		}
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

void NVBDirModel::sort ( int column, Qt::SortOrder order ) {
	emit layoutAboutToBeChanged();
	sortColumn = column;
	sortOrder = order;
	head->sort(NVBDirModelFileInfoLessThan(columns->key(column),order));
	emit layoutChanged();
}
