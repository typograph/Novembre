//
// C++ Interface: NVBDiscrColorPainter
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBDISCRCOLORPAINTER_H
#define NVBDISCRCOLORPAINTER_H

#include "NVBFilterDelegate.h"
#include "NVBDiscrColoring.h"
#include "NVBViewController.h"
#include "NVBColorButton.h"
#include "NVBQuadTree.h"
#include <QList>
#include <QAction>
#include <QWidget>
#include <QRectF>
#include <QRubberBand>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QPainter>
#include <QPointer>
#include <QVector>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include "NVBGraphicsItems.h"

class NVBSubstDiscrColorModel : public NVBDiscrColorModel
{
Q_OBJECT
private:
  const NVBDiscrColorModel * provider;
  QList<QColor> colors;
public:
  NVBSubstDiscrColorModel(NVBDiscrColorModel * source = 0);
  ~NVBSubstDiscrColorModel() {;}
  virtual QColor colorize(int) const;

  void setColor(int index, QColor color, bool = true);
  void setColor(QList< int > indexes, QColor color);
public slots:
  void setModel(const NVBDiscrColorModel * model);
};

class NVBDiscrPainterViz : public QObject, public NVBFilteringGraphicsItem {
Q_OBJECT
protected:

	NVBQuadTree points;
	QVector< bool > touched;
  QBrush brush;
  NVBSpecDataSource * sprovider;

public:
  NVBDiscrPainterViz(NVBSpecDataSource * ):QObject(),NVBFilteringGraphicsItem(),sprovider(0) {;} // Do everything in subclasses
  virtual ~NVBDiscrPainterViz() {;}

protected slots:
  virtual void setSource(NVBDataSource * source);
  virtual void refresh();

public slots:
  virtual void setBrush(QBrush newbrush) {
    brush = newbrush;
    touched.fill(false);
    }
signals:
  void pointsTouched(QList<int>);
};

class NVBDiscrBrushPainterViz : public NVBDiscrPainterViz {
Q_OBJECT
private:
	bool active;
  QRectF rect;
  QRectF mouserect;
  bool showmouse;
public:
  NVBDiscrBrushPainterViz(NVBSpecDataSource * source);
  virtual ~NVBDiscrBrushPainterViz();

  virtual inline QRectF boundingRect () const {
    if (scene() && !scene()->views().isEmpty()) {
      const QGraphicsView * v = scene()->views().at(0);
      return v->mapToScene(v->rect()).boundingRect();
      }
    return rect.adjusted(-mouserect.width()/2,-mouserect.height()/2,mouserect.width()/2,mouserect.height()/2);
    }
  virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

protected slots:
  void refresh();

protected:
  virtual void wheelEvent ( QGraphicsSceneWheelEvent * event );
//  virtual void keyReleaseEvent ( QKeyEvent * event );
  virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
public slots:
  void setBrush(QBrush newbrush) {
    NVBDiscrPainterViz::setBrush(newbrush);
    update(mouserect);
    }
};

class NVBDiscrRectPainterViz : public NVBDiscrPainterViz {
Q_OBJECT
private:
  QRectF brect;
  QPoint rborigin;
  QPointF scorigin;
  QRubberBand * rubberBand;

public:
  NVBDiscrRectPainterViz(NVBSpecDataSource * source);
  virtual ~NVBDiscrRectPainterViz();

  virtual inline QRectF boundingRect () const {
    if (scene() && !scene()->views().isEmpty()) {
      const QGraphicsView * v = scene()->views().at(0);
      return v->mapToScene(v->rect()).boundingRect();
      }
    return brect;
    }
  virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

protected slots:
  void refresh();

protected:
//  virtual void keyReleaseEvent ( QKeyEvent * event );
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
};

class NVBDiscrColorPainterDelegate : public NVBSpecFilterDelegate {
Q_OBJECT
private:
  NVBSpecDataSource * sprovider;
  NVBSubstDiscrColorModel * colors;
public:
  NVBDiscrColorPainterDelegate(NVBSpecDataSource * source, NVBSubstDiscrColorModel * model);
  virtual ~NVBDiscrColorPainterDelegate() { if (colors) delete colors; }

  NVB_FORWARD_SPECDATA(sprovider);

  virtual inline const NVBDiscrColorModel * getColorModel()  const  { return colors; }

protected slots:
  void parentColorsAboutToBeChanged();
  void parentColorsChanged();
  virtual void setSource(NVBDataSource * source);

private :
  void connectSignals();

};

class NVBDiscrColorPainter : public QWidget {
Q_OBJECT
private:
  NVBSubstDiscrColorModel * colors;
  NVBSpecDataSource * provider;
  NVBViewController * wparent;
  QPointer<NVBDiscrColorPainterDelegate> page;
  NVBDiscrPainterViz * painter;
  QColor ccolor;
  QActionGroup * tools;
  NVBColorButton * sliderColor;
  NVBColorButton * minSliderColor;
  NVBColorButton * maxSliderColor;
  void activatePainter(NVBDiscrPainterViz * viz);
public:
  NVBDiscrColorPainter(NVBSpecDataSource * source, NVBViewController * wnd);
  virtual ~NVBDiscrColorPainter();

  NVBDataSource * filter() { return page; }
  static QAction * action();
public slots:
  void getColor();
  void setColor(QColor color);
  void colorizePoints(QList<int> points);
  void activateBrushPainter();
  void activateRectPainter();
  void deactivatePainting();
  void vizDeactivationRequest();
  void setSource(NVBDataSource* source);
  void setSlidingColor(int);
signals:
  void detach2DViz();
};

#endif
