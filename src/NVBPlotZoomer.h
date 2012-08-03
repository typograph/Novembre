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

class NVBPlotZoomer : public QwtPlotPicker
{
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
