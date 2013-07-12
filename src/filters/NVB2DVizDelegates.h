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
#ifndef NVB2DVIZ_H
#define NVB2DVIZ_H

#include "../core/NVBDataSource.h"
#include "../core/NVBGeneralDelegate.h"
#include "../core/NVBQuadTree.h"
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
//#include <QGraphicsItemGroup>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneWheelEvent>

#include "../core/NVBGraphicsItems.h"

/**
 * Visualisation as a QPixmapItem for a topopage.
 * Been there, done that.
 */

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
class NVB2DMapVizDelegate : public QObject, public QGraphicsItem {
		Q_OBJECT
	private:
		QImage * p_image;
	public:
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//   int type () const {return 7;}

#else

#include <QGraphicsPixmapItem>

class NVB2DMapVizDelegate : public QObject, public QGraphicsPixmapItem {
		Q_OBJECT
#endif
	protected:
		NVB3DDataSource* page;
	public :
		NVB2DMapVizDelegate(NVBDataSource* source);
		virtual ~NVB2DMapVizDelegate();

		virtual NVBVizUnion getVizItem();

	public slots:
		void redraw();
		virtual void setSource(NVBDataSource *);
	};

/**
 * Visualisation as a QGraphicsItem for a specpage.
 */

class QPicture;

class NVB2DPtsVizDelegate : public QObject, public NVBFilteringGraphicsItem {
		Q_OBJECT
	private:
		NVBSpecDataSource* page;
		qreal radius;

		NVBQuadTree positions;
		QPicture * cache;

	private slots:
		void initEllipses();

	public :
		NVB2DPtsVizDelegate(NVBDataSource* source);
		virtual ~NVB2DPtsVizDelegate();

		virtual NVBVizUnion getVizItem();

		virtual QRectF boundingRect() const;
		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	protected:
		virtual void wheelEvent(QGraphicsSceneWheelEvent * event);

	public slots:
		virtual void setSource(NVBDataSource *);
		void redraw();
	};

#endif
