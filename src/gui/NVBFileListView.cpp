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

#include "NVBFileListView.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QHeaderView>

NVBFileListView::NVBFileListView(QWidget * parent): QTreeView(parent) {
	//    setMinimumSize(100,1);
	//    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);

	setHeader(new NVBFileListHeaderView(Qt::Horizontal, this));

	setRootIsDecorated(true);
	setAllColumnsShowFocus(true);
	setSortingEnabled(true);

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	sortByColumn(0, Qt::AscendingOrder);
//     header()->setMovable(false);
	header()->setCascadingSectionResizes(true);
	header()->setStretchLastSection(false);
	header()->setResizeMode(QHeaderView::ResizeToContents);
//    resizeColumns();
	}

void NVBFileListView::mousePressEvent(QMouseEvent *event) {
	// First we let the treeView do all selection changes
	QTreeView::mousePressEvent(event);

	// And now we check for right-click anywhere
	if (event->button() == Qt::RightButton)
		emit rightPressed(QPersistentModelIndex(indexAt(event->pos())));
	}

void NVBFileListHeaderView::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		event->accept();
		emit sectionRightPressed(logicalIndexAt(event->x()));
		}
	else
		QHeaderView::mousePressEvent(event);
	}
