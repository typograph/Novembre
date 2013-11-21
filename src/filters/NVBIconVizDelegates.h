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
#ifndef NVBICONVIZ_H
#define NVBICONVIZ_H

#include "../core/dimension.h"
#include <QColor>
#include <qwt_data.h>
#include <QIconEngine>
#include <QIcon>
#include <QPainter>
#include "../core/NVBDataSource.h"
#include "../core/NVBGeneralDelegate.h"

/**
 * Visualisation as plain image of some kind. Used by the browser.
 */

#if QT_VERSION >= 0x040300
class NVBIconVizDelegate : public QObject, public QIconEngineV2 {
#else
class NVBIconVizDelegate : public QObject, public QIconEngine {
#endif
		Q_OBJECT
	protected:
//  NVBDataSource * provider;
		QImage* cache;
		void  connectSource(NVBDataSource * source) {
			connect(source, SIGNAL(dataChanged()), SLOT(redrawCache()));
			connect(source, SIGNAL(colorsChanged()), SLOT(redrawCache()));
//           connect(source, SIGNAL(  objectChanged(NVBDataSource*, NVBDataSource* ) ), SLOT( setSource(NVBDataSource*)) );
			}
	public :
#if QT_VERSION >= 0x040300
		NVBIconVizDelegate(NVBDataSource* source): QIconEngineV2(), cache(0) {
#else
		NVBIconVizDelegate(NVBDataSource* source): QIconEngine(), cache(0) {
#endif

			if (!source)
				NVBOutputError("NULL page");
			else
				connectSource(source);
			}
		virtual ~NVBIconVizDelegate() { if (cache) delete(cache);}

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) {
			if (cache) {
				QSize z = cache->size();
				z.scale(rect.size(), Qt::KeepAspectRatio);
				QRect nrect = QRect(QPoint(), z);
				nrect.moveCenter(rect.center());
				painter->drawImage(nrect, *cache);
				}
			}

	protected slots:
		virtual void redrawCache() = 0;
	public slots:
		virtual void setSource(NVBDataSource *) = 0;
	};

class NVBTopoIconDelegate : public NVBIconVizDelegate {
		Q_OBJECT
	private:
		NVB3DDataSource * page;
	public:
		NVBTopoIconDelegate(NVBDataSource * source);
		virtual ~NVBTopoIconDelegate() {;}
		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
	protected slots:
		virtual void redrawCache(const QSize & size);
		virtual void redrawCache() {
			if (cache)
				redrawCache(cache->size());
			}
	public slots:
		virtual void setSource(NVBDataSource *);
	};

class NVBSpecIconDelegate : public NVBIconVizDelegate {
		Q_OBJECT
	protected:
		virtual void redrawCache(const QSize & size);
		NVBSpecDataSource * page;
	public:
		NVBSpecIconDelegate(NVBDataSource * source);
		virtual ~NVBSpecIconDelegate() {;}

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
	protected slots:
		virtual void redrawCache() {
			if (cache)
				redrawCache(cache->size());
			}
	public slots:
		virtual void setSource(NVBDataSource * source);
	};

#endif
