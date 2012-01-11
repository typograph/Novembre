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

/*!
* Stacks of the widgets corresponding to pages in NVBFileWindow.
* The widgets are organizeed into a tree model with one column per item.
* The children of the root are the pages, and their children are the widgets
* in order of adding.
*/ 
/*
* widgets keep the normal controlwidget information. parent.row corresponds to pagemodel_row, row to stack order
* with 0 being the bottom of the stack, parent must be valid
*/
class NVBWidgetStackModel : public QAbstractItemModel {
Q_OBJECT
private:
	/// Underlying model
	NVBVizModel * pagemodel;
	/// A list of widgets for every page.
	QList< QList< QWidget* > > widgets;
//  QList< QWidget* > vizwidgets;
public:
	NVBWidgetStackModel(NVBVizModel * model);
	~NVBWidgetStackModel();

	virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	/// \overload
	virtual QModelIndex index ( int row, const QModelIndex & parent = QModelIndex() ) const { return index(row,0,parent);}
	/// Always 1
	virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	/// Number of pages for invalid parent, number of installed widgets for a page.
	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent ( const QModelIndex & index ) const;
	/// Returns the widget at @p index as QVariant
	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	/// Swaps two items together with their widget stacks
	void swapItems(int row1, int row2);

public slots:
	/// Adds a \a page to the list of pages and propagates further
	int addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
	/// Add a \a widget to the page at @p index
	void addWidget(QWidget * widget, const QModelIndex & index);

	/// Adds pages inserted from a parent model
	void pagesInserted(const QModelIndex & index, int start, int end);
	/// Removes pages deleted from a parent model
	void pagesAboutToBeRemoved(const QModelIndex & index, int start, int end);

protected slots:
	/// Delete the reference to @p widget if it was deleted
	void widgetDestroyed(QObject * widget);

// signals:
//   void extraControlWidget(QWidget*);

};

#endif
