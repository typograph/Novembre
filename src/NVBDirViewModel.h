//
// C++ Interface: NVBDirViewModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBDIRVIEWMODEL_H
#define NVBDIRVIEWMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QVector>
#include "NVBDirModel.h"

class NVBFile;
class NVBFileFactory;

/**
This class provides a list of files in the folder with pages. Loads on demand.
Unloads if slot defineWindow connected
*/
class NVBDirViewModel : public QAbstractItemModel
{
Q_OBJECT
public:

	NVBDirViewModel(NVBFileFactory * factory, NVBDirModel * model, QObject * parent = 0);
  virtual ~NVBDirViewModel();

  virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent ( const QModelIndex & index ) const;

	void setDisplayItems(QModelIndexList items);

	NVBAssociatedFilesInfo getAllFiles(const QModelIndex & index);

	virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
	virtual QStringList mimeTypes () const;

private:
  NVBFileFactory * fileFactory;
	QList<QPersistentModelIndex> indexes;
	QPersistentModelIndex dirindex;
  NVBDirModel * dirModel;
  mutable QVector<NVBFile*> files;
  mutable QList<int> unloadables;
  mutable QVector<int> rowcounts;
  QVector<QString> * fnamecache;
  bool operationRunning;
  void cacheRowCounts() const;
  void cacheRowCounts ( int first, int last ) const;

private slots:
  void parentInsertingRows( const QModelIndex & parent, int first, int last );
  void parentInsertedRows( const QModelIndex & parent, int first, int last );
  void parentRemovingRows( const QModelIndex & parent, int first, int last );
  void parentRemovedRows( const QModelIndex & parent, int first, int last );
//  void parentChangingLayout();
//  void parentChangedLayout();
  bool loadFile(int index) const;

public slots:
  void defineWindow(int start,int end);
};


#endif
