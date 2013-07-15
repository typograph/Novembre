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
#include "NVBPlotZoomer.h"

#include <QWheelEvent>
#include <math.h>
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_scale_div.h"

/*!
	\brief Create a zoomer for a plot canvas.

	The zoomer is set to those x- and y-axis of the parent plot of the
	canvas that are enabled. If both or no x-axis are enabled, the picker
	is set to QwtPlot::xBottom. If both or no y-axis are
	enabled, it is set to QwtPlot::yLeft.

	The selectionFlags() are set to
	QwtPicker::RectSelection | QwtPicker::ClickSelection, the
	tracker mode to QwtPicker::ActiveOnly.

	\param canvas Plot canvas to observe, also the parent object
	\param doReplot Call replot for the attached plot before initializing
									the zoomer with its scales. This might be necessary,
									when the plot is in a state with pending scale changes.

	\sa QwtPlot::autoReplot(), QwtPlot::replot(), setZoomBase()
*/
NVBPlotZoomer::NVBPlotZoomer(QwtPlotCanvas *canvas, bool doReplot):
	QwtPlotPicker(canvas) {
	if (canvas)
		init(RectSelection | ClickSelection, ActiveOnly, doReplot);
	}

/*!
	\brief Create a zoomer for a plot canvas.

	The selectionFlags() are set to
	QwtPicker::RectSelection | QwtPicker::ClickSelection, the
	tracker mode to QwtPicker::ActiveOnly.

	\param xAxis X axis of the zoomer
	\param yAxis Y axis of the zoomer
	\param canvas Plot canvas to observe, also the parent object
	\param doReplot Call replot for the attached plot before initializing
									the zoomer with its scales. This might be necessary,
									when the plot is in a state with pending scale changes.

	\sa QwtPlot::autoReplot(), QwtPlot::replot(), setZoomBase()
*/

NVBPlotZoomer::NVBPlotZoomer(int xAxis, int yAxis,
                             QwtPlotCanvas *canvas, bool doReplot):
	QwtPlotPicker(xAxis, yAxis, canvas) {
	if (canvas)
		init(RectSelection | ClickSelection, ActiveOnly, doReplot);
	}

//! Init the zoomer, used by the constructors
void NVBPlotZoomer::init(int selectionFlags,
                         DisplayMode trackerMode, bool doReplot) {

	setSelectionFlags(selectionFlags);
	setTrackerMode(trackerMode);
	setRubberBand(RectRubberBand);

	if (doReplot && plot())
		plot()->replot();

	}

NVBPlotZoomer::~NVBPlotZoomer() {
	}


/*!
	Rectangle at the current position on the zoom stack.

	\sa zoomRectIndex(), scaleRect().
*/
QwtDoubleRect NVBPlotZoomer::zoomRect() const {
	return zrect;
	}

/*!
	\brief Zoom in

	Clears all rectangles above the current position of the
	zoom stack and pushs the intersection of zoomRect() and
	the normalized rect on it.

	\note If the maximal stack depth is reached, zoom is ignored.
	\note The zoomed signal is emitted.
*/

void NVBPlotZoomer::zoom(const QwtDoubleRect &rect) {
	zrect = rect;
	rescale();
	emit zoomed(zrect);
	}


/*!
	Adjust the observed plot to zoomRect()

	\note Initiates QwtPlot::replot
*/

void NVBPlotZoomer::rescale() {
	QwtPlot *plt = plot();

	if (!plt)
		return;

	if (!zrect.isValid()) {
		plt->setAxisAutoScale(xAxis());
		plt->setAxisAutoScale(yAxis());
		}
	else if (zrect != scaleRect()) {
		const bool doReplot = plt->autoReplot();
		plt->setAutoReplot(false);

		double x1 = zrect.left();
		double x2 = zrect.right();
#if QWT_VERSION < 0x050200
		if (plt->axisScaleDiv(xAxis())->lBound() >
		    plt->axisScaleDiv(xAxis())->uBound()) {
#else
		if (plt->axisScaleDiv(xAxis())->lowerBound() >
		    plt->axisScaleDiv(xAxis())->upperBound()) {
#endif			
			qSwap(x1, x2);
			}

		plt->setAxisScale(xAxis(), x1, x2);

		double y1 = zrect.top();
		double y2 = zrect.bottom();

#if QWT_VERSION < 0x050200
		if (plt->axisScaleDiv(yAxis())->lBound() >
		    plt->axisScaleDiv(yAxis())->uBound()) {
#else
		if (plt->axisScaleDiv(yAxis())->lowerBound() >
		    plt->axisScaleDiv(yAxis())->upperBound()) {
#endif			
			qSwap(y1, y2);
			}

		plt->setAxisScale(yAxis(), y1, y2);

		plt->setAutoReplot(doReplot);

		plt->replot();
		}
	}

/*!
	Reinitialize the axes, and set the zoom base to their scales.

	\param xAxis X axis
	\param yAxis Y axis
*/

void NVBPlotZoomer::setAxis(int xAxis, int yAxis) {
	if (xAxis != QwtPlotPicker::xAxis() || yAxis != QwtPlotPicker::yAxis()) {
		QwtPlotPicker::setAxis(xAxis, yAxis);
		zrect = QwtDoubleRect();
		rescale();
		}
	}

/*!
	 Qt::RightButton autoscales plot.
*/
void NVBPlotZoomer::widgetMouseReleaseEvent(QMouseEvent *me) {
	if (mouseMatch(MouseSelect2, me)) {
		zrect = QwtDoubleRect();
		rescale();
		}
	else
		QwtPlotPicker::widgetMouseReleaseEvent(me);
	}

/*!
	 Ctrl-Wheel scales,
	 Shift-Wheel scrolls horizontally,
	 Wheel scrolls vertically.
*/
void NVBPlotZoomer::widgetWheelEvent(QWheelEvent *event) {
	QwtDoubleRect newrect = scaleRect();

	if (event->modifiers() & Qt::ControlModifier) { // zoom
		QPointF stable = invTransform(event->pos());

		qreal factor = pow(0.9, event->delta() / 120.0);
		qreal newW = newrect.width() * factor;
		qreal newH = newrect.height() * factor;

		qreal newX = (newrect.x() - stable.x()) * factor + stable.x();
		qreal newY = (newrect.y() - stable.y()) * factor + stable.y();

		newrect = QRectF(newX, newY, newW, newH);
		}
	else if (event->modifiers() & Qt::ShiftModifier) { // shift h
		qreal shift = -newrect.width() * 0.1 * event->delta() / 120.0;
		newrect.moveLeft(newrect.x() + shift);
		}
	else { // shift v
		qreal shift = -newrect.height() * 0.1 * event->delta() / 120.0;
		newrect.moveTop(newrect.y() + shift);
		}

// // Correct edges
//	if (newrect.contains(itemsRect))
//		newrect = itemsRect;
//	else if (!itemsRect.contains(newrect)) {
//		if (newrect.x() < itemsRect.x())
//			newrect.moveLeft(itemsRect.x());
//		if (newrect.right() > itemsRect.right())
//			newrect.moveRight(itemsRect.right());
//		if (newrect.y() < itemsRect.y())
//			newrect.moveTop(itemsRect.y());
//		if (newrect.bottom() > itemsRect.bottom())
//			newrect.moveBottom(itemsRect.bottom());
//		}

	zrect = newrect;

	rescale();
//		QwtPlotPicker::widgetWheelEvent(me);
	}

/*!
	\brief Check and correct a selected rectangle

	Reject rectangles with a hight or width < 2, otherwise
	expand the selected rectangle to a minimum size of 11x11
	and accept it.

	\return true If rect is accepted, or has been changed
					to a accepted rectangle.
*/

bool NVBPlotZoomer::accept(QwtPolygon &pa) const {
	if (pa.count() < 2)
		return false;

	QRect rect = QRect(pa[0], pa[int(pa.count()) - 1]).normalized();

	const int minSize = 2;

	if (rect.width() < minSize && rect.height() < minSize)
		return false;

	const int minZoomSize = 11;

	const QPoint center = rect.center();
	rect.setSize(rect.size().expandedTo(QSize(minZoomSize, minZoomSize)));
	rect.moveCenter(center);

	pa.resize(2);
	pa[0] = rect.topLeft();
	pa[1] = rect.bottomRight();

	return true;
	}

/*!
	Rejects selections, when the stack depth is too deep, or
	the zoomed rectangle is minZoomSize().
*/
void NVBPlotZoomer::begin() {
	QwtPlotPicker::begin();
	}

/*!
	Expand the selected rectangle to minZoomSize() and zoom in
	if accepted.

	\sa accept(), minZoomSize()
*/
bool NVBPlotZoomer::end(bool ok) {
	ok = QwtPlotPicker::end(ok);

	if (!ok)
		return false;

	QwtPlot *plot = NVBPlotZoomer::plot();

	if (!plot)
		return false;

	const QwtPolygon &pa = selection();

	if (pa.count() < 2)
		return false;

	QRect rect = QRect(pa[0], pa[int(pa.count() - 1)]).normalized();

	zrect = invTransform(rect).normalized();

//		const QwtDoublePoint center = zrect.center();
//		zrect.setSize(zrect.size().expandedTo(minZoomSize()));
//		zrect.moveCenter(center);

	rescale();

	return true;
	}
