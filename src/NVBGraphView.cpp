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
#include "NVBGraphView.h"
#include <qwt_text_label.h>

#include "../icons/icons_Gview.xpm"

#ifndef exp10
#define exp10(x) pow(10,x)
#endif

NVBGraphView::NVBGraphView(NVBVizModel * model, QWidget* parent)
	: QFrame(parent), vizmodel(model), d_show_grids(false), d_active_zoom(false) {

	plotlayout = new QVBoxLayout(this);

	if (vizmodel->rowCount() > 0)
		rowsInserted(QModelIndex(), 0, vizmodel->rowCount() - 1);

	connect(vizmodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), SLOT(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
	connect(vizmodel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(rowsInserted(const QModelIndex&, int, int)));
	connect(vizmodel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SLOT(updateVizs(const QModelIndex&, const QModelIndex&)));
	connect(vizmodel, SIGNAL(itemsSwapped(int, int)), SLOT(swapItems(int, int)));
	}


NVBGraphView::~NVBGraphView() {
	while (!grids.isEmpty())
		delete grids.takeFirst();
	}

void NVBGraphView::rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end) {
	Q_UNUSED(parent);

	for (int i = start; i <= end; i++) {
		QLayoutItem* item = plotlayout->itemAt(start);

		if (item) {
			QWidget * widget = item->widget();
			plotlayout->removeItem(item);

			if (widget) delete widget;
			}

		delete grids.takeAt(start);

		if (supraVizs.size() > start) supraVizs.removeAt(start);
		}
	}

void NVBGraphView::rowsInserted(const QModelIndex & parent, int start, int end) {
	Q_UNUSED(parent);

	for (int i = start; i <= end; i++) {
		QwtPlot* plot = new QwtPlot(vizmodel->index(i).data(Qt::DisplayRole).toString());

		plot->setAxisAutoScale(QwtPlot::xBottom);
		plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating);

		plot->setAxisAutoScale(QwtPlot::yLeft);
		plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Floating);

		plot->setAutoReplot(true);
		plot->setCanvasBackground(Qt::white);
		plot->setAutoDelete(false); // The items are managed by the delegates, not by views

		plot->canvas()->setFrameStyle(QFrame::NoFrame);

		plotlayout->insertWidget(i, plot);

		QwtPlotGrid * grid = new QwtPlotGrid();

		grids.insert(i, grid);

		grid->setMajPen(QPen(QBrush(Qt::gray), 1, Qt::DashLine));
		grid->setMinPen(QPen(QBrush(Qt::lightGray), 0.5, Qt::DotLine));

		grid->enableX(true);
		grid->enableXMin(true);
		grid->enableY(true);
		grid->enableYMin(true);

		grid->setZ(-100000);

		d_show_grids ? grid->show() : grid->hide();

		grid->attach(plot);

		NVBPlotZoomer * zoomer = new NVBPlotZoomer(plot->canvas());
		zoomer->setEnabled(d_active_zoom);
		zoomers.prepend(zoomer);

		NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();

		if (tmp.valid && tmp.vtype == NVB::GraphView) {
			NVBDataSource * source = vizmodel->index(i).data(PageRole).value<NVBDataSource*>();
			addItemToPlot(plot, tmp, source);
			}

		plot->replot();

		supraVizs.insert(i, NVBVizUnion());

		}
	}

QToolBar * NVBGraphView::generateToolbar(QWidget * parent) const {
	QToolBar * tBar = new QToolBar(parent);
	QAction * act;

	/*
	  act = tBar->addAction(QIcon(_2Dview_zoom),"Zoom view with window",this,SLOT(setZooming(bool)));
	  act->setCheckable(true);
	  act->setChecked(zooming);

	  act = tBar->addAction(QIcon(_2Dview_keepar),"Keep aspect ratio",this,SLOT(setCarefulZooming(bool)));
	  act->setCheckable(true);
	  act->setChecked(keepRatio);

	  tBar->addSeparator();

	  act = tBar->addAction(QIcon(_2Dview_scale),"Show ticks",this,SLOT(setShowTicks(bool)));
	  act->setCheckable(true);
	  act->setChecked(showTicks);
	    */

	act = tBar->addAction(QIcon(_Gview_grid), "Show grid");
	act->setCheckable(true);
	act->setChecked(false);
	connect(act, SIGNAL(toggled(bool)), this, SLOT(showGrid(bool)));

	act = tBar->addAction(QIcon(_Gview_zoom), "Zoom");
	act->setCheckable(true);
	act->setChecked(false);
	connect(act, SIGNAL(toggled(bool)), this, SLOT(activateZoom(bool)));

	return tBar;
	}

void NVBGraphView::updateVizs(const QModelIndex & start, const QModelIndex & end) {
	for (int i = start.row(); i <= end.row(); i++) {
		NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();

		if (tmp.valid && tmp.vtype == NVB::GraphView) {
			QwtPlot* plot = plotAt(i);

			if (plot) {
				NVBSpecDataSource * source = (NVBSpecDataSource*)vizmodel->index(i).data(PageRole).value<NVBDataSource*>();
				addItemToPlot(plot, tmp, source);
				}
			}
		}
	}

void NVBGraphView::addItemToPlot(QwtPlot * plot, NVBVizUnion tmp, NVBDataSource * source) {
	plot->axisWidget(QwtPlot::xBottom)->setTitle(source->getComment("X axis label"));

	if (source->type() == NVB::SpecPage)
		plot->setAxisScaleDraw(QwtPlot::xBottom, new NVBPhysScaleDraw(((NVBSpecDataSource*)source)->tDim(), plot->axisWidget(QwtPlot::xBottom)));
	else
		plot->setAxisScaleDraw(QwtPlot::xBottom, new NVBPhysScaleDraw(source->xDim(), plot->axisWidget(QwtPlot::xBottom)));

	plot->axisWidget(QwtPlot::yLeft)->setTitle(source->getComment("Z axis label"));
	plot->setAxisScaleDraw(QwtPlot::yLeft, new NVBPhysScaleDraw(source->zDim(), plot->axisWidget(QwtPlot::yLeft)));

	plot->titleLabel()->setToolTip(source->name());
	tmp.GraphViz->setItemAttribute(QwtPlotItem::AutoScale);
//   tmp.GraphViz->setItemAttribute(QwtPlotItem::AutoScale);
	tmp.GraphViz->attach(plot);
	tmp.GraphViz->setZ(0);
	}

void NVBPhysScaleDraw::updateMultiplier() {
	QwtScaleDiv d = scaleDiv();
#if QWT_VERSION < 0x050200
	int ol = NVBPhysValue::getPosMult(d.lBound(), -1, 2);
	int oh = NVBPhysValue::getPosMult(d.hBound(), -1, 2);
#else
	int ol = NVBPhysValue::getPosMult(d.lowerBound(), -1, 2);
	int oh = NVBPhysValue::getPosMult(d.upperBound(), -1, 2);
#endif
	/*  if (ol != 0 && oh != 0 && ol != oh) {
	    double l = d.lBound();
	    double h = d.hBound();
	    if (l<h)
	      d.setInterval(0,h);
	    else // h<l
	      d.setInterval(l,0);
	    cache = d;
	    // setScaleDiv(d);
	    }*/
//   if (ol == 0) ol = oh;
//   if (oh == 0) oh = ol;
	scaleorder = qMax(ol, oh);
	invalidateCache();
	scalewidget->setTitle(scalewidget->title().text().remove(QRegExp(" \\[.*\\]")) + QString(" [%1]").arg(scaledim.unitFromOrder(-scaleorder)));
	scalewidget->update();
	}

// void NVBPhysScaleDraw::draw(QPainter * painter, const QPalette & palette) const
// {
//   if (cache != scaleDiv()) {
//     updateMultiplier();
//     }
//   QwtScaleDraw::draw(painter,palette);
// }

NVBPhysScaleDraw::NVBPhysScaleDraw(NVBDimension dim, QwtScaleWidget * widget): QObject(), QwtScaleDraw(), scaledim(dim), scaleorder(0), scalewidget(widget) {
	connect(widget, SIGNAL(scaleDivChanged()), this, SLOT(updateMultiplier()));
	}

QwtText NVBPhysScaleDraw::label(double value) const {
	return QwtScaleDraw::label(round(value * exp10(scaleorder) * 1e+8) / 1e+8);
	}

void NVBGraphView::setActiveVisualizer(NVBVizUnion viz, const QModelIndex & index) {
	if (index.isValid() && supraVizs.size() > index.row()) {
		deactivateVisualizer(index.row());
		supraVizs.replace(index.row(), viz);
		activateVisualizer(index.row());
		}
	}

void NVBGraphView::activateVisualizer(int row) {
	NVBVizUnion u = supraVizs.at(row);

	if (u.isValid()) {
		QwtPlot * p = plotAt(row);

		if (!p) return;

		if (u.filter == u.GraphInteractiveViz) { // It really is a picker
			u.GraphInteractiveViz->setParent(p->canvas());
			}
		else { // It's not a picker
			u.GraphViz->attach(p);

			if (u.filter) {
				p->canvas()->installEventFilter(u.filter);
				}
			}

		/* TODO SIGNALS and SLOTS with mapper
		    if (u.filter) {
		      connect(u.filter,SIGNAL(),SLOT());
		      } */
		}
	}

void NVBGraphView::deactivateVisualizer(int row) {
	NVBVizUnion u = supraVizs.at(row);

	if (u.isValid()) {
		if (u.filter == u.GraphInteractiveViz) { // It really is a picker
			u.GraphInteractiveViz->setParent(0);
			}
		else { // It's not a picker
			u.GraphViz->detach();

			if (u.filter) {
				QwtPlot * p = plotAt(row);

				if (p) {
//           p->removeEventFilter(u.filter);
					p->canvas()->removeEventFilter(u.filter);
					}
				}
			}

//     emit activeVisualizerDeactivated();
		}
	}

QwtPlot * NVBGraphView::plotAt(int row) {
	return qobject_cast<QwtPlot*>(plotlayout->itemAt(row)->widget());
	}

void NVBGraphView::swapItems(int row1, int row2) {
	supraVizs.swap(row1, row2);
	grids.swap(row1, row2);
	zoomers.swap(row1, row2);
	int rx = qMax(row1, row2);
	QwtPlot * px = plotAt(rx);
	int rn = qMin(row1, row2);
	QwtPlot * pn = plotAt(rn);

	delete plotlayout->takeAt(rx);
	delete plotlayout->takeAt(rn);
	plotlayout->insertWidget(rn, px);
	plotlayout->insertWidget(rx, pn);
	}

void NVBGraphView::showGrid(bool gshow) {
	foreach(QwtPlotGrid * g, grids) {
		gshow ? g->show() : g->hide();
		}
	d_show_grids = gshow;
	}

void NVBGraphView::activateZoom(bool zoom) {
	if (d_active_zoom == zoom) return;

	foreach(NVBPlotZoomer * z, zoomers) {
		z->setEnabled(zoom);
		}

	d_active_zoom = zoom;
	}
