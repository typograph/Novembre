//
// C++ Implementation: NVBDirViewModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBDirViewModel.h"
#include "NVBFile.h"
#include "NVBFileFactory.h"
#include "NVBDirModel.h"


NVBDirViewModel::NVBDirViewModel(NVBFileFactory * factory, QPersistentModelIndex findex, NVBDirModel * model, QObject * parent):
  QAbstractItemModel( parent ),
  fileFactory(factory),
  dirindex(findex),
  dirModel(model),
  files(QVector<NVBFile*>(model->fileCount(findex))),
  fnamecache(0),
  operationRunning(false)
{
  cacheRowCounts();
  connect(dirModel,SIGNAL(rowsAboutToBeInserted(const QModelIndex &,int,int)), this, SLOT(parentInsertingRows(const QModelIndex &,int,int)));
  connect(dirModel,SIGNAL(rowsInserted (const QModelIndex &,int,int)), this, SLOT(parentInsertedRows(const QModelIndex &,int,int)));
  connect(dirModel,SIGNAL(rowsAboutToBeRemoved (const QModelIndex &,int,int)), this, SLOT(parentRemovingRows( const QModelIndex &,int,int)));
  connect(dirModel,SIGNAL(rowsRemoved (const QModelIndex &,int,int)), this, SLOT(parentRemovedRows(const QModelIndex &,int,int)));
  connect(dirModel,SIGNAL(layoutAboutToBeChanged()), this, SLOT(parentChangingLayout()));
  connect(dirModel,SIGNAL(layoutChanged()), this, SLOT(parentChangedLayout()));
}

NVBDirViewModel::~NVBDirViewModel()
{
  foreach(NVBFile * f, files) {
    if (f) f->release();
    }
}

int NVBDirViewModel::rowCount( const QModelIndex & parent ) const
{
  if (!parent.isValid()) // how many files?
    return rowcounts.size();
  else
    return rowcounts.at(parent.row());
}

bool NVBDirViewModel::loadFile(int index) const
{
  if (files.at(index)) return true;
  if (unloadables.contains(index)) return false;

	NVBFile * f = fileFactory->openFile(dirModel->getAllFiles(dirindex.child(dirModel->folderCount(dirindex)+index,0)));
  if (f) {
    f->use();
    files[index] = f;
    return true;
    }
  else {
//     emit layoutAboutToBeChanged();
    unloadables << index;
//     emit layoutChanged();
    return false;
    }
}

int NVBDirViewModel::columnCount( const QModelIndex & ) const
{
  return 1;
}

Qt::ItemFlags NVBDirViewModel::flags( const QModelIndex & index ) const
{
  if (!index.isValid())
    return QAbstractItemModel::flags(index);
  if (!index.parent().isValid())
    return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant NVBDirViewModel::data( const QModelIndex & index, int role ) const
{
  if (!index.isValid()) return QVariant();

  if (index.internalId() == 0) {
    return dirModel->data(dirindex.child(dirModel->folderCount(dirindex)+index.row(),0),role);
    }
  else {
    if (loadFile(index.internalId()-1))
      return files.at(index.internalId()-1)->index(index.row(),0).data(role);
    else
      return QVariant();
    }

  return QVariant();
}

QVariant NVBDirViewModel::headerData( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
  return QVariant();
}

bool NVBDirViewModel::hasChildren( const QModelIndex & parent ) const
{
  if (!parent.isValid()) return true;

  return not unloadables.contains(parent.row());
}

QModelIndex NVBDirViewModel::index( int row, int column, const QModelIndex & parent ) const
{
  if (column > 0) return QModelIndex();

  if (row >= rowCount(parent)) return QModelIndex();

  if (parent.isValid())
    return createIndex(row,column,parent.row()+1);
  else
    return createIndex(row,column,0);
}

QModelIndex NVBDirViewModel::parent( const QModelIndex & index ) const
{
  if (!index.isValid())
    return QModelIndex();

  if (index.internalId() == 0)
    return QModelIndex();

  return createIndex(index.internalId()-1,0,0);
}

void NVBDirViewModel::parentInsertingRows(const QModelIndex & parent, int first, int last)
{
  if (parent == dirindex) {
    int fc = dirModel->folderCount(dirindex);
    first = qMax(first,fc);
    if (last >= first) {
      operationRunning = true;
      beginInsertRows(QModelIndex(),first - fc, last - fc);
      }
    }
}

void NVBDirViewModel::parentInsertedRows(const QModelIndex & /*parent*/, int first, int last)
{
  if (operationRunning) { // FIXME shift unloadables
    rowcounts.insert(first,last-first+1,0);
    cacheRowCounts(first,last);
    for(int i = 0; i < unloadables.size(); i++) {
      if (unloadables.at(i) >= first)
        unloadables[i] += last-first+1;
      }
    files.insert(first,last-first+1,0);
    endInsertRows();
    }
  operationRunning = false;
}

void NVBDirViewModel::parentRemovingRows(const QModelIndex & parent, int first, int last)
{
  if (parent == dirindex) {
    int fc = dirModel->folderCount(dirindex);
    first = qMax(first,fc);
    if (last >= first) {
      operationRunning = true;
      beginRemoveRows(QModelIndex(),first - fc, last - fc);
      }
    }
}

void NVBDirViewModel::parentRemovedRows(const QModelIndex & /*parent*/, int first, int last)
{
  if (operationRunning) {
    rowcounts.remove(first,last-first+1);
    for(int i = 0; i < unloadables.size(); i++) {
      if (unloadables.at(i) > last)
        unloadables[i] -= last-first+1;
      else if (unloadables.at(i) >= first)
        unloadables.removeAt(i--);
      }
    for (int i = first; i <= last; i++) {
      if (files.at(i))
        files.at(i)->release();
      }
    files.remove(first,last-first+1);
//     rowcounts.resize(dirModel->fileCount(dirindex));
    endRemoveRows();
    }
  operationRunning = false;
}

void NVBDirViewModel::cacheRowCounts( ) const
{
  rowcounts.resize(dirModel->fileCount(dirindex));
  cacheRowCounts(0,rowcounts.size()-1);
}

void NVBDirViewModel::cacheRowCounts( int first, int last ) const
{
  int fc = dirModel->folderCount(dirindex);
  for (int i = first ; i <= last; i++) {
    if (unloadables.contains(i))
      rowcounts[i] = 0;
    else {
      const NVBFileInfo * fInfo = dirModel->indexToInfo(dirindex.child(fc+i,0));
      if (fInfo)
        rowcounts[i] = fInfo->pages.size();
      else
        rowcounts[i] = 0;
      }
    }
}

void NVBDirViewModel::parentChangingLayout( )
{
  emit layoutAboutToBeChanged();
  // Cache filenames
	fnamecache = new QVector<QString>(rowCount());
  for(int i = rowCount()-1;i>=0;i--) {
		fnamecache->operator[](i) = dirModel->indexToInfo(dirModel->index(i,0,dirindex))->files.name();
    }
}

void NVBDirViewModel::parentChangedLayout( )
{
  if (fnamecache) {
    QVector<NVBFile*> recache(files.size());
    QVector<int> recounts(rowcounts.size());
    recache.fill(0);
    for(int i = rowCount()-1;i>=0;i--) {
			int x = fnamecache->indexOf(dirModel->indexToInfo(dirModel->index(i,0,dirindex))->files.name());
      changePersistentIndex(index(x,0),index(i,0));
      recache[i] = files.at(x);
      recounts[i] = rowcounts.at(x);
      }
    files = recache;
    rowcounts = recounts;
    delete fnamecache;
    fnamecache = 0;
    }
  emit layoutChanged();
}

void NVBDirViewModel::defineWindow(int start, int end)
{
  for(int i=0;i<start;i++)
    if (files.at(i)) {
      files[i]->release();
      files[i] = 0;
      }

  for(int i=end+1;i<files.size();i++)
    if (files.at(i)) {
      files[i]->release();
      files[i] = 0;
      }
}
