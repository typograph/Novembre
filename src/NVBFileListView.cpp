#include "NVBFileListView.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QHeaderView>

NVBFileListView::NVBFileListView(QWidget * parent):QTreeView(parent) {
	//    setMinimumSize(100,1);
	//    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);

	setHeader(new NVBFileListHeaderView(Qt::Horizontal, this));

	setRootIsDecorated(true);
	setAllColumnsShowFocus(true);
	setSortingEnabled(true);

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	sortByColumn(0,Qt::AscendingOrder);
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
