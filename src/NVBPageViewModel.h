//
// C++ Interface: NVBPageViewModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBPAGEVIEWMODEL_H
#define NVBPAGEVIEWMODEL_H

#include <QAbstractListModel>
#include "NVBGeneralDelegate.h"
#include "NVBViewController.h"
#include "NVBToolsFactory.h"
#include "NVBMimeData.h"

/**
	@author Timofey <timoty@pi-balashov>
*/
class NVBPageViewModel : public QAbstractListModel {
Q_OBJECT
private:
  int lastAddedRow;
protected:
  NVBToolsFactory * tools;
  QList<QIcon> icons;
  QList<NVBDataSource*> pages;
  virtual QVariant pageData(NVBDataSource* page, int role) const;
public:
	NVBPageViewModel();
  virtual ~NVBPageViewModel();

  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

  virtual bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

  virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
  virtual Qt::DropActions supportedDropActions () const;
  virtual QStringList mimeTypes () const;
  virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

  void clear();

  virtual NVB::ViewType viewType() { return NVB::IconView; }

  virtual void setSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
	int addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
  void addSource(NVBDataSource * page, int row, NVBVizUnion viz = NVBVizUnion());
	void addSource(const QModelIndex & index);

	virtual void swapItems(int row1, int row2);

protected slots:
  virtual void updateSource( NVBDataSource * newobj, NVBDataSource * oldobj);
};

#endif
