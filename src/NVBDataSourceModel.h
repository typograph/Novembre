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

#include <QtCore/QAbstractListModel>
#include "NVBDataSource.h"

/**
	@author Timofey <timoty@pi-balashov>
*/
class NVBDataSourceModel : public QAbstractListModel {
Q_OBJECT
public:
	enum Mode { Direct, Crossed };
private:
protected:
  QList<QIcon> icons;
	const NVBDataSource * source;
	QList< QList<axisindex_t> > crosses;
	Mode m;
protected slots:
	void setSource(const NVBDataSource * s);
	void calculateCrosses();
	void initIcons();
	void parentAboutToReform();
	void parentReformed();
public:
	
  NVBDataSourceModel(const NVBDataSource * source);
  virtual ~NVBDataSourceModel();

  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
//  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

//  virtual bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

  virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
  virtual QMimeData * mimeData ( const QModelIndex & index ) const;
//  virtual Qt::DropActions supportedDropActions () const;
  virtual QStringList mimeTypes () const;
//  virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
/*
protected slots:
	void dataSourceChanged();
	void dataSourceReplaced( NVBDataSource * newobj, NVBDataSource * oldobj);
	*/
};

typedef QPair<NVBDataSourceModel *, int> NVBDSLModelSubIndex;

class NVBDataSourceListModel : public QAbstractListModel {
Q_OBJECT;
private:
	QList<NVBDataSourceModel*> models;
	QList<int> cachecounts;

	NVBDSLModelSubIndex map(int k) const;

public:
	explicit NVBDataSourceListModel(QList< NVBDataSource * > sources);
	virtual ~NVBDataSourceListModel();

  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

  virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
  virtual QStringList mimeTypes () const;
	
private slots:
	void subDataChanged(QModelIndex,QModelIndex);
	void	subModelAboutToBeReset ();
	void	subModelReset ();
	void	subRowsAboutToBeInserted ( const QModelIndex & parent, int start, int end );
	void	subRowsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );
	void	subRowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
	void	subRowsInserted ( const QModelIndex & parent, int start, int end );
	void	subRowsMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );
	void	subRowsRemoved ( const QModelIndex & parent, int start, int end );

};

#endif
