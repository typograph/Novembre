//
// C++ Interface: NVBSpecVizDelegate
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBGRAPHVIZ_H
#define NVBGRAPHVIZ_H

#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>

class NVBCurveBunch : public QwtPlotItem {
protected:
  QList<QwtPlotCurve*> curves;
  QRectF rect;
  void clear();
public:
  NVBCurveBunch(const QwtText &title=QwtText()):QwtPlotItem(title) {;}
//  NVBCurveBunch(QList<QwtPlotCurve*> _curves, const QwtText &title=QwtText()):QwtPlotItem(title) {;}
  virtual ~NVBCurveBunch();

  virtual int rtti () const { return QwtPlotItem::Rtti_PlotUserItem + 117; }

  virtual void addCurve(QwtPlotCurve * curve);
  virtual void addCurveFromData(const QwtData * data, const QColor & color = Qt::black);

  virtual void draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &canvasRect) const;
  virtual QwtDoubleRect boundingRect() const {return rect;}

	void recalculateRect();

};

/**
 * Visualisation as a QwtPlotItem, for a QwtPlot used by SpecView.
 */

class NVBCurveVizDelegate : public QObject, public NVBCurveBunch {
Q_OBJECT
private:
  NVBSpecDataSource * page;
private slots:
	void refresh();
  void generateCurves();
  void paintCurves();
  void clear() {NVBCurveBunch::clear();}
	void recalculateRect() {NVBCurveBunch::recalculateRect();}

//   void parentColorsAboutToBeChanged();
//   void parentColorsChanged();
public :
  NVBCurveVizDelegate(NVBDataSource* source);
  virtual ~NVBCurveVizDelegate();

  virtual NVBVizUnion getVizItem();
public slots:
  virtual void setSource( NVBDataSource * );
signals:
	void dataChanged();
};

// /**
// * \a DEPRECATED Visualisation of a topographic page.
// */
// 
// class NVBTopoSliceVizDelegate : public QObject, public NVBCurveBunch {
// Q_OBJECT
// private:
//   NVB3DDataSource * page;
//   double * xs;
// private slots:
//   void generateCurves();
//   void clear() {
//     NVBCurveBunch::clear();
//     if (xs) {
//       delete[] xs;
//       xs = 0;
//       }
//     }
//   void refresh() { itemChanged(); }
// public :
//   NVBTopoSliceVizDelegate(NVBDataSource* source);
//   virtual ~NVBTopoSliceVizDelegate();
// 
//   virtual NVBVizUnion getVizItem();
// public slots:
//   virtual void setSource( NVBDataSource * );
// };

#endif
