//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBPAGETOOLBAR_H
#define NVBPAGETOOLBAR_H

#include <QList>
#include <QAction>
#include <QWidget>
#include <QToolBar>
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

struct ActionLimits {
	ActionLimits(NVB::PageType p = NVB::UndefinedPage,
	             NVB::ViewType v = NVB::AnyView)
		: pageType(p) , viewType(v) {;}
	NVB::PageType pageType;
	NVB::ViewType viewType;
	};

class NVBPageToolbar : public QToolBar {
		Q_OBJECT
	private:
		QList<ActionLimits> types;
		NVB::PageType currentPageType;
		NVB::ViewType currentViewType;
		void fillList();
		void adapt();
	public:
		inline NVBPageToolbar(const QString & title, QWidget * parent = 0)
			: QToolBar(title, parent), currentPageType(NVB::InvalidPage), currentViewType(NVB::NoView) {;}
		inline NVBPageToolbar(QWidget * parent = 0)
			: QToolBar(parent), currentPageType(NVB::InvalidPage), currentViewType(NVB::NoView) {;}
		virtual inline ~NVBPageToolbar() {;}
		inline QAction * addTopoPageAction(QAction * action) {
			return addActionWithType(action, NVB::TopoPage);
			}
		inline QAction * addSpecPageAction(QAction * action) {
			return addActionWithType(action, NVB::SpecPage);
			}
		QAction * addActionWithType(QAction * action, NVB::PageType ptype = NVB::UndefinedPage, NVB::ViewType vtype = NVB::AnyView);

	public slots:
		void switchPageType(NVB::PageType t) { currentPageType = t; adapt();	}
		void switchViewType(NVB::ViewType t) { currentViewType = t; adapt();	}
		void switchControlledWindow();

	};


#endif
