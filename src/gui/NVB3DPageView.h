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
#ifndef NVB3DPAGEVIEW_H
#define NVB3DPAGEVIEW_H

#include <QStackedWidget>
#include "NVBDataView.h"
#include "../core/NVBPageViewModel.h"
#include "NVBVizModel.h"
#include "../core/NVBLogger.h"

class NVB3DPageView : public QStackedWidget, public NVBDataView {
//class NVB3DPageView : public Qwt3D::Plot3D, public NVBDataView {
		Q_OBJECT
	private:
		NVBVizModel * vizmodel;

		Qwt3D::Plot3D * currentPlot();

	public:
		NVB3DPageView(NVBVizModel* model, QWidget * parent);
		~NVB3DPageView();

		virtual bool hasWidthForHeight() const { return false; } // FIXME not entirely true, since there are constrains
		virtual int  widthForHeight(int) const { return -1; }

		virtual bool acceptsPage(NVBDataSource * source) const;

		virtual const QWidget * w() const { return this; }
		virtual QWidget * w() { return this; }

		virtual QToolBar * generateToolbar(QWidget * parent) const;
//   QAbstractListModel * getVizModel() { return vizmodel; }

	public slots :
		void select(const QModelIndex & index);
		void setActiveVisualizer(NVBVizUnion, const QModelIndex &) {;}   // I have no idea what it could be

		void exportImage();

	protected :

	protected slots:
//   virtual void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
		virtual void rowsRemoved(const QModelIndex & parent, int start, int end);
		virtual void rowsInserted(const QModelIndex & parent, int start, int end);
		virtual void updateVizs(const QModelIndex& start, const QModelIndex& end);

	signals :

	};

#endif
