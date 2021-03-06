//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#include "NVBLogger.h"
#include "NVBDirView.h"
#include <QtGui/QPaintEvent>
#include <QtGui/QRegion>
#include <QtCore/QRect>
#include <QtCore/QPoint>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QScrollBar>
#include <QtGui/QStyleOptionTitleBar>
#include <QtGui/QFont>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>


NVBDirView::NVBDirView(QWidget * parent): QAbstractItemView(parent) {
	keepItemsOnModelChanges = false;
	ignoreScroll = false;
	oneItemPerFile = false;
	top_row = 0;
	pages_per_row = 0;
	soft_shift = 0;
	voffset = 0;
	setGridSize(iconSize() + QSize(20, 20));
	setDragEnabled(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	verticalScrollBar()->setEnabled(false);
	}

NVBDirView::~ NVBDirView() {
	}

QModelIndex NVBDirView::indexAt(const QPoint & point) const {
	if (!model())
		return QModelIndex();

	if ( point.x() < leftMargin() || point.x() >= leftMargin() + pages_per_row * gridSize.width()) // in left/right margins
		return QModelIndex();

	int py = point.y() + soft_shift - midMargin();

	if (py < 0) // Between items
		return QModelIndex();

	if (oneItemPerFile) {
		return model()->index(
		         top_row + // Start index
		         (int)((point.x() - leftMargin()) / gridSize.width()) + // x shift
		         pages_per_row * (int)(py / gridSize.height()) // y shift
		         , 0);
		}

	int row = top_row;

	// find the row below
	for(int h = fileHeight(row); py > h && row < model()->rowCount(); h = fileHeight(++row)) {
		py -= h;
		py -= midMargin();

		if (py < 0) // between items
			return QModelIndex();
		}

	if (row >= model()->rowCount())
		return QModelIndex();

	QModelIndex cr = model()->index(row, 0);

	py -= headerHeight();

	if ( py < 0) // in header
		return cr;

	py -= headerMargin();

	if ( py < 0) // between header and items
		return QModelIndex();

	if (pages_per_row) {
		int index = (point.x() - leftMargin()) / gridSize.width();
		int rc = model()->rowCount(cr);
		int lc = 1 + (rc - 1) / pages_per_row;

		for(int i = 0; py >= 0 && i < lc; i++ ) {
			py -= gridSize.height();
			index += pages_per_row;
			}

		if (py < 0)
			return model()->index(index - pages_per_row, 0, cr);
		}

	return QModelIndex();

	}

void NVBDirView::scrollTo(const QModelIndex & index, ScrollHint hint) {
	if (!model() || !index.isValid())
		return;

	bool isAPage = index.parent().isValid() && pages_per_row;
	int file_row = isAPage ? index.parent().row() : index.row();

	switch (hint) {
		case QAbstractItemView::PositionAtTop : {
			if (isAPage && index.row() >= pages_per_row)
				verticalScrollBar()->setValue(voffset - soft_shift + fileDistance(top_row, file_row) + (index.row() / pages_per_row)*gridSize.height() + headerHeight() + headerMargin() );
			else
				verticalScrollBar()->setValue(voffset - soft_shift + fileDistance(top_row, file_row) );

			break;
			}

		case QAbstractItemView::PositionAtBottom : {
			// First, get the necessary y dimension of the interesting index
			int new_row = file_row;
			int new_shift = 0;

			if (isAPage)
				new_shift = (1 + index.row() / pages_per_row) * gridSize.height() + headerHeight() + headerMargin();
			else
				new_shift = fileHeight(top_row);

			// Now calculate how many more we have to include
			while (new_row > 0 && new_shift < viewportHeight()) {
				new_row -= 1;
				new_shift += midMargin() + fileHeight(new_row);
				}

			if (new_shift >= viewportHeight())
				verticalScrollBar()->setValue(voffset - soft_shift + fileDistance(top_row, new_row) + new_shift - viewportHeight());
			else // we have to put row 0 on top, no more rows there
				verticalScrollBar()->setValue(0);

			break;
			}

		case QAbstractItemView::PositionAtCenter : {
			// First, get the necessary dimension of the interesting index
			int new_row = file_row;
			int new_shift = 0;

			if (isAPage)
				new_shift = (index.row() / pages_per_row) * gridSize.height() + gridSize.height() / 2 + headerHeight() + headerMargin();
			else
				new_shift = fileHeight(top_row) / 2;

			// Now calculate how many more we have to include
			while (new_row > 0 && new_shift < viewportHeight()) {
				new_row -= 1;
				new_shift += midMargin() + fileHeight(top_row);
				}

			if (new_shift >= viewportHeight())
				verticalScrollBar()->setValue(voffset - soft_shift + fileDistance(top_row, new_row) + new_shift - viewportHeight());
			else // we have to put row 0 on top, no more rows there
				verticalScrollBar()->setValue(0);

			break;
			}

		case QAbstractItemView::EnsureVisible : {
			if (file_row < top_row) // file somewhere above
				scrollTo(index, QAbstractItemView::PositionAtTop);
			else if (file_row == top_row) { // file already here
// 				setDirtyRegion(viewport()->rect());
				if (isAPage && index.row() >= pages_per_row) {
					int itop = (index.row() / pages_per_row) * gridSize.height() + headerHeight() + headerMargin();
					int ibottom = itop + gridSize.height();

					if (soft_shift > itop) // top of the page at least partially obscured
						verticalScrollBar()->setValue(voffset - soft_shift + itop);
					else if (soft_shift + viewportHeight() < ibottom) // bottom of page at least partially obscured
						verticalScrollBar()->setValue(voffset + ibottom - viewportHeight() - soft_shift);
					}
				else {
// 					setDirtyRegion(viewport()->rect());
					verticalScrollBar()->setValue(voffset - soft_shift);
//					voffset -= soft_shift;
//					soft_shift = 0;
					}
				}
			else  { // file somewhere below
				int ibottom = -soft_shift;
				int brow = top_row;

				while (ibottom < viewportHeight() && brow < file_row)
					ibottom += midMargin() + fileHeight(brow++);

				if (ibottom >= viewportHeight()) // file not in viewport
					scrollTo(index, QAbstractItemView::PositionAtBottom);
				else { // brow already inside
					if (isAPage)
						ibottom += (1 + index.row() / pages_per_row) * gridSize.height() + headerHeight() + headerMargin(); // bottom of page
					else
						ibottom += fileHeight(file_row); // bottom of file

					if (ibottom >= viewportHeight()) {// file not in viewport
						verticalScrollBar()->setValue(voffset + ibottom - viewportHeight());
						}
					}
				}

			break;
			}
		}

	}

QRect NVBDirView::visualRect(const QModelIndex & index) const {
	if (!index.isValid()) return QRect();

	if (index.parent().isValid()) {
		if (pages_per_row < 1)
			return QRect();

		return QRect(
		         QPoint(
		           leftMargin() + gridSize.width() * (index.row() % pages_per_row),
		           fileDistance(top_row, index.parent().row()) - soft_shift + midMargin() + headerHeight() + headerMargin() + gridSize.height() * (index.row() / pages_per_row)
		         )
		         , gridSize
		       );
		}
	else
		return visualRect(index.row());
	}

QRect NVBDirView::visualRect(int index) const {
	if (oneItemPerFile)
		return QRect(QPoint(
		               leftMargin() + gridSize.width() * (index % pages_per_row),
		               fileDistance(top_row, index) + midMargin() - soft_shift
		             ), gridSize);

	return QRect(leftMargin(), fileDistance(top_row, index) + midMargin() - soft_shift, viewport()->width() - rightMargin() - leftMargin(), headerHeight());
	}

QModelIndex NVBDirView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/) {
	if (!currentIndex().isValid()) {
		if (oneItemPerFile)
			return model()->index(0, 0);

		return model()->index(0, 0, model()->index(0, 0));
		}

	QModelIndex cindex = currentIndex();
	QModelIndex pindex = cindex.parent();

	switch (cursorAction) {
		case QAbstractItemView::MoveUp : {
			if (oneItemPerFile) {
				if (cindex.row() < pages_per_row)
					return cindex;

				return model()->index(cindex.row() - pages_per_row, 0);
				}

			if ( cindex.row() < pages_per_row ) {
				if (pindex.row() == 0)
					return cindex;

				QModelIndex up = model()->index(pindex.row() - 1, 0);

				while (model()->rowCount(up) == 0 && up.row() > 0)
					up = model()->index(up.row() - 1, 0);

				if (model()->rowCount(up) == 0) return QModelIndex();

				if (cindex.row() >= model()->rowCount(up) - pages_per_row * ((model()->rowCount(up) - 1) / pages_per_row))
					return model()->index(model()->rowCount(up) - 1, 0, up);
				else
					return model()->index(pages_per_row * ((model()->rowCount(up) - 1) / pages_per_row) + cindex.row(), 0, up);
				}
			else
				return model()->index(cindex.row() - pages_per_row, 0, pindex);
			}

		case QAbstractItemView::MoveDown : {
			if (oneItemPerFile) {
				if (cindex.row() + pages_per_row >= model()->rowCount())
					return cindex;

				return model()->index(cindex.row() + pages_per_row, 0);
				}

			if ( cindex.row() + pages_per_row >= model()->rowCount(pindex) ) {
				if (pindex.row() == model()->rowCount() - 1)
					return cindex;

				QModelIndex down = model()->index(pindex.row() + 1, 0);

				while (model()->rowCount(down) == 0 && down.row() < model()->rowCount() - 1)
					down = model()->index(down.row() + 1, 0);

				if (model()->rowCount(down) == 0) return QModelIndex();

				if (model()->rowCount(down) <= cindex.row() % pages_per_row)
					return model()->index(model()->rowCount(down) - 1, 0, down);
				else
					return model()->index(cindex.row() % pages_per_row, 0, down);
				}
			else
				return model()->index(cindex.row() + pages_per_row, 0, pindex);
			}

		case QAbstractItemView::MoveLeft : {
			if (cindex.row() == 0)
				return cindex;

			if (oneItemPerFile && (cindex.row() % pages_per_row) == 0)
				return cindex;

			return model()->index(cindex.row() - 1, 0, pindex);
			}

		case QAbstractItemView::MoveRight : {
			if (cindex.row() == model()->rowCount(pindex) - 1)
				return cindex;

			if (oneItemPerFile && ((cindex.row() + 1) % pages_per_row) == 0)
				return cindex;

			return model()->index(cindex.row() + 1, 0, pindex);
			}

		case QAbstractItemView::MoveHome : {
			if (oneItemPerFile) return model()->index(0, 0);

			return model()->index(0, 0, model()->index(0, 0));
			}

		case QAbstractItemView::MoveEnd : {
			if (oneItemPerFile) return model()->index(model()->rowCount() - 1, 0);

			return model()->index(0, 0, model()->index(model()->rowCount() - 1, 0));
			}

		case QAbstractItemView::MovePageUp : {
			if (oneItemPerFile) {
				int pages_per_page = pages_per_row * (viewport()->height() / gridSize.height());

				if (cindex.row() < pages_per_page)
					return model()->index(0, 0);

				return model()->index(cindex.row() - pages_per_page, 0);
				}

			if (pindex.row() == 0)
				return cindex;

			return model()->index(0, 0, model()->index(pindex.row() - 1, 0));
			break;
			}

		case QAbstractItemView::MovePageDown : {
			if (oneItemPerFile) {
				int pages_per_page = pages_per_row * (viewport()->height() / gridSize.height());

				if (cindex.row() + pages_per_page >= model()->rowCount())
					return model()->index(model()->rowCount() - 1, 0);

				return model()->index(cindex.row() + pages_per_page, 0);
				}

			if (pindex.row() == model()->rowCount() - 1)
				return cindex;

			return model()->index(0, 0, model()->index(pindex.row() + 1, 0));
			break;
			}

		case QAbstractItemView::MoveNext : {
			if (oneItemPerFile) {
				if (cindex.row() == model()->rowCount() - 1)
					return cindex;

				return model()->index(cindex.row() + 1, 0, pindex);
				}

			if (cindex.row() == model()->rowCount(pindex) - 1) {
				if (pindex.row() == model()->rowCount() - 1)
					return cindex;

				QModelIndex down = model()->index(pindex.row() + 1, 0);
				return model()->index(0, 0, down);
				}

			return model()->index(cindex.row() + 1, 0, pindex);
			}

		case QAbstractItemView::MovePrevious : {
			if (oneItemPerFile) {
				if (cindex.row() == 0)
					return cindex;

				return model()->index(cindex.row() - 1, 0, pindex);
				}

			if (cindex.row() == 0) {
				if (pindex.row() == 0)
					return cindex;

				QModelIndex up = model()->index(pindex.row() - 1, 0);
				return model()->index(model()->rowCount(up) - 1, 0, up);
				}

			return model()->index(cindex.row() - 1, 0, pindex);
			}
		}

	return QModelIndex();
	}

QRegion NVBDirView::visualRegionForSelection(const QItemSelection & selection) const {
	if (selection.indexes().isEmpty())
		return QRegion();

	return visualRect(selection.indexes().first());
	}

void NVBDirView::setSelection(const QRect & rect, QItemSelectionModel::SelectionFlags /*flags*/) {
	setCurrentIndex(indexAt(rect.topLeft()));
	}

void NVBDirView::paintEvent(QPaintEvent * e) {
	if (!model() || !model()->rowCount())
		return;

	if (oneItemPerFile) {
		gridPaintEvent(e);
		return;
		}

	QPainter painter(viewport());

// Find the first visible row that intersects with e->rect()

	int rendered_row = top_row;
	int row_bottom = fileHeight(rendered_row) - soft_shift + midMargin(); // row_bottom / row_top are in real view coordinates, including top midMargin()

	while (rendered_row < model()->rowCount() && row_bottom <= e->rect().top())
		row_bottom += midMargin() + fileHeight(++rendered_row);

	int row_top = row_bottom - fileHeight(rendered_row);

// Render down to the last visible row that intersects with e->rect()

	while (rendered_row < model()->rowCount() && row_top < e->rect().bottom()) {
		// TODO Draw alternating background
		if (pages_per_row > 0)
			drawItems(rendered_row, row_top + headerHeight() + headerMargin(), &painter);

		drawHeader(rendered_row, row_top, &painter); // To put the floating header on top
		row_top += midMargin() + fileHeight(rendered_row++);
		}


#ifdef NVB_DEBUG
	// DEBUG
	QString debugText = QString("TOP : %1 \nSOFT : %2 \nVOFF : %3/%4 \nTOTAL : %5 \nSTEP : %6 \nMAX : %7")
	                    .arg(top_row, 3)
	                    .arg(soft_shift, 6)
	                    .arg(voffset, 8)
	                    .arg(verticalScrollBar()->value(), 8)
	                    .arg(totalHeight(), 6)
	                    .arg(verticalScrollBar()->pageStep(), 6)
	                    .arg(verticalScrollBar()->maximum(), 6);

	QRectF txtrect = painter.boundingRect(0, viewportHeight(), 1, 1, Qt::AlignLeft, debugText);
	txtrect.moveRight(viewport()->width() - 5);
	painter.fillRect(txtrect.adjusted(-5, -5, 5, 5), Qt::white);

	painter.setBrush(QBrush(Qt::black));
	painter.setPen(Qt::black);
	painter.drawText(txtrect, Qt::AlignLeft, debugText);
	// debug
#endif
	}

void NVBDirView::drawHeader(int index, int y, QPainter * painter) const {
	Q_ASSERT(model());

	if (y < 0) y = 0;

	painter->setBrush(QBrush(Qt::black));
	painter->setPen(QPen(painter->brush(), style()->pixelMetric(QStyle::PM_DefaultFrameWidth), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	QFont f = painter->font();
	f.setWeight(QFont::Bold);
	painter->setFont(f);

	QRect hrect = QRect(leftMargin(), y, viewport()->width() - leftMargin() - rightMargin(), headerHeight());

	QString text = model()->index(index, 0).data(Qt::DisplayRole).toString().split("\n").join("   ");
	painter->fillRect(hrect, palette().base());

	style()->drawItemText(painter, hrect, Qt::AlignCenter, palette(), true, text, QPalette::WindowText);

	QRect trect = style()->itemTextRect(painter->fontMetrics(), hrect, Qt::AlignCenter, true, text);
	QLine hline = QLine(trect.right() + leftMargin(), trect.center().y(), hrect.right(), trect.center().y());
	painter->drawLine(hline);
	hline = QLine(leftMargin(), trect.center().y(), trect.left() - rightMargin(), trect.center().y());
	painter->drawLine(hline);

	}

void NVBDirView::drawItems(int index, int y, QPainter * painter) const {
	Q_ASSERT(model());
	Q_ASSERT(pages_per_row);

	QModelIndex fi = model()->index(index, 0);

	int nitems = model()->rowCount(fi);

// Use maximum QStyleOption we can
	QStyleOptionViewItemV4 option = viewOptions();

	option.features |= QStyleOptionViewItemV2::WrapText; // | QStyleOptionViewItemV2::HasDisplay | QStyleOptionViewItemV2::HasDecoration;

	option.locale = locale();
	option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	option.widget = this;

	for (int i = 0; i < nitems; i++) {
		QModelIndex dindex = model()->index(i, 0, fi);
		option.rect = QRect(QPoint(leftMargin() + (i % pages_per_row) * gridSize.width(), y + (i / pages_per_row) * gridSize.height()), gridSize);

		if (dindex == currentIndex())
			option.state |= QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active;
		else
			option.state &= ~( QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active );

		itemDelegate()->paint(painter, option, dindex);
		}
	}

QStyleOptionViewItem NVBDirView::viewOptions() const {
	QStyleOptionViewItem option = QAbstractItemView::viewOptions();

	option.decorationPosition = QStyleOptionViewItem::Top;
	option.displayAlignment = Qt::AlignCenter;
	option.showDecorationSelected = false;
	return option;
	}


void NVBDirView::gridPaintEvent(QPaintEvent * e) {

	if (!pages_per_row) return;

	QPainter painter(viewport());

	int rendered_row = top_row;
	int row_top = -soft_shift;

// Render down to the last visible row that intersects with e->rect()

	while (rendered_row < model()->rowCount() && row_top < e->rect().bottom()) {
		// TODO Draw alternating background
		drawGridItems(rendered_row, qMin(rendered_row + pages_per_row, model()->rowCount()), row_top + midMargin(), &painter);
		row_top += midMargin() + gridSize.height();
		rendered_row += pages_per_row;
		}


#ifdef NVB_DEBUG
	// DEBUG
	painter.setBrush(QBrush(Qt::black));
	painter.setPen(Qt::black);

	painter.drawText(0, 20, QString("TOP : %1 | SOFT : %2 | VOFF : %3 | TOTAL : %4 | STEP : %5 | MAX : %6") \
	                 .arg(top_row, 3).arg(soft_shift, 4).arg(voffset, 6).arg(totalHeight(), 6).arg(verticalScrollBar()->pageStep(), 6).arg(verticalScrollBar()->maximum(), 6));
	// debug
#endif

	}

void NVBDirView::drawGridItems(int start, int end, int y, QPainter *painter) const {
	Q_ASSERT(model());
	Q_ASSERT(pages_per_row);
	Q_ASSERT(start >= 0);
	Q_ASSERT(end > start);

// Use maximum QStyleOption we can
	QStyleOptionViewItemV4 option = viewOptions();

	option.features |= QStyleOptionViewItemV2::WrapText; // | QStyleOptionViewItemV2::HasDisplay | QStyleOptionViewItemV2::HasDecoration;

	option.locale = locale();
	option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	option.widget = this;

	for (int index = start; index < end; index++) {
		QModelIndex dindex = model()->index(index, 0);
		option.rect = QRect(QPoint(leftMargin() + (index - start) * gridSize.width(), y), gridSize);

		if (dindex == currentIndex())
			option.state |= QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active;
		else
			option.state &= ~( QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active );

		itemDelegate()->paint(painter, option, dindex);
//		itemDelegate()-> TODO paint voltage
		}
	}

void NVBDirView::setModel(QAbstractItemModel * m) {
	scrollToTop();
	QAbstractItemView::setModel(m);

	verticalScrollBar()->setEnabled(m);

	if (m != 0) {
		connect(m, SIGNAL(layoutChanged()), this, SLOT(invalidateCache()));
		connect(m, SIGNAL(modelAboutToBeReset()), this, SLOT(scrollToTop()));
		connect(m, SIGNAL(modelReset()), this, SLOT(invalidateCache()));
		connect(m, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(rowsRemoved(QModelIndex, int, int)));
//		connect(m,SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),this,SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
		}

	invalidateCache();
	}

int NVBDirView::totalHeight() {
	if (!model() || !model()->rowCount()) return 0;

	int tHeight;

	if (oneItemPerFile)
		tHeight = (gridSize.height() + midMargin()) * ((model()->rowCount() + pages_per_row - 1) / pages_per_row) - midMargin();
	else {
		tHeight = (headerHeight() + headerMargin() + midMargin()) * model()->rowCount() - midMargin();

		if (pages_per_row)
			for(int j = 1; j <= (counts_total.size() - 1) / pages_per_row + 1 ; j++) // rows per item
				for(int i = qMin(j * pages_per_row, counts_total.size() - 1); i > (j - 1)*pages_per_row; i--)
					tHeight += gridSize.height() * j * counts_total.at(i);
		}

	return tHeight;
	}


void NVBDirView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
	Q_ASSERT(model());

	if (parent.isValid()) { // Pages within a file about to be removed
		int nrows = model()->rowCount(parent);
		counts_total[nrows] -= 1;

		if (parent.row() < top_row)
			counts_above[nrows] -= 1;

		nrows -= end - start + 1;
		counts_total[nrows] += 1;

		if (parent.row() < top_row)
			counts_above[nrows] += 1;
		}
	else { // Files about to be removed

		// First, update cache
		for(int i = start; i < top_row; i++)
			counts_above[ model()->rowCount(model()->index(i, 0, parent)) ] -= 1;

		for(int i = start; i <= end; i++)
			counts_total[ model()->rowCount(model()->index(i, 0, parent)) ] -= 1;

		// We move depending on where the top was - but later
		}

//	updateScrollBars();
	QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
	}

void NVBDirView::rowsRemoved(const QModelIndex& parent, int start, int end) {
	if (!parent.isValid()) {
		if (top_row <= start) // Everything is below
			setDirtyRegion(rect());
		else if (top_row > end) // Everything is above - we "stay in place"
			top_row -= end - start + 1;
		else { // We intersect
			top_row = qMax(0, start - 1);
			soft_shift = 0;
			}
		}

	// Reevaluating vertical offset should be done even if only pages
	// within a file were changed - maybe it came over pages_per_row

	updateBottom();
	}

// void NVBDirView::rowsAboutToBeInserted(const QModelIndex& /*parent*/, int /*start*/, int /*end*/) {
// }

void NVBDirView::rowsInserted(const QModelIndex& parent, int start, int end) {
	Q_ASSERT(model());

	if (parent.isValid()) { // Inserted pages within a file
		int nrows = model()->rowCount(parent);
		counts_total[nrows] += 1;

		if (parent.row() < top_row)
			counts_above[nrows] += 1;

		nrows += end - start + 1;

		if (counts_total.size() <= nrows) {
			counts_total.resize(nrows + 1);
			counts_above.resize(nrows + 1);
			}

		counts_total[nrows] += 1;

		if (parent.row() < top_row)
			counts_above[nrows] -= 1;
		}
	else { // Inserted files
		// update cache
		for(int i = start; i <= end; i++) {
			int nrows = model()->rowCount(model()->index(i, 0));

			if (counts_total.size() <= nrows) {
				counts_total.resize(nrows + 1);
				counts_above.resize(nrows + 1);
				}

			counts_total[nrows] += 1;

			if (i < top_row)
				counts_above[nrows] += 1;
			}

		if (top_row >= start && (top_row != 0 || selectionModel()->hasSelection()))
			top_row += end - start + 1;
		else
			setDirtyRegion(rect());

		// If we are on top of the view and stuff is inserted on top we stay on top

		}

	QAbstractItemView::rowsInserted(parent, start, end);

	updateBottom();
	}

void NVBDirView::keyPressEvent(QKeyEvent * event) {
	if (event == QKeySequence::Copy && model()) {
		QApplication::clipboard()->setMimeData(model()->mimeData(selectedIndexes()));
		event->accept();
		}
	else
		QAbstractItemView::keyPressEvent(event);
	}

void NVBDirView::updatePPR(int nppr) {
	if (!model()) {
		pages_per_row = nppr;
		return;
		}

	if (pages_per_row != nppr) {
		int	top_pages = model()->rowCount(model()->index(top_row, 0));
		int top_row_old = pages_per_row ? 1 + (top_pages - 1) / pages_per_row : 0;
		int top_row_new = nppr ? 1 + (top_pages - 1) / nppr : 0;

		// If less than half a row is visible, change soft_shift
		if (top_row_new != top_row_old)
			if ((top_row_old && fileHeight(top_row) - soft_shift < gridSize.height() / 2)
			    || (!top_row_old && soft_shift > headerHeight() / 2))
				soft_shift += (top_row_new - top_row_old) * gridSize.height();

		pages_per_row = nppr;
		updateBottom();
		}

	}

void NVBDirView::updateBottom() {
	calculateVOffset();
	int maxoffset = qMax(0, totalHeight() - viewportHeight());
	verticalScrollBar()->setEnabled(maxoffset);

	if (voffset > maxoffset) {
		soft_shift -= voffset - maxoffset;
		voffset = maxoffset;

		while (soft_shift < 0) {
			updateTopRow(top_row, top_row - 1);
			soft_shift += fileHeight(top_row) + midMargin();
			}
		}

	ignoreScroll = true;
	verticalScrollBar()->setRange(0, qMax(-1, maxoffset));
	verticalScrollBar()->setPageStep(viewportHeight());
	verticalScrollBar()->setValue(voffset);
	ignoreScroll = false;
	update();
	}

void NVBDirView::resizeEvent(QResizeEvent * event) {
	int nppr = (viewport()->width() - leftMargin() - rightMargin()) / gridSize.width();
	updatePPR(nppr);
	QAbstractItemView::resizeEvent(event);
	}

void NVBDirView::setGridSize(QSize s) {
	gridSize = s;

	verticalScrollBar()->setSingleStep(gridSize.height() / 10); // TODO user setting
	setMinimumSize(
	  1 + leftMargin() + gridSize.width() + rightMargin() +
	  style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2 * frameWidth(),
	  1 + 2 * midMargin() + headerHeight() + headerMargin() + gridSize.height());

	int nppr = qMax(0, viewport()->width() - leftMargin() - rightMargin()) / gridSize.width();
	updatePPR(nppr);

	scheduleDelayedItemsLayout();
	}

void NVBDirView::scrollContentsBy(int dx, int dy) {
// Remember, dy is opposite to voffset!
	if (ignoreScroll) return;

	dy = voffset - verticalScrollBar()->value();

	if (model()) {
		soft_shift -= dy;
		voffset -= dy;

		while (soft_shift < 0) {
			updateTopRow(top_row, top_row - (oneItemPerFile ? pages_per_row : 1));
			soft_shift += fileHeight(top_row) + midMargin();
			}

		while (soft_shift > fileHeight(top_row) + midMargin()) {
			soft_shift -= fileHeight(top_row) + midMargin();
			updateTopRow(top_row, top_row + (oneItemPerFile ? pages_per_row : 1));
			}

		setDirtyRegion(viewport()->rect());
		}

	QAbstractItemView::scrollContentsBy(dx, dy);
	}

int NVBDirView::fileHeight(int index) const {
	if (oneItemPerFile) return gridSize.height();

	Q_ASSERT(model());
	int rc = model()->rowCount(model()->index(index, 0));
	return headerHeight() + headerMargin() + (pages_per_row ? gridSize.height() * (1 + (rc - 1) / pages_per_row) : 0);
	}

void NVBDirView::calculateVOffset() {
	voffset = 0;

	if (!model()) return;

	if (top_row) {

		if (oneItemPerFile)
			voffset = (gridSize.height() + midMargin()) * (top_row / pages_per_row);
		else {
			voffset = (headerHeight() + headerMargin() + midMargin()) * top_row;

			if (pages_per_row)
				for(int j = 1; j <= (counts_above.size() - 1) / pages_per_row + 1 ; j++) // rows per item
					for(int i = qMin(j * pages_per_row, counts_above.size() - 1); i > (j - 1)*pages_per_row; i--)
						voffset += gridSize.height() * j * counts_above.at(i);
			}
		}

	voffset += soft_shift;
	}

void NVBDirView::updateTopRow(int rold, int rnew) {
	if (rnew < 0) rnew = 0;
	else if (rnew >= model()->rowCount()) rnew = model()->rowCount() - 1;

	if (rold == rnew)	return;
	else if (rold < rnew && model()) {
		for(int i = rold; i < rnew; i++)
			counts_above[model()->rowCount(model()->index(i, 0))] += 1;
		}
	else if (rold > rnew && model()) {
		for(int i = rnew; i < rold; i++)
			counts_above[model()->rowCount(model()->index(i, 0))] -= 1;
		}

	top_row = rnew;
	}

void NVBDirView::invalidateCache() {
	top_row = 0;
	soft_shift = 0;
	voffset = 0;

	counts_above.clear();
	counts_total.clear();

	if (model()) {
		int mrc = counts_total.size();

		for( int i = model()->rowCount() - 1; i >= 0; i--) {
			int rc = model()->rowCount(model()->index(i, 0));

			if (mrc <= rc) {
				mrc = rc + 1;
				counts_total.resize(mrc);
				counts_above.resize(mrc);
				}

			counts_total[rc] += 1;
			}
		}

	int maxoffset = qMax(0, totalHeight() - viewportHeight());
	verticalScrollBar()->setEnabled(maxoffset);
	verticalScrollBar()->setRange(0, maxoffset);
	verticalScrollBar()->setPageStep(viewportHeight());
	update();

	}

int NVBDirView::fileDistance( int index1, int index2 ) const {
	if (oneItemPerFile)
		return (index2 / pages_per_row - index1 / pages_per_row) * (midMargin() + gridSize.height());

	int i0 = qMin(index1, index2);
	int in = qMax(index1, index2);
	int dst = 0;

	while (in > i0)
		dst += midMargin() + fileHeight(i0++);

	return dst * (index1 > index2 ? -1 : 1);
	}

void NVBDirView::switchToNormalMode() {
	oneItemPerFile = false;
	int ctr = top_row;
	invalidateCache();
	setDirtyRegion(viewport()->rect());
	scrollTo(model()->index(ctr, 0));
	}

void NVBDirView::switchToGridMode() {
	oneItemPerFile = true;
	// TOP means the index of top-left viewport corner
	top_row = pages_per_row * (top_row / pages_per_row);
	updateBottom();
	setDirtyRegion(viewport()->rect());
	}
