//
// C++ Implementation: NVBTopoLeveler
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBTopoLeveler.h"
#include <float.h>
#include <QIcon>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPainter>
#include <QGraphicsScene>

#include "../icons/leveler.xpm"

NVBTopoLevelerWidget::NVBTopoLevelerWidget(NVBTopoLeveler::Mode mode, NVB::ViewType vtype, QWidget * parent):QWidget(parent)
{
  setWindowTitle("Leveling");

  QHBoxLayout * l = new QHBoxLayout(this);
#if QT_VERSION >= 0x040300
  l->setContentsMargins(0,0,0,0);
#else
  l->setMargin(0);
#endif
  actionCnt = new QActionGroup(this);

  l->addStretch(1);

  // No leveling
{
  QAction * action = actionCnt->addAction(QIcon(_lv_nolv),"No leveling");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(noLevelingModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::NoLeveling)
    action->setChecked(true);
}
  // Line leveling
{
  QAction * action = actionCnt->addAction(QIcon(_lv_linelv),"Level substraction");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(lineLevelingModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::LineLeveling)
    action->setChecked(true);
}
  // Offset leveling
{
  QAction * action = actionCnt->addAction(QIcon(_lv_offslv),"Offset substraction");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(offsetLevelingModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::OffsetLeveling)
    action->setChecked(true);
}
  // Slope leveling
{
  QAction * action = actionCnt->addAction(QIcon(_lv_slopelv),"Slope substraction");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(lineSlopeLevelingModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::LineSlopeLeveling)
    action->setChecked(true);
}

#ifdef WITH_2DVIEW
if (vtype == NVB::TwoDView) {

  // 3Pts leveling
{
  QAction * action = actionCnt->addAction(QIcon(_lv_3ptslv),"3-points plane substraction");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(threePointsLevelingModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::ThreePointsLeveling)
    action->setChecked(true);
}

  // Parabola leveling
{
/* // TODO find the good algorithm for this
  action = actionCnt->addAction(QIcon(_lv_prbl),"Parabola substraction");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(parabolaModeActivated()));
  tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(tb->iconSize());
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBTopoLeveler::Parabola)
    action->setChecked(true);
*/
}

}
#endif

//  l->setSizeConstraint(QLayout::SetFixedSize);
  l->addStretch(1);

  setLayout(l);
//  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

}

NVBTopoLevelerWidget::~ NVBTopoLevelerWidget()
{
}

// -------------------------

NVBTopoLeveler::NVBTopoLeveler(NVB3DDataSource * source,  NVBViewController * wnd )
  : NVB3DFilterDelegate(source)
  , tprovider(source)
#ifdef WITH_2DVIEW
  , vizcontroller(0)
#endif
  , fdata(0)
  , mode(NoLeveling)
  , isLeveling(false)
  , view(wnd)
{
  colors = new NVBTrackingRescaleColorModel(source->getZMin(), source->getZMax(), source->getColorModel()->zMin(), source->getColorModel()->zMax());
/*
  vizcontroller = new NVBTopoLevelerViz(this);

  connect(vizcontroller,SIGNAL(pointsSelected(QRectF, QRectF, QRectF)),SLOT(levelByThreePoints(QRectF,QRectF,QRectF)));
*/

  connectSignals();

  followSource();

}

void NVBTopoLeveler::reset(bool resetmode)
{
  if (resetmode) {
    isLeveling = false;
    mode = NoLeveling;
    }
  if (fdata) {
    if (tprovider)
      memcpy(fdata,tprovider->getData(),sizeof(double)*tprovider->resolution().width()*tprovider->resolution().height());
    else
      memset(fdata,0,sizeof(double)*tprovider->resolution().width()*tprovider->resolution().height());
    }
  else if (tprovider) {
    fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());
    }
  if (provider) {
    zMin = tprovider->getZMin();
    zMax = tprovider->getZMax();
    }
  else {
    zMin = DBL_MAX;
    zMax = DBL_MIN;
    }
}

void NVBTopoLeveler::levelByLine()
{
  reset();

//   fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());

  // TODO think about using getData(x,y)
  // FIXME This algorithm will work poorly with scans done in y direction

  const double * const tdata = tprovider->getData();
  QSize s = tprovider->resolution();
//  memcpy(fdata, tdata, s.width());

  for ( int i = s.height()-1; i>=0; i--) {
    double offset = 0;
    int aoff = i*s.width();
    for ( int j = s.width()-1; j>=0; j--) {
      offset += tdata[j+aoff];
      }
    offset /= s.width();
    for ( int j = s.width()-1; j>=0; j--) {
      fdata[j+aoff] = tdata[j+aoff] - offset;
      }
    }

  mode = LineLeveling;
  getMinMax();
  autoScaleColors();

}

void NVBTopoLeveler::levelByOffset()
{
  reset();

//   fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());

  // TODO think about using getData(x,y)
  // FIXME This algorithm will work poorly with scans done in y direction

  const double * const tdata = tprovider->getData();
  QSize s = tprovider->resolution();
//  memcpy(fdata, tdata, s.width());

  for ( int i = s.height()-1; i>0; i--) {
    double offset = 0;
    int aoff = i*s.width();
    int boff = (i-1)*s.width();
    for ( int j = s.width()-1; j>=0; j--) {
      offset += tdata[j+aoff] - tdata[j+boff];
      }
    offset /= s.width();
    for ( int j = s.width()-1; j>=0; j--) {
      fdata[j+boff] = tdata[j+boff] + offset;
      }
    }

  mode = OffsetLeveling;
  getMinMax();
  autoScaleColors();

}

void NVBTopoLeveler::levelByLineSlope()
{
  reset();

//   fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());

  // TODO think about using getData(x,y)
  // FIXME This algorithm will work poorly with scans done in y direction

  const double * const tdata = tprovider->getData();
  QSize s = tprovider->resolution();
//  memcpy(fdata, tdata, s.width());

  int aoff,i;
  for ( i = s.height(), aoff = --i*s.width() ;
                       i>=0; aoff = --i*s.width()) {

    double slope = (tdata[aoff] - tdata[aoff + s.width()-1]) / (s.width()-1);

    double offset = 0;
    for ( int j = s.width()-1; j>=0; j--) {
      offset += tdata[j+aoff];
      }
    offset /= s.width();

    offset += slope*(s.width()-1)/2;

    for ( int j = s.width()-1; j>=0; j--) {
      fdata[j+aoff] = tdata[j+aoff] - offset + slope*j;
      }
    }

  mode = LineSlopeLeveling;
  getMinMax();
  autoScaleColors();
}

#ifdef WITH_2DVIEW
void NVBTopoLeveler::levelByThreePoints(QRectF p1, QRectF p2, QRectF p3)
{
  stopInteractiveMode();

  QRectF pos = QRectF(QPoint(),resolution()); // the fact that the points are inside must be checked by viz

  QRect r1 = discretizeRect(pos & p1);
  QRect r2 = discretizeRect(pos & p2);
  QRect r3 = discretizeRect(pos & p3);

  double x1,x2,x3;
  double y1,y2,y3;
  double z1,z2,z3;

  x1 = r1.center().x();
  x2 = r2.center().x();
  x3 = r3.center().x();

  y1 = r1.center().y();
  y2 = r2.center().y();
  y3 = r3.center().y();

  z1 = getAverageOnDRect(r1);
  z2 = getAverageOnDRect(r2);
  z3 = getAverageOnDRect(r3);

  double a,b,c,d;

//   a = (y1-y2)*(z3-z2)-(y3-y2)*(z1-z2);
//   b = (z1-z2)*(x3-x2)-(z3-z2)*(x1-x2);
//   c = (x1-x2)*(z3-z2)-(x3-x2)*(y1-y2);
//   a /= c;
//   b /= c;
//   d = a*x2+b*y2+z2;

  a = (y2-y1)*z3+(y1-y3)*z2+(y3-y2)*z1;
  b = (x2-x1)*z3+(x1-x3)*z2+(x3-x2)*z1;
  c = (x2-x1)*y3+(x1-x3)*y2+(x3-x2)*y1;
  d = (x1*y2-x2*y1)*z3+(x3*y1-x1*y3)*z2+(x2*y3-x3*y2)*z1;

  a /= c;
  b /= -c;
  d /= -c;

  reset();

//   fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());

  const double * const tdata = tprovider->getData();
  QSize s = tprovider->resolution();
//  memcpy(fdata, tdata, s.width());

  int aoff,i,j;
  for ( i = s.height(), aoff = --i*s.width() ;
                       i>=0; aoff = --i*s.width()) {
    for ( j = s.width()-1; j>=0; j--) {
      fdata[j+aoff] = tdata[j+aoff] + a*j + b*i + d;
      }
    }

  bool wasLeveling = isLeveling;
  if (!isLeveling) emit dataAboutToBeChanged();

  mode = ThreePointsLeveling;

  isLeveling = true;

  getMinMax();
  autoScaleColors();

  if (!wasLeveling) emit dataChanged();
  else emit dataAdjusted();

}
#endif

void NVBTopoLeveler::setMode(Mode new_mode)
{
  if (mode == new_mode) {
    if (mode != NoLeveling && !isLeveling) {
      emit dataAboutToBeChanged();
      isLeveling = true;
      emit dataChanged();
      }
    }
  else if (new_mode == NoLeveling) {
    emit dataAboutToBeChanged();
    isLeveling = false;
    emit dataChanged();
    }
  else {
    bool wasLeveling = isLeveling;
    switch (new_mode) {
      case NoLeveling: break; // Cannot happen
#ifdef WITH_2DVIEW
      case Parabola: break;  // Cannot happen
#endif
      case LineLeveling: {
        if (!isLeveling) emit dataAboutToBeChanged();
        isLeveling = true;
        levelByLine();
        if (!wasLeveling) emit dataChanged();
        else emit dataAdjusted();
        break;
        }
      case OffsetLeveling: {
        if (!isLeveling) emit dataAboutToBeChanged();
        isLeveling = true;
        levelByOffset();
        if (!wasLeveling) emit dataChanged();
        else emit dataAdjusted();
        break;
        }
      case LineSlopeLeveling: {
        if (!isLeveling) emit dataAboutToBeChanged();
        isLeveling = true;
        levelByLineSlope();
        if (!wasLeveling) emit dataChanged();
        else emit dataAdjusted();
        break;
        }
#ifdef WITH_2DVIEW
      case ThreePointsLeveling: {
        vizcontroller = new NVBTopoLevelerViz(this);
        connect(vizcontroller,SIGNAL(pointsSelected(QRectF, QRectF, QRectF)),SLOT(levelByThreePoints(QRectF,QRectF,QRectF)));
        connect(vizcontroller,SIGNAL(selectionBroken()),this,SLOT(stopInteractiveMode()));
        vizcontroller->activatePointSelection();
        NVBVizUnion u(NVB::TopoPage,vizcontroller);
        u.filter = this;
        view->setActiveVisualizer(u);
        if (isLeveling) {
          emit dataAboutToBeChanged();
          isLeveling = false;
          emit dataChanged();
          }
        break;
        }
/*
      case Parabola: {
        levelWithParaboloid();
        break;
        }
*/
#endif
/*      default : {
        mode = NoLeveling;
        if (isLeveling) {
          isLeveling = false;
          }
        }*/
      }
    }
}



QAction * NVBTopoLeveler::action()
{
  return new QAction(QIcon(_lv_3ptslv),QString("Leveling"),0);
}

QWidget * NVBTopoLeveler::widget()
{
  NVBTopoLevelerWidget * widget = new NVBTopoLevelerWidget( isLeveling ? mode : NoLeveling , view->viewType());
  connect(widget,SIGNAL(levelingModeActivated( NVBTopoLeveler::Mode )),SLOT(setMode( NVBTopoLeveler::Mode )));
  connect(this,SIGNAL(delegateReset()),widget,SLOT(reset()));
  return widget;
}

void NVBTopoLeveler::recalculate()
{ // dataChanged is emitted by this function, since it can be called as a slot after dataChanged is emitted by the page itself
  if (mode == NoLeveling) {
    emit dataChanged();
    return;
    }
  switch (mode) {
    case LineLeveling: {
      levelByLine();
      break;
      }
    case LineSlopeLeveling: {
      levelByLineSlope();
      break;
      }
#ifdef WITH_2DVIEW
    case ThreePointsLeveling: {
      emit colorsAboutToBeChanged();
      isLeveling = false;
      emit colorsChanged();
      mode = NoLeveling;
      break;
      }
#endif
/*
    case : {
      break;
      }
*/
    default : {
      mode = NoLeveling;
      isLeveling = false;
      }
    }
  if (isLeveling)
    autoScaleColors();
  emit dataChanged();
}

void NVBTopoLeveler::setSource(NVBDataSource * source)
{

  if (tprovider) tprovider->disconnect(this);

  // If underlying page type is not good, self-destruct

  if (!source || source->type() != NVB::TopoPage) {
    emit objectPopped(source, this); // going away
    return;
    }

  emit dataAboutToBeChanged();

  parentColorsAboutToBeChanged();

  NVB3DFilterDelegate::setSource(source);

}

void NVBTopoLeveler::connectSignals()
{
  if (fdata) free(fdata);

  tprovider = (NVB3DDataSource*)provider;

  fdata = (double*)calloc(sizeof(double),tprovider->resolution().width()*tprovider->resolution().height());

  NVB3DFilterDelegate::connectSignals();

  connect(provider,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged()));
  connect(provider,SIGNAL(dataAdjusted()),SLOT(recalculate()));
  connect(provider,SIGNAL(dataChanged()),SLOT(recalculate()));
//     connect(provider,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));
  connect(provider,SIGNAL(colorsAdjusted()),SLOT(autoScaleColors()));
//     connect(provider,SIGNAL(colorsChanged()),SLOT(parentColorsChanged()));

  parentColorsChanged();

  recalculate();

}

#ifdef WITH_2DVIEW
QRect NVBTopoLeveler::discretizeRect(QRectF _rect) {

  QRect rect = _rect.toRect();
  if (rect.width() == 0) rect.setWidth(1);
  if (rect.height() == 0) rect.setHeight(1);
  return rect;
}

double NVBTopoLeveler::getAverageOnDRect(QRect rect)
{

  double level = 0;

  for (int i = rect.left(); i <= rect.right(); i++)
    for (int j = rect.top(); j <= rect.bottom(); j++)
      level += tprovider->getData(i,j);

  level /= rect.width()*rect.height();

  return level;
}
#endif

void NVBTopoLeveler::parentColorsAboutToBeChanged()
{
  colors->disconnect(this);
  colors->setModel(0);
}

void NVBTopoLeveler::parentColorsChanged()
{
  colors->setModel(tprovider->getColorModel());
  connect(colors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));
  autoScaleColors();
}

void NVBTopoLeveler::autoScaleColors()
{
  colors->setLimits(getZMin(),getZMax());
}

void NVBTopoLeveler::getMinMax()
{
  if (fdata)
    getMemMinMax<double>(fdata, resolution().width()*resolution().height(), zMin, zMax);
}

#ifdef WITH_2DVIEW
void NVBTopoLevelerViz::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);
//  NVB2DMapVizDelegate::paint(painter,option,widget);
  if (threepointing) {
    painter->save();

    painter->setPen(QPen(Qt::black));
    painter->setBrush(QBrush());

    painter->drawRect(mouserect);

    painter->setPen(QPen(Qt::red));
    foreach (QRectF r, points) {
      painter->drawRect(r);
      }

    painter->restore();
    }
}

void NVBTopoLevelerViz::activatePointSelection()
{
  setVisible(true);
#if QT_VERSION >= 0x040400
  setAcceptHoverEvents(true);
#endif
  setFlag(QGraphicsItem::ItemIsFocusable);
  setCursor(Qt::BlankCursor);

//   setFocus();
//  grabMouse();

  points.clear();
  // just setSize will make the rect non-square on non-square topographies
  // >> mouserect.setSize(QSizeF(provider->resolution())/10);
  // We have to scale to the minimum size
  double lside = qMin(provider->resolution().width(),provider->resolution().height())/10;
  mouserect.setSize(QSizeF(lside,lside));

  threepointing = true;

  update();
}

void NVBTopoLevelerViz::deactivatePointSelection()
{
  if (threepointing) {
//    unsetCursor();
//    ungrabMouse();
//     clearFocus();

//     setAcceptHoverEvents(false);
//     setFlag(QGraphicsItem::ItemIsFocusable, false);

//     scene()->removeItem(this);

    points.clear();
    threepointing = false;

//    emit selectionBroken();
//    update();
    }
}

void NVBTopoLevelerViz::wheelEvent(QGraphicsSceneWheelEvent * event)
{
  if (threepointing) {
    QRectF r = mouserect;
    mouserect.setSize(mouserect.size()*(1+event->delta()/400.0));
    mouserect.moveCenter(r.center());
    update(r | mouserect);
    }
  else
    event->ignore();
}

void NVBTopoLevelerViz::keyReleaseEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_Escape) {
    deactivatePointSelection();
//    update(mouserect);
    }
}

void NVBTopoLevelerViz::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  QRectF r = mouserect;
  mouserect.moveCenter(event->pos());

  if ( !boundingRect().contains(mouserect) ) {

    if ( mouserect.left() < 0 )
      mouserect.translate(-mouserect.left(),0);
    else if ( mouserect.right() > provider->resolution().width())
      mouserect.translate( provider->resolution().width() - mouserect.right() ,0);

    if ( mouserect.top() < 0 )
      mouserect.translate(0,-mouserect.top());
    else if ( mouserect.bottom() > provider->resolution().height())
      mouserect.translate(0, provider->resolution().height() - mouserect.bottom());

    }

  if (threepointing) { update(r | mouserect); }
}

void NVBTopoLevelerViz::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if (threepointing) {
    event->accept();
    if (!points.isEmpty() && points.last().center() == mouserect.center())
      points.removeLast();
    points << mouserect;
    if (points.size() == 3) {
      emit pointsSelected(points.at(0),points.at(1),points.at(2));
      }
    }
}

void NVBTopoLevelerViz::setSource(NVBDataSource * source)
{
  Q_UNUSED(source);
  deactivatePointSelection();
  emit selectionBroken();
//  NVB2DMapVizDelegate::setSource(source);
}

void NVBTopoLevelerViz::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  event->accept();
}
#endif

#ifdef WITH_2DVIEW
void NVBTopoLeveler::vizDeactivationRequest()
{
  setMode(NoLeveling);
  mode = NoLeveling;

  stopInteractiveMode();

  emit delegateReset();
}

void NVBTopoLeveler::stopInteractiveMode()
{
  emit detach2DViz();

  delete vizcontroller;
  vizcontroller = 0;
}
#endif 

void NVBTopoLevelerWidget::reset()
{
  actionCnt->actions()[0]->setChecked(true);
}

#ifdef WITH_2DVIEW
NVBTopoLevelerViz::NVBTopoLevelerViz(NVB3DDataSource * page): QObject(),NVBFilteringGraphicsItem(),provider(page),threepointing(false)
{
//   connect(this,SIGNAL(destroyed(QObject*)),SIGNAL(selectionBroken()));

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

}
#endif

void NVBTrackingRescaleColorModel::parentAdjusted()
{
  if ( source->zMin() != pzmin || source->zMax() != pzmax ) {
    zscaler.change_output(pzmin,pzmax,source->zMin(),source->zMax());
    pzmin = source->zMin();
    pzmax = source->zMax();
    }
  NVBRescaleColorModel::parentAdjusted();
}

