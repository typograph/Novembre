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
#ifndef NVBGRAPHVIZ_H
#define NVBGRAPHVIZ_H

#include "../core/NVBDataSource.h"
#include "../core/NVBGeneralDelegate.h"
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>

class NVBCurveBunch : public QwtPlotItem {
	protected:
		QList<QwtPlotCurve*> curves;
		QRectF rect;
		void clear();
	public:
		NVBCurveBunch(const QwtText &title = QwtText()): QwtPlotItem(title) {;}
//  NVBCurveBunch(QList<QwtPlotCurve*> _curves, const QwtText &title=QwtText()):QwtPlotItem(title) {;}
		virtual ~NVBCurveBunch();

		virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem + 117; }

		virtual void addCurve(QwtPlotCurve * curve);
		virtual void addCurveFromData(const QwtData * data, const QColor & color = Qt::black);

		virtual void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &canvasRect) const;
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
		virtual void setSource(NVBDataSource *);
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
