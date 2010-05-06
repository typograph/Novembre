//
// C++ Interface: NVBCircAverager
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBCIRCAVERAGER_H
#define NVBCIRCAVERAGER_H

#include "NVBDataSource.h"
#include "NVBDiscrColoring.h"
#include "NVBFilterDelegate.h"
#include "NVBGraphicsItems.h"

#include <QRect>
#include <QPoint>
#include <QRectF>
#include <QPointF>

class NVBViewController;
class QAction;

class NVBBullsEyeViz : public QObject, public NVBFilteringGraphicsItem {
Q_OBJECT
private:
  NVB3DDataSource * provider;
  double radius;
  QPointF center;
public:
  NVBBullsEyeViz(NVB3DDataSource * leveler);
  virtual ~NVBBullsEyeViz() {;}

  virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
  virtual QRectF boundingRect () const { return QRectF(QPointF(),provider->resolution()); }

protected:
  virtual void wheelEvent ( QGraphicsSceneWheelEvent * event );
  virtual void keyReleaseEvent ( QKeyEvent * event );
  virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

signals:
  void pointSelected(QPoint);
  void selectionBroken();

};

class NVBCircAverager : public NVBSpecDataSource {
Q_OBJECT
private:
  NVB3DDataSource * provider;
  NVBDimension ld,hd;
  QwtArrayData * av_data;
  QPointF pt_at;
  QString filename;

  NVBViewController * pr_window;

  NVBBullsEyeViz * my_bullseye;
  NVBConstDiscrColorModel * colors;

public:
  NVBCircAverager(NVB3DDataSource * topo, NVBViewController * wnd);
  ~NVBCircAverager();

  NVBVizUnion bullseye();

  static QAction * action();

  virtual QString name() const { return QString("Circular average"); }
  virtual QString fileName() const { return filename;}
  virtual void setFileName(QString) {;}

  virtual NVBDimension xDim() const { return ld; }
  virtual NVBDimension yDim() const { return ld; }
  virtual NVBDimension zDim() const { return hd; }
  virtual NVBDimension tDim() const { return ld; }

  virtual const NVBDiscrColorModel * getColorModel()  const { return colors; }

  virtual QList<QPointF> positions() const { return av_data ? (QList<QPointF>() << pt_at) : QList<QPointF>();}
  virtual QList<QwtData*> getData() const { return av_data ? (QList<QwtData*>() << av_data) : QList<QwtData*>();}

  virtual QSize datasize() const { return QSize(av_data->size(),av_data ? 1 : 0);}

private slots:
  void averageAround(QPoint);

public slots:
  void vizDeactivationRequest();

signals:
  void detach2DViz();
};
#endif
