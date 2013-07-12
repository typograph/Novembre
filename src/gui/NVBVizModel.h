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
#ifndef NVBVIZMODEL_H
#define NVBVIZMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QPair>
#include "../core/NVBGeneralDelegate.h"

class NVBPageViewModel;

/*!
* NVBVizModel stores and delivers visualizers for pages in a NVBPageViewModel.
*
* It is used exclusively by NVBFileWindow.
*/
class NVBVizModel : public QAbstractListModel {
		Q_OBJECT
	private:
		/// The type of the view this model belongs to.
		NVB::ViewType viewtype;
		/// All the visualizers.
		QList<NVBVizUnion> vizs;
	protected:
		/// NVBPageViewModel this model is sitting on top of
		NVBPageViewModel * pagemodel;
	public:

		/// Create NVBVizModel for a view of type @p type on top of \a model.
		NVBVizModel(NVBPageViewModel* model, NVB::ViewType vtype);
		virtual ~NVBVizModel();
		/// The number of pages in the model
		virtual int rowCount(const QModelIndex & index = QModelIndex()) const {
			if (index.isValid()) return 0;

			return vizs.size();
			}
		/// The same data as NVBPageViewModel would deliver.
		virtual QVariant data(const QModelIndex &index, int role) const;
		virtual QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) { return QVariant();}

		/// All the items are editable
		virtual Qt::ItemFlags flags(const QModelIndex &index) const;
		/// Set a visualizer for the page at @p index
		virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//  void addViz(NVBVizUnion u = NVBVizUnion(), int row = 1);

	public slots:
		/// Set a visualizer for page at @p index
		void setVisualizer(NVBVizUnion visualizer, const QModelIndex& index);
		/// Set a visualizer for page at @p row
		void setVisualizer(NVBVizUnion visualizer, int row = 0);
		/// Swap two pages at @p row1 and @p row2
		void swapItems(int row1, int row2);

	protected slots:
		/// Prepare for pages being removed from the list
		virtual void pagesAboutToBeRemoved(const QModelIndex & parent, int start, int end);
		/// Inform about reaching a stable state.
		virtual void pagesRemoved(const QModelIndex & parent, int start, int end);
		/// Prepare for pages being inserted into the list
		virtual void pagesAboutToBeInserted(const QModelIndex & parent, int start, int end);
		/// Insert pages into the list
		virtual void pagesInserted(const QModelIndex & parent, int start, int end);
		/// Request new visualizers if page type changed
		virtual void pagesChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

	signals:
		/// Emitted after swapItems
		void itemsSwapped(int row, int row2);
	};


#endif
