//
// C++ Interface: NVBListItemDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBLISTITEMDELEGATE_H
#define NVBLISTITEMDELEGATE_H

#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QEvent>

class NVBListItemDelegate : public QItemDelegate {
Q_OBJECT
private:
public:
     NVBListItemDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}

     void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
  virtual bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );
};

#endif
