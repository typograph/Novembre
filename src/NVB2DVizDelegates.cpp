//
// C++ Implementation: NVB2DVizDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVB2DVizDelegates.h"

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400

NVB2DMapVizDelegate::NVB2DMapVizDelegate(NVBDataSource * source):QObject(),QGraphicsItem(),page(0)
{
  p_image = 0;
  setSource(source);
}

NVB2DMapVizDelegate::~ NVB2DMapVizDelegate()
{
  if (p_image) delete p_image;
}

QRectF NVB2DMapVizDelegate::boundingRect () const
{
 float halfPw = 0.5;
//  return QRectF(0,0,10,10);
  return (!p_image || p_image->isNull()) ? QRectF() : QRectF(pos(), p_image->size()).adjusted(-halfPw, -halfPw, halfPw, halfPw);
}

void NVB2DMapVizDelegate::paint ( QPainter * painter, const QStyleOptionGraphicsItem * /* option */, QWidget * )
{
  if (p_image) {
/*    QRectF exposed = option->exposedRect.adjusted(-1, -1, 1, 1);
    exposed &= QRectF(x(), y(), p_image->width(), p_image->height());
    exposed.translate(pos());*/
    painter->drawImage(pos()-QPointF(-0.5,0.5), *p_image);
    }
}

void NVB2DMapVizDelegate::redraw()
{
  if (p_image) delete p_image;
  p_image = page->getColorModel()->colorize(page->getData(),page->resolution());
  update();
}

#else

NVB2DMapVizDelegate::NVB2DMapVizDelegate(NVBDataSource * source):QObject(),QGraphicsPixmapItem(),page(0)
{
  setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  setSource(source);
}

NVB2DMapVizDelegate::~ NVB2DMapVizDelegate()
{
}

void NVB2DMapVizDelegate::redraw()
{
  QImage* tmpImage = page->getColorModel()->colorize(page->getData(),page->resolution());
  setPixmap(QPixmap::fromImage(*tmpImage));
  update();
  delete tmpImage;
}

#endif

NVBVizUnion NVB2DMapVizDelegate::getVizItem()
{
  NVBVizUnion u;
  u.TwoDViz = this;
  return u;
}

void NVB2DMapVizDelegate::setSource(NVBDataSource * source)
{
  if (page) {
    page->disconnect(this);
    disconnect(page->getColorModel(),0,this,0);
    }

  if (source->type() != NVB::TopoPage) {
    page = 0;
#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
    if (p_image) delete p_image;
    p_image = 0;
#else
    setPixmap(QPixmap());
#endif
    }
  else {
    page = (NVB3DDataSource*)source;

    setPos(page->position().topLeft());

#if QT_VERSION >= 0x040300
    setTransform(
      QTransform().scale(
        page->position().width()/page->resolution().width(),
        page->position().height()/page->resolution().height()
        )
      );
#else
    setMatrix(
      QMatrix().scale(
        page->position().width()/page->resolution().width(),
        page->position().height()/page->resolution().height()
        )
      );
#endif
/* This method requires reseting the scale, not provided by the API
    scale(
        page->position().width()/page->resolution().width(),
        page->position().height()/page->resolution().height()
        );
*/
    connect(page,SIGNAL(dataChanged()),SLOT(redraw()));
    connect(page,SIGNAL(dataAdjusted()),SLOT(redraw()));
    connect(page,SIGNAL(colorsChanged()),SLOT(redraw()));
    connect(page,SIGNAL(colorsAdjusted()),SLOT(redraw()));
    connect(page,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)));
    connect(page,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)),Qt::QueuedConnection);
  
    redraw();
    }
}
// ===============

NVB2DPtsVizDelegate::NVB2DPtsVizDelegate(NVBDataSource * source):QObject(),NVBFilteringGraphicsItem(),page(0),radius(0)
{
  if (source->type() != NVB::SpecPage) throw;

  setSource(source);
}

NVB2DPtsVizDelegate::~ NVB2DPtsVizDelegate()
{

}

NVBVizUnion NVB2DPtsVizDelegate::getVizItem()
{
  return NVBVizUnion();
}

QRectF NVB2DPtsVizDelegate::boundingRect() const
{
  return page->occupiedArea().adjusted(-radius,-radius,radius,radius);
}

void NVB2DPtsVizDelegate::initEllipses()
{
  prepareGeometryChange();

  QList<uint> r,g,b,cnt;
  positions.clear();
  colors.clear();

//  int j;
  QColor c = page->colors().at(0);
  positions.append(page->positions().at(0));
  r.append(c.red());
  g.append(c.green());
  b.append(c.blue());
  cnt.append(1);


  for(int i = 1; i < page->positions().size(); i++) {
    c = page->colors().at(i);
    QPointF spos = page->positions().at(i);
//    j = positions.indexOf();
//    if (j >= 0) {
    if (positions.last() == spos) {
      r.last() += c.red();
      g.last() += c.green();
      b.last() += c.blue();
      cnt.last() += 1;
      }
    else {
      positions.append(spos);
      r.append(c.red());
      g.append(c.green());
      b.append(c.blue());
      cnt.append(1);
      }
    }

  for(int i = 0; i < positions.size(); i++) {
    colors.append(QColor(r.at(i)/cnt.at(i),g.at(i)/cnt.at(i),b.at(i)/cnt.at(i)));
    }

  update();

}

void NVB2DPtsVizDelegate::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget * )
{
  painter->save();
  painter->setPen(QPen(Qt::NoPen));
  for(int i = positions.size()-1; i>=0; i--) {
    painter->setBrush(QBrush(colors.at(i)));
#if QT_VERSION >= 0x040400
    painter->drawEllipse(positions.at(i),radius,radius);
#else
    painter->drawEllipse(QRectF(positions.at(i)-QPointF(radius,radius),QSizeF(2*radius,2*radius)));
#endif
    }
  painter->restore();
}

void NVB2DPtsVizDelegate::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  prepareGeometryChange();
  radius += radius*event->delta()/400.0;
  update();
}

void NVB2DPtsVizDelegate::setSource(NVBDataSource * source)
{
  if (page) page->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    page = 0;
    positions.clear();
    colors.clear();
    }
  else {
    page = (NVBSpecDataSource*)source;

    //connect(page,SIGNAL(dataAdjusted()),SLOT(initEllipses())); // Changes in the curves do not matter
    connect(page,SIGNAL(dataChanged()),SLOT(initEllipses()));
    connect(page,SIGNAL(colorsAdjusted()),SLOT(initEllipses())); // TODO Think of just recoloring w/o repositioning
    connect(page,SIGNAL(colorsChanged()),SLOT(initEllipses()));
    connect(page,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)));
    connect(page,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)),Qt::QueuedConnection);

    initEllipses();

    if (radius == 0) {
/*      qreal pw = page->occupiedArea().width()/sqrt(positions.size())/10;
      qreal ph = page->occupiedArea().height()/sqrt(positions.size())/10;
      radius = qMin(pw,ph);*/
      radius = (page->occupiedArea().width() + page->occupiedArea().height())/sqrt(positions.size())/3;
//       if (radius == 0) radius = (pw == 0) ? ph : pw;
      if (radius == 0) radius = 1;
//       NVBOutputError("radius","w: %f, h: %f, r: %f",pw,ph,radius);
      }
    }
}


