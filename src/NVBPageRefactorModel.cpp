//
// C++ Implementation: NVBPageRefactorModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBPageRefactorModel.h"
#include "NVBPageViewModel.h"

#include <QPainter>

#if QT_VERSION >= 0x040300
#include <QIconEngineV2>
class NVBMixTSIcon : public QObject, public QIconEngineV2 {
public :
  NVBMixTSIcon(NVB3DDataSource* topo, NVBSpecDataSource* spec):QIconEngineV2(),
#else
#include <QIconEngine>
class NVBMixTSIcon : public QObject, public QIconEngine {
public :
  NVBMixTSIcon(NVB3DDataSource* topo, NVBSpecDataSource* spec):QIconEngine(),
#endif
  cache(0),stopo(topo),sspec(spec) { 
    if (!stopo || !sspec) throw;
    cache = stopo->getColorModel()->colorize( stopo->getData(), stopo->resolution() );
    if (!cache) throw;
    }
  virtual ~NVBMixTSIcon() { if (cache) delete (cache);}

  virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) {
    painter->drawImage(rect,*cache);
    // Paint dots

    painter->save();
    painter->setPen(QPen(Qt::blue));
    painter->setBrush(Qt::blue);

    scaler<double,int> w(stopo->position().left(),stopo->position().right(),rect.left(),rect.right());
    scaler<double,int> h(stopo->position().top(),stopo->position().bottom(),rect.top(),rect.bottom());
    
    foreach( QPointF p,sspec->positions()) {
      painter->drawEllipse(w.scale(p.x())-1,h.scale(p.y())-1,2,2);
      }

    painter->restore();

    }

protected:
//  NVBDataSource * provider;
  QImage* cache;
  NVB3DDataSource* stopo;
  NVBSpecDataSource* sspec;
};


int NVBPageRefactorModel::rowCount(const QModelIndex & parent) const
{
  switch(mode) {
    case None : return source->rowCount(parent);
    case MarkSpectra : return source->rowCount(parent);
    case Empty :
    default :
      return 0;
    }
}

QVariant NVBPageRefactorModel::data(const QModelIndex & index, int role) const
{
  switch(mode) {
    case None : return source->data(index,role);
    case MarkSpectra : {
      if (!icons.isEmpty() && role == Qt::DecorationRole)
        return QIcon(icons.at(index.row()));
      else 
        return source->data(index,role);
      }
    case Empty :
    default :
      return QVariant();
    }
}

bool NVBPageRefactorModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  switch(mode) {
    case None : 
    case MarkSpectra : 
      return source->setData(index,value,role);
    case Empty :
    default :
      return false;
    }
}

Qt::ItemFlags NVBPageRefactorModel::flags(const QModelIndex & index) const
{
//   if (!index.isValid()) return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  switch(mode) {
    case None : 
    case MarkSpectra : 
      return source->flags(index) | Qt::ItemIsSelectable;
    case Empty :
    default :
//       return Qt::NoItemFlags;
      return Qt::ItemFlags();
//       return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

bool NVBPageRefactorModel::removeRow(int row, const QModelIndex & parent)
{
  switch(mode) {
    case None :
    case MarkSpectra :
      return source->removeRow(row,parent);
    case Empty :
    default :
      return false;
    }
}

QMimeData * NVBPageRefactorModel::mimeData(const QModelIndexList & indexes) const
{
  switch(mode) {
    case None :
    case MarkSpectra :
      return source->mimeData(indexes);
    case Empty :
    default :
      return 0;
    }
}

Qt::DropActions NVBPageRefactorModel::supportedDropActions() const
{
  switch(mode) {
    case None :
    case MarkSpectra : 
      return source->supportedDropActions();
    case Empty :
    default :
      return Qt::IgnoreAction;
    }
}

QStringList NVBPageRefactorModel::mimeTypes() const
{
  switch(mode) {
    case None :
    case MarkSpectra :
      return source->mimeTypes();
    case Empty :
    default :
      return QStringList();
    }
}

bool NVBPageRefactorModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
  switch(mode) {
    case None :
    case MarkSpectra :
      return source->dropMimeData(data,action,row,column,parent);
    case Empty :
    default :
      return false;
    }
}

void NVBPageRefactorModel::setModel(NVBPageViewModel * model)
{
  clear();
  source = model;
  if (source && mode == Empty) {
    mode = bkup_mode;
    bkup_mode = None;
    }
  initialize();
}

void NVBPageRefactorModel::setMode(RefactorMode new_mode)
{
  if (source) {
    mode = new_mode;
    initialize();
    }
  else {
    bkup_mode = mode;
    }
}

void NVBPageRefactorModel::clear()
{
  if (mode != Empty) {
    beginRemoveRows(QModelIndex(),0,rowCount(QModelIndex())-1);
    bkup_mode = mode;
    mode = Empty;
    endRemoveRows();
    }
  icons.clear();
}

void NVBPageRefactorModel::initialize()
{
  if (mode == MarkSpectra) {
    NVBSpecDataSource * specnote = 0;
    for (int i=0; i < source->rowCount(QModelIndex()); i++) {
      if (source->data(source->index(i),PageTypeRole).value<NVB::PageType>() == NVB::SpecPage) {
        specnote = (NVBSpecDataSource*)(source->data(source->index(i),PageRole).value<NVBDataSource*>());
        break;
        }
      }
    if (!specnote) return;
    for (int i=0; i < source->rowCount(QModelIndex()); i++) {
      if (source->data(source->index(i),PageTypeRole).value<NVB::PageType>() == NVB::TopoPage) {
        icons << QIcon(new NVBMixTSIcon((NVB3DDataSource*)(source->data(source->index(i),PageRole).value<NVBDataSource*>()),specnote));
        }
      else {
        icons << source->data(source->index(i),Qt::DecorationRole).value<QIcon>();
        }
      }
    }
  reset();
}

// QModelIndex NVBPageRefactorModel::index(int row, int column, const QModelIndex & parent) const  {
//   if (mode == Empty) return QModelIndex();
//   return source->index(row,column,parent);
// }
