//
// C++ Interface: NVBGraphView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBGRAPHVIEW_H
#define NVBGRAPHVIEW_H

#include <QFrame>
#include <QVBoxLayout>
#include <QToolBar>
#include "NVBDataView.h"
#include "NVBPageViewModel.h"
#include "NVBVizModel.h"
#include <qwt_plot.h>
#include <qwt_plot_item.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_zoomer.h>

/*
class NVBNarrowLinearScaleEngine : public QwtLinearScaleEngine {
public :
  NVBNarrowLinearScaleEngine();
  virtual ~NVBNarrowLinearScaleEngine();

  virtual void autoScale (int maxSteps, double &x1, double &x2, double &stepSize) const;
  virtual QwtScaleDiv divideScale (double x1, double x2, int numMajorSteps, int numMinorSteps, double stepSize=0.0) const;

  virtual QwtScaleTransformation * transformation () const;
};
*/

class NVBPhysScaleDraw : public QObject, public QwtScaleDraw {
Q_OBJECT
protected:
  NVBDimension scaledim;
  int scaleorder;
  QwtScaleWidget * scalewidget;
protected slots:
  virtual void updateMultiplier();
public:
  NVBPhysScaleDraw(NVBDimension dim, QwtScaleWidget * widget);
  virtual ~NVBPhysScaleDraw() {;}
  virtual QwtText label( double value ) const;
};

/**
  class NVBGraphView is a subclassed QwtPlot,
  meaning it creates a plot. The plot may be used to display
  any graph, normally meaning the spectroscopic data.
    Of course, any 2D dependence can be plotted in such a way,
  meaning that sections, for example, can also be visualized.
  This leads to a conclusion, that the view can not reject any page,
  undependent of its type -- even a topography can have a default
  representation. We use the toolsFactory->getDefaultDelegate
  to cope with this problem.
    The pages might have different X and Y units, and even if the
  units are the same, they may be on an extremely different scale.
  A plot has 2 X and 2 Y axes, allowing very complicated graphs,
  but I have decided to limit this to 1 X and 2 Y. Thus, it is possible
  to plot I(V) and dI/dV(V) on the same graph, but the topography
  will not be accepted with it.

	@author Timofey <timoty@pi-balashov>
*/
class NVBGraphView : public QFrame, public NVBDataView {
Q_OBJECT
private:
  QVBoxLayout* plotlayout;
  NVBVizModel * vizmodel;
  void addItemToPlot(QwtPlot * plot, NVBVizUnion tmp, NVBDataSource * source);

  bool d_show_grids;
	bool d_active_zoom;

  QList<NVBVizUnion> supraVizs;
  QList<QwtPlotGrid*> grids;
	QList<QwtPlotZoomer*> zoomers;

  QwtPlot * plotAt(int row);

public:
  NVBGraphView(NVBVizModel * model, QWidget* parent = 0);
  virtual ~NVBGraphView();

  QToolBar * generateToolbar(QWidget * parent) const;
  QAbstractListModel * getVizModel() { return vizmodel; }

  virtual bool hasWidthForHeight() const { return false; }
	virtual int  widthForHeight(int) const { return -1; }

	virtual bool acceptsPage(NVBDataSource *) const { return true; } // FIXME

  virtual const QWidget * w() const { return this; }
  virtual QWidget * w() { return this; }

public slots:
  void setActiveVisualizer( NVBVizUnion viz, const QModelIndex & index);
  void showGrid(bool);
	void activateZoom();
	void deactivateZoom();

protected slots:
  void activateVisualizer(int row);
  void deactivateVisualizer(int row);
  virtual void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
  virtual void rowsInserted ( const QModelIndex & parent, int start, int end );
  void updateVizs(const QModelIndex& start, const QModelIndex& end);

signals : 
  void activeVisualizerDeactivated();
//   void syncronizeYWidths( int width );

};

#endif
