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
#ifndef NVBVIZDELEGATE_H
#define NVBVIZDELEGATE_H

#include "../core/NVBGeneralDelegate.h"
#include <QIconEngine>
#include <QIcon>
#include <QGraphicsItem>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <qwt_plot_item.h>

/**
 * Visualisation delegates should subclass GeneralDelegate.
 * The main difference is that a specific function should be present,
 * and called the same way in all subclasses.
 * NVBVizUnion NVBVizDelegate::getVizItem();
 * The other possibility would be making the delegate the actual
 * item, but that would make the subclassing difficult.
 * One could always just return <this>.
 */

union NVBVizUnion {
	bool valid;
	void * p;
	QGraphicsItem* TwoDViz;
	QwtPlotItem * GraphViz;
	QIconEngine * IconViz;
// Qwt3D::Plot3D * ThreeDViz;
	};

Q_DECLARE_METATYPE(NVBVizUnion);

/**
 * Visualisation as a QGraphicsItem, for use in a QGraphicsScene.
 * The resulting shape should have real-world coordinates and sizes.
 */

class NVB2DVizDelegate : public QObject, public NVBGeneralDelegate {
		Q_OBJECT
	private:
	public :
		NVB2DVizDelegate(NVBDataSource* source): NVBGeneralDelegate(source) {;}
		virtual ~NVB2DVizDelegate() {;}

		virtual QGraphicsItem* getVizItem() = 0;
	};

Q_DECLARE_METATYPE(NVB2DVizDelegate*);

/**
 * Visualisation as a QwtPlotItem, for a QwtPlot used by SpecView.
 */

class NVBSpecVizDelegate : public QObject, public NVBGeneralDelegate {
		Q_OBJECT
	public :
		NVBSpecVizDelegate(NVBDataSource* source): NVBGeneralDelegate(source) {;}
		virtual ~NVBSpecVizDelegate() {;}

		virtual QwtPlotItem * getVizItem() = 0;
	};

Q_DECLARE_METATYPE(NVBSpecVizDelegate*);

/**
 * Visualisation in 3D. Used by 3DView.
 */

class NVB3DVizDelegate : public QObject, public NVBGeneralDelegate {
		Q_OBJECT
	public :
		NVB3DVizDelegate(NVBDataSource* _source): NVBGeneralDelegate(_source) {;}
		virtual ~NVB3DVizDelegate() {;}

//  virtual Qwt3D::Plot3D* getVizItem() =0;
	};

Q_DECLARE_METATYPE(NVB3DVizDelegate*);

/**
 * Visualisation as plain image of some kind. Used by the browser.
 */

#if QT_VERSION >= 0x040300
class NVBIconVizDelegate : public QObject, public NVBGeneralDelegate, public QIconEngineV2 {
#else
class NVBIconVizDelegate : public QObject, public NVBGeneralDelegate, public QIconEngine {
#endif
		Q_OBJECT
	protected:
		QImage* cache;
	public :
#if QT_VERSION >= 0x040300
		NVBIconVizDelegate(NVBDataSource* source): NVBGeneralDelegate(source), QIconEngineV2() {;}
#else
		NVBIconVizDelegate(NVBDataSource* source): NVBGeneralDelegate(source), QIconEngine() {;}
#endif
		virtual ~NVBIconVizDelegate() { if (cache) delete(cache);}

		virtual QIconEngine* getVizItem() {return this;}

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) { if (cache) painter->drawImage(rect, *cache); }

	};

Q_DECLARE_METATYPE(NVBIconVizDelegate*);

#endif
