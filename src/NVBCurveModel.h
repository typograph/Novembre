//
// C++ Interface: NVBCurveModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBCURVEMODEL_H
#define NVBCURVEMODEL_H

#include <QAbstractListModel>
#include "NVBDataSource.h"

class NVBCurveModel : public QAbstractListModel {
Q_OBJECT
private:
  NVBSpecDataSource * provider;
public:
  NVBCurveModel(NVBSpecDataSource * source);
  virtual ~NVBCurveModel() {;}

  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
public slots:
  void updateModel();
  void resetModel();
};

#endif

