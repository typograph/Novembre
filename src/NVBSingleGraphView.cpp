/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "NVBSingleGraphView.h"
#include "NVBPlotCurves.h"
#include "NVBPhysScaleDraw.h"
#include "qwt_scale_engine.h"
#include "qwt_plot_zoomer.h"
#include "qwt_scale_widget.h"
#include "NVBAxisMaps.h"

QwtPlotZoomer * zoomer;

NVBSingleGraphView::NVBSingleGraphView(NVBDataSet* dataset, QWidget* parent)
: QwtPlot(parent)
, ds(0)
{
	curves = new NVBPlotCurves(0);
	if (curves) {
		curves->attach(this);
		curves->setAxes(QwtPlot::xBottom,QwtPlot::yLeft);
		}
	else
		NVBOutputError("Not enough memory to create curves");

	setAxisAutoScale(QwtPlot::xBottom);
	axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating);

	setAxisAutoScale(QwtPlot::yLeft);
	axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Floating);
	
	setAutoReplot(true);
	setCanvasBackground(Qt::white);
	setAutoDelete(false); // The items are managed by the delegates, not by views

	zoomer = new QwtPlotZoomer(canvas());
	zoomer->setEnabled(true);

	
	setDataSet(dataset);
}

NVBSingleGraphView::~NVBSingleGraphView()
{
	setDataSet(0);
	if (curves) delete curves;
}

void NVBSingleGraphView::setDataSet(NVBDataSet* dataset)
{
	releaseDataSet(ds);
	ds = dataset;
	useDataSet(ds);
	if (curves) {
		curves->setDataSet(dataset);
		axisWidget(QwtPlot::yLeft)->setTitle(dataset->name());
		setAxisScaleDraw(QwtPlot::yLeft,new NVBPhysScaleDraw(dataset->dimension(),axisWidget(QwtPlot::yLeft)));
		zoomer->setZoomBase(curves->boundingRect());
		}
}


void NVBSingleGraphView::setXAxis(axisindex_t x)
{
	if (curves)
		curves->setXAxis(x);
	if (ds) {
		NVBAxis a = ds->axisAt(x);
		axisWidget(QwtPlot::xBottom)->setTitle(a.name());
		setAxisScaleDraw(QwtPlot::xBottom,new NVBPhysScaleDraw(a.physMap() ? a.physMap()->units() : NVBUnits(),axisWidget(QwtPlot::xBottom)));
		}
	
}

