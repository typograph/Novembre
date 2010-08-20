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

#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
QTransform invertQTransform(const QTransform & t);
#endif

/**
 * \class NVBFullGraphicsItem
 *
 * A special QGraphicsItem that covers all the other items in QGraphicsScene.
 * This item is always at the background (zIndex = 100000000)
 * It also accepts all events.
 */

class NVBFullGraphicsItem : public QGraphicsItem {
public :
  NVBFullGraphicsItem();
  virtual ~NVBFullGraphicsItem() {;}
	virtual QRectF boundingRect () ;
	virtual void paint ( QPainter * , const QStyleOptionGraphicsItem * , QWidget *  = 0 );
protected :
  virtual bool sceneEvent ( QEvent * event );
};

/**
 * \class NVBFilteringGraphicsItem
 *
 * This QGraphicsItem intercepts events sent to all items in scene
 * (which means any events in the sceneRect()), and forwards them to itself.
 * To be subclassed by classes that need user input in NVB2DView
 */

class NVBFilteringGraphicsItem : public QGraphicsItem {
protected :
  bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event );
public :
  NVBFilteringGraphicsItem();
  virtual ~NVBFilteringGraphicsItem() {;}
};

#endif
