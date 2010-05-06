//
// C++ Implementation: NVBSpecSlicer
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecSlicer.h"
#include "dimension.h"
#include <QHBoxLayout>
#include <QLabel>

NVBSpecSlicerSpinBox::NVBSpecSlicerSpinBox(NVBSpecDataSource * source,  QWidget * parent):QSpinBox(parent),dim(source->xDim()),x0(0),xstep(0),nsteps(0)
{
  setReadOnly(true);
  QRectF r = source->boundingRect();
  x0 = r.left();
  nsteps = source->datasize().width()-1;
  xstep = r.width()/nsteps;
  setMaximum(nsteps);
}

NVBSpecSlicerSpinBox::~ NVBSpecSlicerSpinBox()
{
}

QString NVBSpecSlicerSpinBox::textFromValue(int value) const
{
  return NVBPhysValue(x0 + value*xstep,dim).toString();
}

int NVBSpecSlicerSpinBox::valueFromText(const QString & text) const
{
  Q_UNUSED(text);
  return 0;
}

// -------------------

NVBSpecSlicerWidget::NVBSpecSlicerWidget(NVBSpecDataSource * source)
{
  QHBoxLayout * l = new QHBoxLayout(this);

  l->addWidget(new QLabel(source->getComment("X axis label")));
  valueBox = new NVBSpecSlicerSpinBox(source,this);
  l->addWidget(valueBox);
  connect(valueBox,SIGNAL(valueChanged(int)),SIGNAL(posChanged(int)));

  setLayout(l);
}

// -------------

NVBSpecSlicerDelegate::NVBSpecSlicerDelegate(NVBSpecDataSource * source):NVB3DFilterDelegate(source),page(source),slicepos(0)
{
  cache.fill(NULL,page->datasize().width());
  initSize();

  connect(page,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged()));
  connect(page,SIGNAL(dataChanged()),SLOT(invalidateCache()));
  connect(page,SIGNAL(dataChanged()),SIGNAL(dataChanged()));
//   connect(page,SIGNAL(colorsChanged()),SIGNAL(colorsChanged()));
  connect(page,SIGNAL(commentsChanged()),SIGNAL(commentsChanged()));

  connectSignals();
  followSource();

}

bool NVBSpecSlicerDelegate::setColorModel(NVBContColorModel * colorModel)
{
  if (colorModel) {
    colors = colorModel;
    emit colorsChanged();
    return true;
    }
  return false;
}

void NVBSpecSlicerDelegate::invalidateCache()
{
  foreach(double * data, cache) {
    if(data) free(data);
    }
  cache.fill(NULL,page->datasize().width());
}

void NVBSpecSlicerDelegate::fillCache()
{
  if (slicepos < 0 || slicepos >= cache.size()) return;
  if (cache.at(slicepos)) return;
  double * data = (double*)calloc(sizeof(data),resolution().width()*resolution().height());
  cache[slicepos] = data;
  getMinMax();
}

void NVBSpecSlicerDelegate::initSize()
{
  // Long and tedious calculation
  rect = page->occupiedArea();
//  datasize = (int)sqrt(page->positions().size());
}

void NVBSpecSlicerDelegate::getMinMax()
{
  if (!cache.at(slicepos))
    fillCache();
  getMemMinMax<double>(cache.at(slicepos), resolution().width()*resolution().height(), zMin, zMax);
}

NVBSpecSlicerPosTracker::NVBSpecSlicerPosTracker(NVBSpecDataSource * source):QObject(),QwtPlotItem()
{
  Q_UNUSED(source);
}

void NVBSpecSlicerPosTracker::draw(QPainter * painter, const QwtScaleMap & xMap, const QwtScaleMap & yMap, const QRect & canvasRect) const
{
  Q_UNUSED(painter);
  Q_UNUSED(xMap);
  Q_UNUSED(yMap);
  Q_UNUSED(canvasRect);
}
