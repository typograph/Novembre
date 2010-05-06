//
// C++ Implementation: NVBPageViewModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBPageViewModel.h"

NVBPageViewModel::NVBPageViewModel( NVBWorkingArea * area):NVBViewController(area),lastAddedRow(-1)
{
  tools = qApp->property("toolsFactory").value<NVBToolsFactory*>();
//   icons = new NVBVizModel(this,NVB::IconView);
//   connect(icons,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));

  setSupportedDragActions(Qt::CopyAction);
}

NVBPageViewModel::~NVBPageViewModel()
{
  if (rowCount()) {
//    beginRemoveRows(QModelIndex(),0,rowCount()-1);
    while(!pages.isEmpty())
      removeRow(0);
//    endRemoveRows();
    }
//   if (icons) delete icons;
}

int NVBPageViewModel::rowCount(const QModelIndex & parent) const
{
  if (parent.isValid()) return 0;
  else return pages.count();
}

QVariant NVBPageViewModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (index.row() >= rowCount()) return QVariant();
//  if (role == Qt::CheckStateRole) return QVariant(Qt::Checked);
//  if (role == OriginalIndexRole) return QVariant(pages.at(index.row()).row());
  switch (role) {
    case Qt::DecorationRole : {
      return QIcon(icons.at(index.row()));
/*      QIcon * i = icons->index(index.row()).data(PageVizItemRole).value<NVBVizUnion>().IconViz;
      if (i)
        return QIcon(*i);
      else
        return QVariant();*/
      }
    default : return pageData(pages.at(index.row()),role);
    }
}

QVariant NVBPageViewModel::pageData(NVBDataSource* page, int role) const {
  switch (role) {
    case Qt::DisplayRole : 
    case Qt::EditRole    : {
                              return page->name();
                           }
    case Qt::StatusTipRole : 
    case Qt::ToolTipRole   : {
                              if (page->type() == NVB::TopoPage)
                                return 
                                 page->name() + 
                                 QString(" Topography ") + 
                                 QString::number(((NVB3DDataSource *)page)->resolution().width()) + QString("x") +
                                 QString::number(((NVB3DDataSource *)page)->resolution().height());
                              else if (page->type() == NVB::SpecPage)
                                return 
                                 page->name() + 
                                 QString(" Spectroscopy ") + 
                                 QString::number(((NVBSpecDataSource*)page)->datasize().height()) + QString(" curves, ") +
                                 QString::number(((NVBSpecDataSource*)page)->datasize().width()) + QString(" points/curve");
                              else
                                return QVariant();
                             }
    case PageRole           : {
                                return QVariant::fromValue(page);
                              }

    case PageTypeRole       : {
                                return QVariant::fromValue(page->type());
                              }
/*    
    case PageDataRole       : {
                                return QVariant();
                              }
*/
    case PagePositionRole   : {
                              if (page->type() == NVB::TopoPage)
                                return ((NVB3DDataSource*)page)->position();
                              else if (page->type() == NVB::SpecPage)
                                return ((NVBSpecDataSource*)page)->occupiedArea();
                              else
                                return QVariant();
                              }
    
    case PageDataSizeRole   : {
                              if (page->type() == NVB::TopoPage)
                                return ((NVB3DDataSource*)page)->position();
                              else if (page->type() == NVB::SpecPage)
                                return ((NVBSpecDataSource*)page)->datasize();
                              else
                                return QVariant();
    
                              }
    default : {
                return QVariant();
              }
    }
}

bool NVBPageViewModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid()) return false;
  if (role == PageRole) {
    releaseDataSource(pages.at(index.row()));
    pages.replace(index.row(),value.value<NVBDataSource*>());
    emit dataChanged(index,index);
    return true;
    }
//  if (role == Qt::CheckStateRole && value.toInt() == Qt::Unchecked) return removeRow(index.row());
  return false;
}

bool NVBPageViewModel::removeRow(int row, const QModelIndex & parent)
{
  if (parent.isValid()) return false;
  beginRemoveRows(QModelIndex(),row,row);
  releaseDataSource(pages.takeAt(row));
  icons.takeAt(row);
  endRemoveRows();
  return true;
}

void NVBPageViewModel::clear()
{
  if (rowCount()) {
    beginRemoveRows(QModelIndex(),0,rowCount()-1);
    while (!pages.isEmpty()) releaseDataSource(pages.takeFirst());
    icons.clear();
    endRemoveRows();
    }
}

Qt::ItemFlags NVBPageViewModel::flags(const QModelIndex & index) const
{
  if (index.isValid())
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;// | Qt::ItemIsUserCheckable;
  else
    return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled ;
}

void NVBPageViewModel::setSource(NVBDataSource * , NVBVizUnion )
{
  // Do nothing since there's no note which to set
}

void NVBPageViewModel::addSource(NVBDataSource * page, NVBVizUnion viz)
{
  addSource(page,0,viz);
}

void NVBPageViewModel::addSource(NVBDataSource * page, int row, NVBVizUnion viz)
{
  if (!page) return;

  if (row < 0) row = 0;
  if (row > pages.count()) row = pages.count();

  beginInsertRows(QModelIndex(),row,row);

  useDataSource(page);

  lastAddedRow = row;

  pages.insert(row,page);

  if (viz.valid)
    icons.insert(row,*(viz.IconViz));
  else {
    icons.insert(row,QIcon());
    tools->activateDefaultVisualizer(page,this);
    }

  endInsertRows();

  connect(page,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)));
  connect(page,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)),Qt::QueuedConnection);
}

void NVBPageViewModel::addSource(const QModelIndex & index)
{
  if (index.isValid())
    addSource(index.data(PageRole).value<NVBDataSource*>());
}

NVBViewController * NVBPageViewModel::openInNewWindow(NVBDataSource * , NVBVizUnion , NVB::ViewType )
{
// I have no idea how to use that
  return NULL;
}

void NVBPageViewModel::addControlWidget(QWidget * )
{
  // Do nothing since there's no controller for us
}

void NVBPageViewModel::setActiveVisualizer(NVBVizUnion )
{
  // Do nothing since there's no supervizs
}

void NVBPageViewModel::setVisualizer(NVBVizUnion visualizer)
{
  // This function will be called after a query for a new icon.
  // The position is defined by lastAddedRow.
//   icons->setVisualizer(visualizer,lastAddedRow);
  if (visualizer.valid)
    icons.replace(lastAddedRow,*(visualizer.IconViz));
}

void NVBPageViewModel::updateSource(NVBDataSource * newobj, NVBDataSource * oldobj)
{ // The icons will take care of themselves
  int i = pages.indexOf(oldobj);
  if (i >=0) {
    oldobj->disconnect(this);
    if (newobj) {
      pages.replace(i,newobj);
      connect(newobj,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)));
      connect(newobj,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)),Qt::QueuedConnection);
      emit dataChanged(index(i),index(i));
      }
    else {
      beginRemoveRows(QModelIndex(),i,i);
      pages.removeAt(i);
      endRemoveRows();
      }
    releaseDataSource(oldobj);
    }
  else
    NVBOutputError( "NVBPageViewModel::updateSource", "Page not in model" );
}

QMimeData * NVBPageViewModel::mimeData(const QModelIndexList & indexes) const
{
  if (indexes.count() > 1) {
    NVBOutputError("NVBPageViewModel::mimeData","Dragging more than one object");
    return 0;
    }

  if (indexes.isEmpty()) return 0;

  return new NVBDataSourceMimeData(NVBToolsFactory::hardlinkDataSource(indexes.at(0).data(PageRole).value<NVBDataSource*>()));
}

Qt::DropActions NVBPageViewModel::supportedDropActions() const
{
  return Qt::CopyAction | Qt::MoveAction;
}

QStringList NVBPageViewModel::mimeTypes() const
{
  return QStringList() << NVBDataSourceMimeData::dataSourceMimeType();
}

bool NVBPageViewModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
  Q_UNUSED(row);
  Q_UNUSED(column);
//   if (column != 0) return false;
//   if (parent.isValid()) return false;
  if (!data->hasFormat(NVBDataSourceMimeData::dataSourceMimeType()))
    return false;
  if (!(action & supportedDropActions())) return false;
  addSource(((NVBDataSourceMimeData*)data)->getPageData(),parent.isValid() ? parent.row() : rowCount());
  return true;
}
