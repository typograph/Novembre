//
// C++ Implementation: NVBListItemDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBListItemDelegate.h"

void NVBListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QItemDelegate::paint(painter,option,index);
//  painter->fillRect(option.rect, option.palette.light());
  if (option.state & QStyle::State_MouseOver)
    painter->fillRect(option.rect, option.palette.light());
}

QSize NVBListItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  return QItemDelegate::sizeHint(option,index);
}

bool NVBListItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
  switch (event->type()) {
    case QEvent::HoverEnter : {
      return true;
      }
    case QEvent::HoverLeave : {;
      return true;
      }
    default : return QItemDelegate::editorEvent(event,model,option,index);
    }
}

#include "NVBListItemDelegate.h"

