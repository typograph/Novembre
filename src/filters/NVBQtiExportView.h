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
#ifndef NVBQTIEXPORTVIEW_H
#define NVBQTIEXPORTVIEW_H

#include "../core/NVBViewController.h"
#include <QListWidget>
#include <QFile>
#include "NVBQtiTableDialog.h"
#if QT_VERSION >= 0x040300
#include <QMdiSubWindow>
#endif

class QString;

/**
	@author Timofey <timoty@pi-balashov>
*/
#if QT_VERSION >= 0x040300
class NVBQtiExportView : public QMdiSubWindow, public NVBViewController
#else
class NVBQtiExportView : public QWidget, public NVBViewController
#endif
	{
	private:
		QFile * qti_file;
		QListWidget * list;
		NVBQtiTableDialog dialog;
//   QLineEdit * c_line;

	public:
		NVBQtiExportView(NVBWorkingArea * area, QString filename);
		~NVBQtiExportView();

		virtual NVB::ViewType viewType() { return NVB::ListView; }

		virtual void setSource(NVBDataSource * page, NVBVizUnion = NVBVizUnion()) { addSource(page); }
		virtual void addSource(NVBDataSource * , NVBVizUnion  = NVBVizUnion());

		virtual void setVisualizer(NVBVizUnion) {;}
		virtual void addControlWidget(QWidget *) {;}
		virtual void setActiveVisualizer(NVBVizUnion) {;}

		/*
		  virtual void addControlWidget(QWidget * widget) =0;
		  virtual void setControlWidget(QWidget * widget) =0;
		*/

		virtual NVBViewController * openInNewWindow(NVBDataSource * , NVBVizUnion  = NVBVizUnion(), NVB::ViewType  = NVB::DefaultView) { return 0;}

	};

#endif
