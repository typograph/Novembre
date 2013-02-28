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
#include "NVBListItemDelegate.h"

void NVBListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
	QItemDelegate::paint(painter, option, index);

//  painter->fillRect(option.rect, option.palette.light());
	if (option.state & QStyle::State_MouseOver)
		painter->fillRect(option.rect, option.palette.light());
	}

QSize NVBListItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
	return QItemDelegate::sizeHint(option, index);
	}

bool NVBListItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index) {
	switch (event->type()) {
		case QEvent::HoverEnter : {
			return true;
			}

		case QEvent::HoverLeave : {
			;
			return true;
			}

		default :
			return QItemDelegate::editorEvent(event, model, option, index);
		}
	}

#include "NVBListItemDelegate.h"

