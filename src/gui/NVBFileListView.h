//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBFILELISTVIEW_H
#define NVBFILELISTVIEW_H

#include <QtGui/QTreeView>
#include <QtGui/QHeaderView>

class NVBFileListView : public QTreeView {
		Q_OBJECT
	public:
		NVBFileListView(QWidget * parent = 0);
		virtual ~NVBFileListView() {;}
	protected:
		virtual void mousePressEvent(QMouseEvent *event);
	signals:
		void rightPressed(const QModelIndex &);
	};

class NVBFileListHeaderView : public QHeaderView {
		Q_OBJECT
	public:
		NVBFileListHeaderView(Qt::Orientation orientation, QWidget * parent = 0): QHeaderView(orientation, parent) {;}
		virtual ~NVBFileListHeaderView() {;}
	protected:
		virtual void mousePressEvent(QMouseEvent *event);
	signals:
		void sectionRightPressed(int);
	};

#endif // NVBFILELISTVIEW_H
