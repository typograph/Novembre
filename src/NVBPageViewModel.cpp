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

NVBPageViewModel::NVBPageViewModel():QAbstractListModel(),lastAddedRow(-1)
{
	tools = qApp->property("toolsFactory").value<NVBToolsFactory*>();
	setSupportedDragActions(Qt::CopyAction);
}

NVBPageViewModel::~NVBPageViewModel()
{
	clear();
// 	if (rowCount()) {
// 		while(!pages.isEmpty())
// 			removeRow(0);
// 		}
}

int NVBPageViewModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid()) return 0;
	else return pages.count();
}

/**
 * NVBPageViewModel defines a set of special roles, that are
 * \li \c PageRole \returns the page itself 
 * \li \c PageTypeRole \returns the type of the page
 * \li \c PagePositionRole \returns the bounding rect of the page
 * \li \c PageDataSizeRole \returns the shape of the data of the page
 */
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

/**
 * This function is useful in subclasses, "standartizing" the data for page roles.
 * Note that it does not return anything for Qt::DecorationRole, as the icons
 * returned by NVBPageViewModel are local to the model, and are not completely defined
 * by the page. 
 */
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
		case PageRole : {
			return QVariant::fromValue(page);
			}

		case PageTypeRole : {
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

/**
 * This function will only work for setting \c PageRole, i.e. replacing the whole page 
 */
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

/**
 * All items in the model are always enabled, editable and drag-n-drop-able.
 * 
 */
Qt::ItemFlags NVBPageViewModel::flags(const QModelIndex & index) const
{
	if (index.isValid())
		return QAbstractListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;// | Qt::ItemIsUserCheckable;
	else
		return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled ;
}

/**
 * The @p viz parameter should specify an icon for the added page.
 * If none is provided, the model will use the default one.
 */
int NVBPageViewModel::addSource(NVBDataSource * page)
{
	addSource(page,0);
	return 0;
}

/**
 * \overload NVBPageViewModel::addSource
 */
void NVBPageViewModel::addSource(NVBDataSource* page, int row)
{
	if (!page) return;

	if (row < 0) row = 0;
	if (row > pages.count()) row = pages.count();

	beginInsertRows(QModelIndex(),row,row);

	useDataSource(page);

	lastAddedRow = row;

	pages.insert(row,page);

	icons.insert(row,tools->getDefaultIcon(page));

	endInsertRows();

	connect(page,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)));
	connect(page,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(updateSource(NVBDataSource*, NVBDataSource*)),Qt::QueuedConnection);
}

/**
 * \overload NVBPageViewModel::addSource
 */
void NVBPageViewModel::addSource(const QModelIndex & index)
{
	if (index.isValid())
		addSource(index.data(PageRole).value<NVBDataSource*>());
}

/**
 * The icons should update automatically
 */
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
		NVBOutputError( "Page not in model" );
}

/**
 * The function will not work if there are multiple items in @p indexes.
 */
QMimeData * NVBPageViewModel::mimeData(const QModelIndexList & indexes) const
{
	if (indexes.count() > 1) {
		NVBOutputError("Dragging more than one object");
		return 0;
		}

	if (indexes.isEmpty()) return 0;

	return new NVBDataSourceMimeData(NVBToolsFactory::hardlinkDataSource(indexes.at(0).data(PageRole).value<NVBDataSource*>()));
}

Qt::DropActions NVBPageViewModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

/**
 * Thie model supports NVBDataSourceMimeData::dataSourceMimeType
 * \sa NVBDataSourceMimeData
 */
QStringList NVBPageViewModel::mimeTypes() const
{
	return QStringList() << NVBDataSourceMimeData::dataSourceMimeType();
}

/**
 * If a valid page is dropped, it will be inserted in the model either right before @p parent,
 * (i.e. user dropped the page on an item or right before it), or at the end of the list,
 * in case the user dropped it there (@p parent is invalid)
 */
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
