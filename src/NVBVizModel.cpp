//
// C++ Implementation: NVBVizModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBVizModel.h"

NVBVizModel::NVBVizModel(QAbstractListModel * model, NVB::ViewType vtype):QAbstractListModel(),pagemodel(model),viewtype(vtype)
{
  connect(pagemodel,SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,int,int)),
                      SLOT(pagesAboutToBeRemoved(const QModelIndex&,int,int)));
  connect(pagemodel,SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                      SLOT(pagesRemoved(const QModelIndex&,int,int)));
  connect(pagemodel,SIGNAL(rowsAboutToBeInserted(const QModelIndex&,int,int)),
                      SLOT(pagesAboutToBeInserted(const QModelIndex&,int,int)));
  connect(pagemodel,SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                      SLOT(pagesInserted(const QModelIndex&,int,int)));
  connect(pagemodel,SIGNAL(dataChanged(const QModelIndex& , const QModelIndex& )),
                      SLOT(pagesChanged(const QModelIndex&, const QModelIndex& )));
//   vizs.append(NVBVizUnion()); // Invalid union

  if (pagemodel->rowCount())
    NVBOutputError("NVBVizModel::NVBVizModel","Parent page model not empty!");
//    pagesInserted(QModelIndex(),0,pagemodel->rowCount()-1);

}

void NVBVizModel::pagesAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
  if (parent.isValid()) return;

  beginRemoveRows(parent,start,end);

  for (int i = start; i <= end; i++) {
    vizs.takeAt(start).clear();
    }

}

void NVBVizModel::pagesInserted(const QModelIndex & parent, int start, int end)
{
  if (parent.isValid()) return;

  NVBVizUnion u; // Invalid union

  for (int i = start; i <= end; i++) {
    vizs.insert(i,u);
    }

  endInsertRows();
}

void NVBVizModel::pagesChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
  if (!topLeft.isValid() || !bottomRight.isValid()) return;
  for (int i = topLeft.row(); i<= bottomRight.row(); i++) {
    if (vizs.at(i).valid && vizs.at(i).ptype != pagemodel->index(i).data(PageTypeRole).value<NVB::PageType>()) {
      vizs[i].clear();
      vizs.replace(i,NVBVizUnion());
      emit dataChanged(index(i),index(i));
      }
    }
}

QVariant NVBVizModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (index.row() >= vizs.size()) return QVariant();
//  if (role != Qt::DisplayRole) return QVariant();
  switch (role) {
    case PageVizItemRole: return QVariant::fromValue(vizs.at(index.row()));;
    default: return pagemodel->data(index, role);
    }
}

Qt::ItemFlags NVBVizModel::flags(const QModelIndex & index) const
{
  if (index.isValid())
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
  else
    return QAbstractListModel::flags(index);
}

bool NVBVizModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid()) return false;

  switch(role) {
    case PageVizItemRole: {
      setVisualizer(value.value<NVBVizUnion>(),index.row());
      return true;
      }
    default : return false;
    }
}

NVBVizModel::~ NVBVizModel()
{
  if (rowCount()) {
    beginRemoveRows(QModelIndex(),0,rowCount()-1);
    while(!vizs.isEmpty()) vizs.takeLast().clear();
    endRemoveRows();
    }
}

void NVBVizModel::setVisualizer(NVBVizUnion visualizer, const QModelIndex & index)
{
  if (index.isValid()) {
    vizs.replace(index.row(),visualizer);
    emit dataChanged(index,index);
    }
}

void NVBVizModel::setVisualizer(NVBVizUnion visualizer, int row)
{
  if (row < 0 || row >= rowCount()) return;
  vizs.replace(row,visualizer);
  emit dataChanged(index(row),index(row));
}

void NVBVizModel::pagesAboutToBeInserted(const QModelIndex & parent, int start, int end)
{
  if (parent.isValid()) return;

  beginInsertRows(parent,start,end);
}

void NVBVizModel::pagesRemoved(const QModelIndex & parent, int start, int end)
{
  Q_UNUSED(start);
  Q_UNUSED(end);
  if (parent.isValid()) return;

  endRemoveRows();
}
