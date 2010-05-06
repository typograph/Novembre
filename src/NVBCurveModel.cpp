//
// C++ Implementation: NVBCurveModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBCurveModel.h"

int NVBCurveModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return provider->datasize().height();
}

QVariant NVBCurveModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();
  switch (role) {
    case Qt::DisplayRole : return QString("Curve %1").arg(index.row());
    case Qt::DecorationRole : return QColor(provider->getColorModel()->colorize(index.row()));
    default : return QVariant();
    }
}

NVBCurveModel::NVBCurveModel(NVBSpecDataSource * source):QAbstractListModel(),provider(source)
{

  if (!provider) throw;

  connect(provider,SIGNAL(dataChanged()),SLOT(resetModel()));
  connect(provider,SIGNAL(colorsChanged()),SLOT(updateModel()));
  connect(provider,SIGNAL(colorsAdjusted()),SLOT(updateModel()));
//  connect(provider,SIGNAL(dataChanged()),SLOT());

}

void NVBCurveModel::updateModel()
{
  emit dataChanged(index(0),index(rowCount()));
}

void NVBCurveModel::resetModel()
{
  reset();
}
