//
// C++ Interface: NVBWidgetStackModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBWIDGETSTACKMODEL_H
#define NVBWIDGETSTACKMODEL_H

#include <QList>
#include <QWidget>
#include <QAbstractItemModel>

#include "NVBVizModel.h"

/**
  * widgets keep the normal controlwidget information. parent.row corresponds to pagemodel_row, row to stack order
  * with 0 being the bottom of the stack, parent must be valid
  */

class NVBWidgetStackModel : public QAbstractItemModel {
Q_OBJECT
private:
	NVBVizModel * pagemodel;
  QList< QList< QWidget* > > widgets;
//  QList< QWidget* > vizwidgets;
public:
	NVBWidgetStackModel(NVBVizModel * model);
  ~NVBWidgetStackModel();

  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex index ( int row, const QModelIndex & parent = QModelIndex() ) const { return index(row,0,parent);}
  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent ( const QModelIndex & index ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

public slots:
	int addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
  void addWidget(QWidget * widget, const QModelIndex & index = QModelIndex());
//  void addWidget(QWidget * widget, const QModelIndex & index = QModelIndex());
//   void setVizWidget(QWidget * widget, const QModelIndex & index);

  void pagesInserted(const QModelIndex & index, int start, int end);
  void pagesAboutToBeRemoved(const QModelIndex & index, int start, int end);

protected slots:
  void widgetDestroyed(QObject * widget);

// signals:
//   void extraControlWidget(QWidget*);

};

#endif
