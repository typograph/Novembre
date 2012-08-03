//
// C++ Implementation: NVBSpecVizDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBGraphVizDelegates.h"
#include <qwt_plot.h>

void NVBCurveBunch::draw(QPainter * painter, const QwtScaleMap & xMap, const QwtScaleMap & yMap, const QRect & canvasRect) const
{
  for (int i = 0; i < curves.size(); ++i) {
    curves.at(i)->draw(painter,xMap,yMap,canvasRect);
  }
}

void NVBCurveBunch::addCurve(QwtPlotCurve * curve)
{
  curves.append(curve);
  rect = rect.united(curve->boundingRect());
//  itemChanged(); // To minimize autoreplot overhead. You have to emit the signal in subclasses.
}

void NVBCurveBunch::addCurveFromData(const QwtData * data, const QColor &color)
{
  QwtPlotCurve* curve = new QwtPlotCurve();
  curve->setData(*data);
  curve->setPen(QPen(color));
  addCurve(curve);
}

void NVBCurveBunch::clear()
{
  rect = QRectF();
  while(!curves.isEmpty()) delete curves.takeFirst();
}

void NVBCurveBunch::recalculateRect()
{
	rect = QRectF();
	if (!curves.empty())
		rect = curves.first()->boundingRect();

	foreach(QwtPlotCurve* c,curves)
		rect = rect.united(c->boundingRect());
}

NVBCurveBunch::~ NVBCurveBunch()
{
  clear();
}

NVBCurveVizDelegate::NVBCurveVizDelegate(NVBDataSource * source):QObject(),NVBCurveBunch(),page(0)
{
//	connect(this,SIGNAL(dataChanged()),this,SLOT(recalculateRect()));
  setSource(source);
}

NVBCurveVizDelegate::~ NVBCurveVizDelegate()
{
}

void NVBCurveVizDelegate::refresh()
{
	recalculateRect();
	itemChanged();
	emit dataChanged();
}

NVBVizUnion NVBCurveVizDelegate::getVizItem()
{
  NVBVizUnion u;
  u.GraphViz = this;
  return u;
}

// NVBTopoSliceVizDelegate::NVBTopoSliceVizDelegate(NVBDataSource * source):QObject(),NVBCurveBunch(),page(0),xs(0)
// {
//   setSource(source);
// }
// 
// void NVBTopoSliceVizDelegate::generateCurves() {
// 
//   clear();
// 
//   xs = new double[page->resolution().width()];
//   for (int i=0; i<page->resolution().width(); i++) {
//     xs[i] = page->position().left() + i*page->position().width()/page->resolution().width();
//     }
// 
//   for (int i=0; i<page->resolution().height(); i++) {
//     addCurveFromData(new QwtCPointerData(xs,page->getData()+i*page->resolution().width(),page->resolution().width()),Qt::black);
//     }
// 
//   itemChanged();
// //  if (plot() && !plot()->autoReplot()) plot()->replot();
// }
// 
// NVBTopoSliceVizDelegate::~ NVBTopoSliceVizDelegate()
// {
//   if (xs) delete[] xs;
// }
// 
// void NVBTopoSliceVizDelegate::setSource(NVBDataSource * source)
// {
//   if (page) page->disconnect(this);
// 
//   if (source->type() != NVB::TopoPage) {
//     page = 0;
//     clear();
//     }
//   else {
//     page = (NVB3DDataSource*)source;
//   
//     connect(page,SIGNAL(dataAboutToBeChanged()),SLOT(clear()));
//     connect(page,SIGNAL(dataAdjusted()),SLOT(refresh()));
//     connect(page,SIGNAL(dataChanged()),SLOT(generateCurves()));
// //     connect(page,SIGNAL(colorsAdjusted()),SLOT(refresh())); // Topography colors are irrelevant
// //     connect(page,SIGNAL(colorsChanged()),SLOT(paintCurves()));
//     connect(page,SIGNAL(objectChanged(NVBDataSource*, NVBDataSource* )),SLOT(setSource(NVBDataSource*)));
//   
//     generateCurves();
//     }
// }
// 
// NVBVizUnion NVBTopoSliceVizDelegate::getVizItem()
// {
//   NVBVizUnion u;
//   u.GraphViz = this;
//   return u;
// }

void NVBCurveVizDelegate::generateCurves()
{
  clear();

  QList<QwtData*> arrays = page->getData();
  QList<QColor> colors = page->colors();

  for (int i=0; i<arrays.size(); i++)
    addCurveFromData(arrays.at(i),colors.at(i));

  itemChanged();
	emit dataChanged();

//  if (plot() && !plot()->autoReplot()) plot()->replot();
}

void NVBCurveVizDelegate::paintCurves()
{
  QList<QColor> colors = page->colors();
  if (colors.size() < curves.size())
    NVBOutputError("Not enough colors for repaint");
  else {
    for (int i = 0; i<curves.size(); i++) {
      curves[i]->setPen(QPen(colors.at(i)));
      }

		itemChanged();
//    if (plot()) plot()->replot();
    }
}

void NVBCurveVizDelegate::setSource(NVBDataSource * source)
{
  if (page) page->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    page = 0;
    clear();
    }
  else {
    page = (NVBSpecDataSource*)source;
  
    connect(page,SIGNAL(dataAboutToBeChanged()),SLOT(clear()));
    connect(page,SIGNAL(dataAdjusted()),SLOT(refresh()));
		connect(page,SIGNAL(dataChanged()),SLOT(generateCurves()));
//     connect(page,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));
    connect(page,SIGNAL(colorsAdjusted()),SLOT(paintCurves()));
    connect(page,SIGNAL(colorsChanged()),SLOT(paintCurves()));

    connect(page,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)));
    connect(page,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )),SLOT(setSource(NVBDataSource*)),Qt::QueuedConnection);
  
    generateCurves();
    }
}

