//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "NVBGraphVizDelegates.h"
#include <qwt_plot.h>

void NVBCurveBunch::draw(QPainter * painter, const QwtScaleMap & xMap, const QwtScaleMap & yMap, const QRect & canvasRect) const {
	for (int i = 0; i < curves.size(); ++i) {
		curves.at(i)->draw(painter, xMap, yMap, canvasRect);
		}
	}

void NVBCurveBunch::addCurve(QwtPlotCurve * curve) {
	curves.append(curve);
	rect = rect.united(curve->boundingRect());
//  itemChanged(); // To minimize autoreplot overhead. You have to emit the signal in subclasses.
	}

void NVBCurveBunch::addCurveFromData(const QwtData * data, const QColor &color) {
	QwtPlotCurve* curve = new QwtPlotCurve();
	curve->setData(*data);
	colorCurve(curve,color);
	addCurve(curve);
	}

void NVBCurveBunch::colorCurve(QwtPlotCurve * curve, QColor color) {
	if (color.alpha() < 0xFF && color.alpha() >= 0xF0) {
		QColor c(color);
		c.setAlpha(0xFF);
		QPen p(QBrush(c),0xFF - color.alpha());
		curve->setPen(p);
		}
	else
		curve->setPen(QPen(color));
	}
	
void NVBCurveBunch::clear() {
	rect = QRectF();

	while (!curves.isEmpty()) delete curves.takeFirst();
	}

void NVBCurveBunch::repaintCurve(int i, const QColor & color) {
	colorCurve(curves[i],color);
	}
	
void NVBCurveBunch::recalculateRect() {
	rect = QRectF();

	if (!curves.empty())
		rect = curves.first()->boundingRect();

	foreach(QwtPlotCurve * c, curves)
	rect = rect.united(c->boundingRect());
	}

NVBCurveBunch::~ NVBCurveBunch() {
	clear();
	}

NVBCurveVizDelegate::NVBCurveVizDelegate(NVBDataSource * source): QObject(), NVBCurveBunch(), page(0) {
	setSource(source);
	}

NVBCurveVizDelegate::~ NVBCurveVizDelegate() {
	}

void NVBCurveVizDelegate::refresh() {
	recalculateRect();
	itemChanged();
	}

NVBVizUnion NVBCurveVizDelegate::getVizItem() {
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

void NVBCurveVizDelegate::generateCurves() {
	clear();

	QList<QwtData*> arrays = page->getData();
	QList<QColor> colors = page->colors();

	for (int i = 0; i < arrays.size(); i++)
		addCurveFromData(arrays.at(i), colors.at(i));

	itemChanged();

//  if (plot() && !plot()->autoReplot()) plot()->replot();
	}

void NVBCurveVizDelegate::paintCurves() {
	QList<QColor> colors = page->colors();

	if (colors.size() < curves.size())
		NVBOutputError("Not enough colors for repaint");
	else {
		for (int i = 0; i < curves.size(); i++)
			repaintCurve(i, colors.at(i));
		itemChanged();
//    if (plot()) plot()->replot();
		}
	}

void NVBCurveVizDelegate::setSource(NVBDataSource * source) {
	if (page) page->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		page = 0;
		clear();
		}
	else {
		page = (NVBSpecDataSource*)source;

		connect(page, SIGNAL(dataAboutToBeChanged()), SLOT(clear()));
		connect(page, SIGNAL(dataAdjusted()), SLOT(refresh()));
		connect(page, SIGNAL(dataChanged()), SLOT(generateCurves()));
//     connect(page,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));
		connect(page, SIGNAL(colorsAdjusted()), SLOT(paintCurves()));
		connect(page, SIGNAL(colorsChanged()), SLOT(paintCurves()));

		connect(page, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
		connect(page, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);

		generateCurves();
		}
	}

