//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef NVBDATASETICONS_H
#define NVBDATASETICONS_H

#include <QtCore/QHash>
#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
#include "NVBDataGlobals.h"
#include "NVBAxisSelector.h"

class NVBDataSet;
class NVBDataColorInstance;
class NVBAxisPointMap;
class NVBAxes2DGridMap;
class NVBFile;

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set);
QList<QIcon> createDataIcons(const NVBFile * file);
QList<QIcon> createSpecOverlayIcons(const NVBFile * file);

class NVB2DIconEngine : public QObject, public QIconEngineV2 {
		Q_OBJECT
	protected:
		const NVBDataSet * dset;
		NVBDataColorInstance * ci;
		QList<QPixmap> cache;
		NVBAxisSelector selector;
		NVBSelectorDataInstance si;
		bool flipX, flipY;

		void setFlip();

	public :

		NVB2DIconEngine(const NVBDataSet* dataset);
		NVB2DIconEngine(const NVB2DIconEngine & other);
		NVB2DIconEngine & operator=(const NVB2DIconEngine & other);
		virtual ~NVB2DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);

		static QPixmap colorizeWithPlaneSubtraction(NVBDataSet * page);

	protected slots:
		virtual void redrawCache();
		void setSource(const NVBDataSet * dataset = 0);
	};


class NVB1DIconEngine : public QObject, public QIconEngineV2 {
		Q_OBJECT
	protected:
		const NVBDataSet* dset;
		QHash<QSize, QPixmap> cache;

		NVBAxisSelector selector;
		NVBSelectorDataInstance instance;

		QPixmap drawCacheAt(QSize size);
	public:
		NVB1DIconEngine(const NVBDataSet* dataset);
		NVB1DIconEngine(const NVB1DIconEngine & other);
		NVB1DIconEngine & operator=(const NVB1DIconEngine & other);
		~NVB1DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
	protected slots:
		void setSource(const NVBDataSet * dataset = 0);
		void redrawCache() { cache.clear(); }
	};

class NVBMixTSIconEngine : public NVB2DIconEngine {
	private:
	protected:
		QList<QPointF> points;
	public :
		NVBMixTSIconEngine (const NVBDataSet* topo, const NVBAxisPointMap * points, axissize_t npnts);
		NVBMixTSIconEngine (const NVBDataSet* topo, const NVBAxes2DGridMap * points, axissize_t nptsx, axissize_t nptsy);
		virtual ~NVBMixTSIconEngine() {;}

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
	};

#endif
