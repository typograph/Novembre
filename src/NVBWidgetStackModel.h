//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

		virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
		/// \overload
		virtual QModelIndex index(int row, const QModelIndex & parent = QModelIndex()) const { return index(row, 0, parent);}
		/// Always 1
		virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
		/// Number of pages for invalid parent, number of installed widgets for a page.
		virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
		virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
		virtual QModelIndex parent(const QModelIndex & index) const;
		/// Returns the widget at @p index as QVariant
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	public slots:
		/// Add a \a widget to the page at @p index
		void addWidget(QWidget * widget, const QModelIndex & index);

		/// Adds pages inserted from a parent model
		void pagesInserted(const QModelIndex & index, int start, int end);
		/// Removes pages deleted from a parent model
		void pagesAboutToBeRemoved(const QModelIndex & index, int start, int end);

		/// Swaps two items together with their widget stacks
		void swapItems(int row1, int row2);

	protected slots:
		/// Delete the reference to @p widget if it was deleted
		void widgetDestroyed(QObject * widget);

// signals:
//   void extraControlWidget(QWidget*);

	};

#endif
