//
// C++ Interface: NVB2DVizDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVB2DVIZ_H
#define NVB2DVIZ_H

#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"
#include <QtGui/QGraphicsItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneWheelEvent>

#include "NVBGraphicsItems.h"

/**
 * Visualisation as a QPixmapItem for a topopage.
 * Been there, done that.
 */

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
class NVB2DMapVizDelegate : public QObject, public QGraphicsItem {
Q_OBJECT
private:
  QImage * p_image;
public:
  virtual QRectF boundingRect () const;
  virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
//   int type () const {return 7;}

#else

#include <QtGui/QGraphicsPixmapItem>

class NVB2DMapVizDelegate : public QObject, public QGraphicsPixmapItem {
Q_OBJECT
#endif
protected:
  NVB3DDataSource* page;
public :
  NVB2DMapVizDelegate(NVBDataSource* source);
  virtual ~NVB2DMapVizDelegate();

  virtual NVBVizUnion getVizItem();

public slots:
  void redraw();
  virtual void setSource( NVBDataSource * );
};

/**
 * Visualisation as a QGraphicsItem for a specpage.
 * Been there, done that.
 */

class NVB2DPtsVizDelegate : public QObject, public NVBFilteringGraphicsItem {
Q_OBJECT
private:
  NVBSpecDataSource* page;
  qreal radius;

  QList<QPointF> positions;
  QList<QColor> colors;

private slots:
  void initEllipses();

public :
  NVB2DPtsVizDelegate(NVBDataSource* source);
  virtual ~NVB2DPtsVizDelegate();

  virtual NVBVizUnion getVizItem();

  virtual QRectF boundingRect () const;
  virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

protected:
  virtual void wheelEvent ( QGraphicsSceneWheelEvent * event );

public slots:
  virtual void setSource( NVBDataSource * );
//  void redraw();
};

#endif
