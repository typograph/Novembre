//
// C++ Interface: NVBGraphicsItems
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBGRAPHICSITEM_H
#define NVBGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
QTransform invertQTransform(const QTransform & t);
#endif

class NVBFullGraphicsItem : public QGraphicsItem {
private :
//  QGraphicsItem ** subcontrol;
public :
  NVBFullGraphicsItem();
  virtual ~NVBFullGraphicsItem() {;}
	virtual inline QRectF boundingRect () const {
		static bool protect = false;
		if (  protect ) return QRectF();
		if ( !scene() ) return QRectF();
		protect = true;
		QRectF rect = scene()->sceneRect();
		protect = false;
		return rect;
		}
  virtual void paint ( QPainter * , const QStyleOptionGraphicsItem * , QWidget *  = 0 );
protected :
  virtual bool sceneEvent ( QEvent * event );
/*
  virtual bool sceneEvent ( QEvent * event ) {
    if (!*subcontrol) return false;
    return (*subcontrol)->sceneEvent(event);
    }
*/
};


class NVBFilteringGraphicsItem : public QGraphicsItem {
protected :
  bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event );
public :
  NVBFilteringGraphicsItem();
  virtual ~NVBFilteringGraphicsItem() {;}
};

#endif
