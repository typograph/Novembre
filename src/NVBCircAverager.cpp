//
// C++ Implementation: NVBCircAverager
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBCircAverager.h"
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QAction>
// #include <QLinkedList>
#include <QHash>
#include "NVBViewController.h"
#include <QApplication>

#include "../icons/radial.xpm"

NVBBullsEyeViz::NVBBullsEyeViz(NVB3DDataSource * data):QObject(),NVBFilteringGraphicsItem(),provider(data),radius(provider->resolution().width()/5)
{
//   connect(this,SIGNAL(destroyed(QObject*)),SIGNAL(selectionBroken()));

  center = QPointF(provider->resolution().width()/2,provider->resolution().height()/2);

  setPos(provider->position().topLeft());

#if QT_VERSION >= 0x040300
  setTransform(
    QTransform().scale(
      provider->position().width()/provider->resolution().width(),
      provider->position().height()/provider->resolution().height()
      )
    );
#else
  setMatrix(
    QMatrix().scale(
      provider->position().width()/provider->resolution().width(),
      provider->position().height()/provider->resolution().height()
      )
    );
#endif

  setVisible(true);
#if QT_VERSION >= 0x040400
  setAcceptHoverEvents(true);
#endif
  setFlag(QGraphicsItem::ItemIsFocusable);
  setCursor(Qt::BlankCursor);

}

void NVBBullsEyeViz::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
  painter->save();

  painter->setPen(QPen(Qt::black));
  painter->setBrush(QBrush());

  for (float cradius = 1; cradius <= 1.5 * provider->resolution().width(); cradius += radius)
    painter->drawEllipse(QRectF(center,QSizeF(2*cradius,2*cradius)).translated(-cradius,-cradius));

  painter->restore();
}

void NVBBullsEyeViz::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  radius += event->delta()/60.0;
  if (radius < 1) radius = 1;
  update();
}

void NVBBullsEyeViz::keyReleaseEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_Escape) {
    emit selectionBroken();
    }
}

void NVBBullsEyeViz::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  center = event->pos();
  update();
}

void NVBBullsEyeViz::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  event->accept();
}

void NVBBullsEyeViz::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  event->accept();
  emit pointSelected(center.toPoint());
//   deactivatePointSelection();
}

// CircAverager

NVBCircAverager::NVBCircAverager(NVB3DDataSource * topo, NVBViewController * wnd):NVBSpecDataSource(),provider(topo),av_data(0),pr_window(wnd),my_bullseye(0),colors(0)
{

  // Set a parent. Thus, if user interrupts the selection process, this delegate can be deleted as a child
  setParent(qApp);
	owner = topo->owner;

  // As of that moment bullseye is not yet created, so we can't do much

  //TODO Here one should be sure that xd() == yd(). At the moment (v0.0.4) they always are.
  ld = provider->xDim();
  hd = provider->zDim();

  colors = new NVBConstDiscrColorModel(Qt::black);
}

NVBCircAverager::~ NVBCircAverager()
{
  if (av_data) delete av_data;
  if (my_bullseye) delete my_bullseye;
  if (colors) delete colors;
}

NVBVizUnion NVBCircAverager::bullseye()
{
  if (!my_bullseye) {
    my_bullseye = new NVBBullsEyeViz(provider);
    connect(my_bullseye, SIGNAL(pointSelected(QPoint)), this, SLOT(averageAround(QPoint)));
    connect(my_bullseye, SIGNAL(selectionBroken()), this, SLOT(vizDeactivationRequest()));
    NVBVizUnion u(NVB::TopoPage,my_bullseye);
    u.filter = this;
    return u;
    }
  else
    return NVBVizUnion();
}

QAction * NVBCircAverager::action()
{
  return new QAction(QIcon(_rad_av),QString("Radial averaging"),0);
}

namespace {

  // Stands for RAdial AVerage EntrY

  struct ravey{
    double value;
    unsigned char counts;
    ravey():value(0),counts(0) {;}
    ravey & operator+=(double v) { value += v; counts += 1; return *this; }
    inline double average() { return value/counts; }
    };

};

void NVBCircAverager::averageAround(QPoint p)
{

  vizDeactivationRequest();


  // Now, how do we average all the points?
  // Let's go easy -- use QHash

  QHash<uint,ravey> vs;
  for (int i = provider->resolution().width()-1; i >= 0;i--)
    for (int j = provider->resolution().height()-1; j >= 0;j--)
      vs[ (int)(pow(i-p.x(),2) + pow(j-p.y(),2)) ] += provider->getData(i,j);


  QwtArray<double> xs,ys;
  xs.reserve(vs.count());
  ys.reserve(vs.count());

  // FIXME What if we have rectangular pixels?
  double factor = provider->position().width()/provider->resolution().width();

  QList<uint> rs = vs.keys();
  qSort(rs);

  foreach(uint r, rs) {
    xs << sqrt(r) * factor;
    ys << vs[r].average();
    }

  pt_at.setX(p.x()*factor);
  pt_at.setY(p.y()*factor);

  av_data = new QwtArrayData(xs,ys);

#ifdef WITH_GRAPHVIEW
  pr_window->openInNewWindow(this,NVBVizUnion(),NVB::GraphView);

  setParent(0);
#endif
//   useDataSource(this);
}

void NVBCircAverager::vizDeactivationRequest()
{
  emit detach2DViz();
  if (my_bullseye) {
    delete my_bullseye;
    my_bullseye = 0;
    }
}
