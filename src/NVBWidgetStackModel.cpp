//
// C++ Implementation: NVBWidgetStackModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "NVBWidgetStackModel.h"


NVBWidgetStackModel::NVBWidgetStackModel(NVBVizModel * model):pagemodel(model)
{
	connect(pagemodel,SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,int,int)),SLOT(pagesAboutToBeRemoved(const QModelIndex&,int,int)));
	connect(pagemodel,SIGNAL(rowsInserted(const QModelIndex&,int,int)),SLOT(pagesInserted(const QModelIndex&,int,int)));
//  connect(pagemodel,SIGNAL(dataChanged(const QModelIndex& , const QModelIndex& )),SLOT(pagesChanged(const QModelIndex&, const QModelIndex& )));

	if (pagemodel->rowCount())
		NVBOutputError("Parent page model not empty!");
//    pagesInserted(QModelIndex(),0,pagemodel->rowCount()-1);
}

NVBWidgetStackModel::~ NVBWidgetStackModel()
{
	if (rowCount())
		pagesAboutToBeRemoved(QModelIndex(),0,rowCount()-1);
// while (!widgets.at(0).isEmpty()) delete widgets.at(0).takeLast();
}

/**
 * @p Column of a valid index is 0.
 * @p Row corresponds to the order of pages in NVBPageViewModel and
 * to the order of insertion (0 is the oldest widget).
 * 
 */
QModelIndex NVBWidgetStackModel::index(int row, int column, const QModelIndex & parent) const
{
	if (column != 0) return QModelIndex();

	if (parent.isValid()) {
		if (parent.parent().isValid()) return QModelIndex();
		if (row >= widgets.at(parent.row()).size()) return QModelIndex();
		return createIndex(row,0,parent.row());
		}
	else {
		if (row < 0 || row >= widgets.size()) return QModelIndex();
		return createIndex(row,0,-1);
		}

}

/**
 * Adds @p page to the underlying model.
 * 
 */
int NVBWidgetStackModel::addSource(NVBDataSource *page, NVBVizUnion viz) {
	return pagemodel->addSource(page,viz);
	}

/**
 * Swaps two widget stacks at @p row1 and @p row2
 */
void NVBWidgetStackModel::swapItems(int row1, int row2) {
	if (!(row1 >= 0 && row2 >= 0 && row1 < rowCount() && row2 < rowCount())) {
		NVBOutputError("Swap targets not in model");
		return;
		}
	emit layoutAboutToBeChanged();
	widgets.swap(row1,row2);
	pagemodel->swapItems(row1,row2);
	emit layoutChanged();
	}

int NVBWidgetStackModel::columnCount(const QModelIndex & ) const
{
	return 1;
}

int NVBWidgetStackModel::rowCount(const QModelIndex & parent) const
{
	if (!parent.isValid())
		return widgets.size();
	else if (parent.isValid() && !parent.parent().isValid())
		return widgets.at(parent.row()).size();
	else
		return 0;
}

QModelIndex NVBWidgetStackModel::parent(const QModelIndex & index) const
{
	if (index.isValid() && index.internalId() != -1)
		return this->index(index.internalId());
	else
		return QModelIndex();
}

/**
 * For page indexes returns QVariant(). 
 */
QVariant NVBWidgetStackModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid()) return QVariant();
	if (index.column() > 0) return QVariant();
	if (role != Qt::DisplayRole) return QVariant();
	if (index.parent().isValid())
		return QVariant::fromValue(widgets.at(index.internalId()).at(index.row()));
	else
		return QVariant(); //::fromValue(vizwidgets.at(index.row()));
}

/**
 * Update the number of pages.
 */
void NVBWidgetStackModel::pagesInserted(const QModelIndex & index, int start, int end)
{
	if (index.isValid()) return;

	beginInsertRows(index,start,end);

	for (int i = start; i <= end; i++) {
		widgets.insert(i,QList< QWidget* >());
//    vizwidgets.insert(i,0); // Insert empty widgets
		}

	endInsertRows();

}

/**
 * Remove the widgets for the deleted pages. 
 */
void NVBWidgetStackModel::pagesAboutToBeRemoved(const QModelIndex & index, int start, int end)
{
	if (index.isValid()) return;

	beginRemoveRows(index,start,end);

	for (int i = start; i <= end; i++) {
		while (!widgets.at(start).isEmpty()) {
			QWidget * w = widgets[start].takeLast();
			if (w) delete w;
			}
		widgets.removeAt(start);
//     if (vizwidgets.at(start)) delete vizwidgets.at(start);
//     vizwidgets.removeAt(start);
		}

	endRemoveRows();

}

/**
 * The widget is appended to the list for the page.
 */
void NVBWidgetStackModel::addWidget(QWidget * widget, const QModelIndex & index)
{
	if (widget && index.isValid() && index.row() < rowCount()) {
		beginInsertRows(this->index(index.row()),widgets.at(index.row()).size(),widgets.at(index.row()).size());
		widgets[index.row()].append(widget);
		connect(widget,SIGNAL(destroyed(QObject*)),SLOT(widgetDestroyed(QObject*)));
		endInsertRows();
		}
}

/*
void NVBWidgetStackModel::setVizWidget(QWidget * widget, const QModelIndex & index)
{
	if (widget) {
		if (vizwidgets.at(index.row())) delete vizwidgets.takeAt(index.row());
		vizwidgets.insert(index.row(),widget);
		emit dataChanged(index,index);
		}
}
*/

bool NVBWidgetStackModel::hasChildren(const QModelIndex & parent) const
{
	return !parent.isValid() || parent.internalId() == -1;
}

void NVBWidgetStackModel::widgetDestroyed(QObject * widget)
{
	if (!widget->isWidgetType()) return;
	for(int i = 0; i < widgets.size(); i++) {
		int index;
		if ((index = widgets.at(i).indexOf(qobject_cast<QWidget*>(widget))) >= 0 ) {
			beginRemoveRows(QModelIndex(),index,index);
			widgets[i].removeAt(index);
			endRemoveRows();
			return;
			}
		}
}
