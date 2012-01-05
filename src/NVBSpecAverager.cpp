//
// C++ Implementation: NVBSpecAverager
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecAverager.h"
#include "NVBAverageColor.h"
#include <QIcon>
#include <QHBoxLayout>
#include <QToolButton>

#include "../icons/averager.xpm"

class NVBRenewableCPtData : public QwtData {
public:
     NVBRenewableCPtData(QwtArray<double> *x, QwtArray<double> *y) {
       d_x = x;
       d_y = y;
       }
     ~NVBRenewableCPtData() {
       if (d_x) delete d_x;
       if (d_y) delete d_y;
       }

     void setX(QwtArray<double> *x) {
       if (d_x) delete d_x;
       d_x = x;
       };

     void setY(QwtArray<double> *y) {
       if (d_y) delete d_y;
       d_y = y;
       };
     
     void setXY(QwtArray<double> *x, QwtArray<double> *y) {
       setX(x);
       setY(y);
       };

     virtual size_t size() const { return d_x->size();}
     virtual double x(size_t i) const { return d_x->at(i); };
     virtual double y(size_t i) const { return d_y->at(i); };
 
 private:
     QwtArray<double> *d_x;
     QwtArray<double> *d_y;
     size_t d_size;
};

NVBSpecAveragerWidget::NVBSpecAveragerWidget(NVBSpecAverager::Mode mode, QWidget * parent):QWidget(parent)
{
  setWindowTitle("Averaging");

  QHBoxLayout * l = new QHBoxLayout(this);
  actionCnt = new QActionGroup(this);

  // No averaging
{
  QAction * action = actionCnt->addAction(QIcon(_av_noav),"No averaging");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(noAverageModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBSpecAverager::NoAverage)
    action->setChecked(true);
}
  // Point averaging
{
  QAction * action = actionCnt->addAction(QIcon(_av_posav),"Average by point");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(posAverageModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBSpecAverager::PosAverage)
    action->setChecked(true);
}
  // Color averaging
{
  QAction * action = actionCnt->addAction(QIcon(_av_colorav),"Average by color");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(colorAverageModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBSpecAverager::ColorAverage)
    action->setChecked(true);
}
  // All averaging
{
  QAction * action = actionCnt->addAction(QIcon(_av_allav),"Average all");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(allAverageModeActivated()));
  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(QSize(32,32));
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBSpecAverager::AllAverage)
    action->setChecked(true);
}
  // Class averaging
{
/* // TODO find the good algorithm for this
  action = actionCnt->addAction(QIcon(_av_classav),"Average similar");
  action->setCheckable(true);
  connect(action,SIGNAL(triggered()),SLOT(classAverageModeActivated()));
  tb = new QToolButton(this);
  tb->setDefaultAction(action);
  tb->setMinimumSize(tb->iconSize());
  tb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  l->addWidget(tb);
  if (mode == NVBSpecAverager::ClassAverage)
    action->setChecked(true);
*/
}

  l->setSizeConstraint(QLayout::SetFixedSize);

  setLayout(l);
//  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

}

NVBSpecAveragerWidget::~ NVBSpecAveragerWidget()
{
}

NVBSpecAverager::NVBSpecAverager(NVBSpecDataSource * source):
    NVBSpecFilterDelegate(source),sprovider(0),
    fcolors(0),mode(NoAverage),isAveraging(false) {
  connectSignals();
  followSource();
}


void NVBSpecAverager::resetMode()
{
  isAveraging = false;
  mode = NoAverage;
}


void NVBSpecAverager::reset(bool resetmode)
{
  if (resetmode) resetMode();
  clearFData();
  if (fcolors) {
    delete fcolors;
    fcolors = 0;
    }
  fpositions.clear();
}

void NVBSpecAverager::averageAll()
{
  aindexes.clear();

  QList<int> allindexes;
  QSize s = sprovider->datasize();
  for (int i = 0; i<s.height(); i++)
    allindexes.append(i);
  aindexes.append(allindexes);

  mode = AllAverage;

  recalculateAll();
}

void NVBSpecAverager::averageColors()
{
  aindexes.clear();

  int cind;
  QList<QColor> cs = sprovider->colors();
  QList<QColor> uniqcolors;
  for (int i = 0; i<cs.size(); i++) {
    if ((cind = uniqcolors.indexOf(cs.at(i))) >= 0)
      aindexes[cind] << i;
    else {
      aindexes << QList<int>();
      aindexes[uniqcolors.size()] << i;
      uniqcolors << cs.at(i);
      }
    }

  mode = ColorAverage;

  recalculateAll();
}

void NVBSpecAverager::averagePos()
{
  aindexes.clear();

  int cind;
  QList<QPointF> pts = sprovider->positions();
  QList<QPointF> uniqpoints;
  for (int i = 0; i<pts.size(); i++) {
    if ((cind = uniqpoints.indexOf(pts.at(i))) >= 0)
      aindexes[cind] << i;
    else {
      aindexes << QList<int>();
      aindexes[uniqpoints.size()] << i;
      uniqpoints << pts.at(i);
      }
    }

  mode = PosAverage;

  recalculateAll();
}

void NVBSpecAverager::setMode(Mode new_mode)
{
  if (mode == new_mode) {
    if (mode != NoAverage && !isAveraging) {
      emit dataAboutToBeChanged();
      isAveraging = true;
      emit dataChanged();
      }
    }
  else {
    emit dataAboutToBeChanged();
    isAveraging = true;
    switch (new_mode) {
      case NoAverage: {
        isAveraging = false;
        break;
        }
      case AllAverage: {
        averageAll();
        break;
        }
      case PosAverage: {
        averagePos();
        break;
        }
      case ColorAverage: {
        averageColors();
        break;
        }
/*
      case : {
        break;
        }
*/
      default : {
        mode = NoAverage;
        isAveraging = false;
        }
      }
    emit dataChanged();
    }
}



QAction * NVBSpecAverager::action()
{
  return new QAction(QIcon(_av_colorav),QString("Average"),0);
}

QWidget * NVBSpecAverager::widget()
{
  NVBSpecAveragerWidget * widget = new NVBSpecAveragerWidget( isAveraging ? mode : NoAverage );
  connect(widget,SIGNAL(averageModeActivated( NVBSpecAverager::Mode )),SLOT(setMode( NVBSpecAverager::Mode )));
  return widget;
}

#if 0
void NVBSpecAverager::recalculate()
{
  if (mode == NoAverage) {
    emit dataChanged();
    return;
    }
  switch (mode) {
    case AllAverage: {
      averageAll();
      break;
      }
    case PosAverage: {
      averagePos();
      break;
      }
    case ColorAverage: {
      averageColors();
      break;
      }
/*
    case : {
      break;
      }
*/
    default : {
      mode = NoAverage;
      isAveraging = false;
      }
    }
//  if (isAveraging)
  emit dataChanged();
}
#endif

void NVBSpecAverager::setSource(NVBDataSource * source)
{
  if (sprovider) sprovider->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    emit objectPopped(source,this); // going away
    return;
    }

  NVBSpecFilterDelegate::setSource(source);

}

void NVBSpecAverager::connectSignals()
{
//### emit delegateReset();
    sprovider = (NVBSpecDataSource*)provider;

    emit dataAboutToBeChanged();

    isAveraging = false;
    mode = NoAverage;

    recalculateAll();
    emit dataChanged();

    connect(provider,SIGNAL(dataAdjusted()),SLOT(parentDataAdjusted()));
    connect(provider,SIGNAL(dataAboutToBeChanged()),SLOT(parentDataAboutToBeChanged()));
    connect(provider,SIGNAL(dataChanged()),SLOT(parentDataChanged()));
    connect(provider,SIGNAL(colorsAdjusted()),SLOT(parentColorsAdjusted()));
    connect(provider,SIGNAL(colorsChanged()),SLOT(parentColorsChanged()));
    connect(provider,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));

}


void NVBSpecAverager::recalculateAll()
{

  reset();

  QList<QwtData*> ds = sprovider->getData();
  QwtData * d = ds.at(0);
  // FIXME different Xs
  QwtArray< double > xs;
  for ( uint i = 0; i < d->size(); i++)
    xs << d->x(i);

  QList<QColor> cs = sprovider->colors();
  QList<QPointF> ps = sprovider->positions();
  fcolors = new NVBIndexedDiscrColorModel();

  foreach (QList<int> l, aindexes) {
    NVBAverageColor ac;
    if (l.size() > 0) {
      QwtArray< double > ys(sprovider->datasize().width(),0);
      QPointF p;
      foreach(int i, l) {
        for( int j = 0; j < ys.size(); j++)
          ys[j] += ds.at(i)->y(j);
        p += ps.at(i);
        ac << cs.at(i);
        }
      for(int i = 0; i < ys.size(); i++) 
        ys[i] /= l.size();
      p /= l.size();
      fdata << new QwtArrayData(xs,ys);
      (*fcolors) << ac.color();
      fpositions << p;
      }
    }

  connect(fcolors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));

}

void NVBSpecAverager::recalculateData()
{
//### Find a way not to change data pointers

  fpositions.clear();

  QList<QwtData*> ds = sprovider->getData();
//  QwtData * d = ds.at(0);

  QList<QPointF> ps = sprovider->positions();

  int k = 0;
  foreach (QList<int> l, aindexes) {
    if (l.size() > 0) {
      QwtArray< double > * ys = new QwtArray< double >(sprovider->datasize().width(),0);
      QPointF p;
      foreach(int i, l) {
        for( int j = 0; j < ys->size(); j++)
          (*ys)[j] += ds.at(i)->y(j);
        p += ps.at(i);
        }
      for(int i = 0; i < ys->size(); i++) 
        (*ys)[i] /= l.size();
      p /= l.size();
      ((NVBRenewableCPtData*)fdata[k++])->setY(ys);
      fpositions << p;
      }
    }

}

void NVBSpecAverager::recalculateColors()
{

  if (fcolors) {
    fcolors->disconnect(this);
    fcolors->clear();
    }
  else
    fcolors = new NVBIndexedDiscrColorModel();


  QList<QColor> cs = sprovider->colors();

  foreach (QList<int> l, aindexes) {
    NVBAverageColor ac;
    if (l.size() > 0) {
      QPointF p;
      foreach(int i, l)
        ac << cs.at(i);
      (*fcolors) << ac.color();
      }
    }

  connect(fcolors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));

}

void NVBSpecAverager::parentDataChanged()
{
  if (isAveraging)
    switch(mode) {
      case AllAverage : {
        emit dataAboutToBeChanged();
        averageAll();
        break;
        }
      case PosAverage : {
        emit dataAboutToBeChanged();
        averagePos();
        break;
        }
      case ColorAverage : {
        emit dataAboutToBeChanged();
        averageColors();
        break;
        }
      case NoAverage : 
      default : break;
      }

  emit dataChanged();
}

void NVBSpecAverager::parentDataAboutToBeChanged()
{
  if (!isAveraging) {
    resetMode();
    emit dataAboutToBeChanged();
    }
  else
    switch(mode) {
      case NoAverage : {
        emit dataAboutToBeChanged();
        break;
        }
      case AllAverage :
      case PosAverage :
      case ColorAverage :
      default : break;
      }
}

void NVBSpecAverager::parentDataAdjusted()
{
  if (!isAveraging) {
    resetMode();
    emit dataAdjusted();
    }
  else
    switch(mode) {
      case PosAverage :
      case ColorAverage :
      case AllAverage : {
        emit dataAboutToBeChanged();
        recalculateData();
        emit dataChanged();
        break;
        }
      case NoAverage :
      default : {
        emit dataAdjusted();
        break;
        }
      }
}

void NVBSpecAverager::parentColorsAdjusted()
{
  if (!isAveraging)
    resetMode(); // This should not happen for AllAverage, but we leave it like that
  else
    switch(mode) {
      case AllAverage :
      case PosAverage : {
        recalculateColors();
        emit colorsAdjusted();
        break;
        }
      case ColorAverage : {
        emit dataAboutToBeChanged();
        averageColors();
//        recalculateAll();
//        emit dataAdjusted();
        emit dataChanged();
        break;
        }
      case NoAverage : 
      default : {
        break;
        }
      }

  emit colorsAdjusted();
}

void NVBSpecAverager::parentColorsAboutToBeChanged()
{
  if (!isAveraging) {
    resetMode(); // This should not happen for AllAverage, but we leave it like that
    emit colorsAboutToBeChanged();
    }
  else
    switch(mode) {
      case NoAverage : {
        emit colorsAboutToBeChanged();
        break;
        }
      case PosAverage :
      case ColorAverage :
      case AllAverage :
      default : break;
      }
}

void NVBSpecAverager::parentColorsChanged()
{
  if (!isAveraging)
    emit colorsChanged();
  else
    switch(mode) {
      case PosAverage : {
        recalculateColors();
        emit colorsAdjusted();
        break;
        }
      case ColorAverage : {
        emit dataAboutToBeChanged();
        recalculateAll();
        emit dataChanged();
        break;
        }
      case AllAverage : break;
      case NoAverage : 
      default : {
        emit colorsChanged();
        break;
        }
      }
}

