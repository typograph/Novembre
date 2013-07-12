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
#ifndef NVBPLOTZOOMER_H
#define NVBPLOTZOOMER_H

#include "qwt_plot_picker.h"
#include "qwt_double_rect.h"

/*!
	\brief NVBPlotZoomer provides mousewheel zooming for a plot widget

	NVBPlotZoomer offers mousewheel and rubberband selections on the plot canvas,
	translating wheel events or the selected rectangles into plot coordinates
	and adjusting the axes to them.

	Unlike QwtPlotZoomer, NVBPlotZoomer does not keep a stack of zoom rectangles.
	Instead, it provides a way to zoom out with mouse wheel.

	Right click makes the plot autoscale.

*/

class NVBPlotZoomer : public QwtPlotPicker {
		Q_OBJECT
	public:
		explicit NVBPlotZoomer(QwtPlotCanvas *, bool doReplot = true);
		explicit NVBPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas *, bool doReplot = true);
		explicit NVBPlotZoomer(int xAxis, int yAxis, int selectionFlags, DisplayMode trackerMode, QwtPlotCanvas *,	bool doReplot = true);

		virtual ~NVBPlotZoomer();

		QwtDoubleRect zoomRect() const;

		virtual void setAxis(int xAxis, int yAxis);

	public slots:
		virtual void zoom(const QwtDoubleRect &);

	signals:
		void zoomed(const QwtDoubleRect &rect);

	protected:
		virtual void rescale();

//		virtual QwtDoubleSize minZoomSize() const;

		virtual void widgetWheelEvent(QWheelEvent *);
		virtual void widgetMouseReleaseEvent(QMouseEvent *);
//		virtual void widgetKeyPressEvent(QKeyEvent *);

		virtual void begin();
		virtual bool end(bool ok = true);
		virtual bool accept(QwtPolygon &) const;

	private:
		QwtDoubleRect zrect;
		void init(int selectionFlags, DisplayMode trackerMode, bool doReplot);

	};


#endif // NVBPLOTZOOMER_H
