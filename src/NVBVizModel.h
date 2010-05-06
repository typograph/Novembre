//
// C++ Interface: NVBVizModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBVIZMODEL_H
#define NVBVIZMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "NVBGeneralDelegate.h"

class NVBVizModel : public QAbstractListModel {
Q_OBJECT
private:
  QList<NVBVizUnion> vizs;
  QAbstractListModel * pagemodel;
  NVB::ViewType viewtype;
protected:

public:

  NVBVizModel(QAbstractListModel* model, NVB::ViewType vtype);
  virtual ~NVBVizModel();

  virtual int rowCount(const QModelIndex & index = QModelIndex()) const {
    if (index.isValid()) return 0;
    return vizs.size(); }
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) { return QVariant();}

  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//  void addViz(NVBVizUnion u = NVBVizUnion(), int row = 1);

public slots:

  void setVisualizer(NVBVizUnion visualizer, const QModelIndex& index);
  void setVisualizer(NVBVizUnion visualizer, int row = 0);

protected slots:
  virtual void pagesAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
  virtual void pagesRemoved ( const QModelIndex & parent, int start, int end );
  virtual void pagesAboutToBeInserted ( const QModelIndex & parent, int start, int end );
  virtual void pagesInserted ( const QModelIndex & parent, int start, int end );
  virtual void pagesChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
};


#endif
