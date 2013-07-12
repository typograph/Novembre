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
#include "NVB2DVizDelegates.h"
#include <QtGui/QPicture>

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400

NVB2DMapVizDelegate::NVB2DMapVizDelegate(NVBDataSource * source): QObject(), QGraphicsItem(), page(0) {
	p_image = 0;
	setSource(source);
	}

NVB2DMapVizDelegate::~ NVB2DMapVizDelegate() {
	if (p_image) delete p_image;
	}

QRectF NVB2DMapVizDelegate::boundingRect() const {
	float halfPw = 0.5;
//  return QRectF(0,0,10,10);
	return (!p_image || p_image->isNull()) ? QRectF() : QRectF(pos(), p_image->size()).adjusted(-halfPw, -halfPw, halfPw, halfPw);
	}

void NVB2DMapVizDelegate::paint(QPainter * painter, const QStyleOptionGraphicsItem * /* option */, QWidget *) {
	if (p_image) {
		/*    QRectF exposed = option->exposedRect.adjusted(-1, -1, 1, 1);
				exposed &= QRectF(x(), y(), p_image->width(), p_image->height());
				exposed.translate(pos());*/
		painter->drawImage(pos() - QPointF(-0.5, 0.5), *p_image);
		}
	}

void NVB2DMapVizDelegate::redraw() {
	if (p_image) delete p_image;

	p_image = page->getColorModel()->colorize(page->getData(), page->resolution());
	update();
	}

#else

NVB2DMapVizDelegate::NVB2DMapVizDelegate(NVBDataSource * source): QObject(), QGraphicsPixmapItem(), page(0) {
	setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	setSource(source);
	}

NVB2DMapVizDelegate::~ NVB2DMapVizDelegate() {
	}

void NVB2DMapVizDelegate::redraw() {
	QImage* tmpImage = page->getColorModel()->colorize(page->getData(), page->resolution());
	setPixmap(QPixmap::fromImage(*tmpImage));
	update();
	delete tmpImage;
	}

#endif

NVBVizUnion NVB2DMapVizDelegate::getVizItem() {
	NVBVizUnion u;
	u.TwoDViz = this;
	return u;
	}

void NVB2DMapVizDelegate::setSource(NVBDataSource * source) {
	if (page) {
		page->disconnect(this);
		disconnect(page->getColorModel(), 0, this, 0);
		}

	if (source->type() != NVB::TopoPage) {
		page = 0;
#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400

		if (p_image) delete p_image;

		p_image = 0;
#else
		setPixmap(QPixmap());
#endif
		}
	else {
		page = (NVB3DDataSource*)source;

		setPos(page->position().topLeft());

#if QT_VERSION >= 0x040300
		setTransform(
		  QTransform().scale(
		    page->position().width() / page->resolution().width(),
		    page->position().height() / page->resolution().height()
		  )
		);
#else
		setMatrix(
		  QMatrix().scale(
		    page->position().width() / page->resolution().width(),
		    page->position().height() / page->resolution().height()
		  )
		);
#endif
		/* This method requires reseting the scale, not provided by the API
				scale(
						page->position().width()/page->resolution().width(),
						page->position().height()/page->resolution().height()
						);
		*/

		setTransformOriginPoint(page->resolution().width() / 2, page->resolution().height() / 2);
		setRotation(page->rotation());

		connect(page, SIGNAL(dataChanged()), SLOT(redraw()));
		connect(page, SIGNAL(dataAdjusted()), SLOT(redraw()));
		connect(page, SIGNAL(colorsChanged()), SLOT(redraw()));
		connect(page, SIGNAL(colorsAdjusted()), SLOT(redraw()));
		connect(page, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
		connect(page, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);

		redraw();
		}
	}
// ===============

NVB2DPtsVizDelegate::NVB2DPtsVizDelegate(NVBDataSource * source): QObject(), NVBFilteringGraphicsItem(), page(0), radius(0), cache(0) {
	if (source->type() != NVB::SpecPage)
		NVBOutputError("Not a spectroscopy page");

	setSource(source); // will react correctly to topography
	}

NVB2DPtsVizDelegate::~ NVB2DPtsVizDelegate() {

	}

NVBVizUnion NVB2DPtsVizDelegate::getVizItem() {
	return NVBVizUnion();
	}

QRectF NVB2DPtsVizDelegate::boundingRect() const {
	return positions.rect().adjusted(-radius, -radius, radius, radius);
	}

void NVB2DPtsVizDelegate::initEllipses() {
	if (!page) return;

	prepareGeometryChange();

	positions.setRect(page->occupiedArea());
	int ipt = 0;
	foreach(QPointF pt, page->positions()) {
		positions.insert(pt, ipt);
		ipt += 1;
		}

	redraw();

	update();

	}

void NVB2DPtsVizDelegate::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget *) {
	if (!page || !cache) return;

	cache->play(painter);
	}

void NVB2DPtsVizDelegate::redraw() {
	QPainter painter;
	cache->setData(0, 0);

	painter.begin(cache);

	painter.setPen(QPen(Qt::NoPen));

	const NVBDiscrColorModel * clrs = page->getColorModel();

	QRectF e;
	e.setWidth(radius * 2);
	e.setHeight(radius * 2);

	foreach(NVBQuadTree::PointData pair, positions.points()) {
		e.moveCenter(pair.first);
		int i = 0;
		int span = 360 * 16 / pair.second.size();
		foreach(QVariant v, pair.second) {
			int ie = v.toInt();
			painter.setBrush(clrs->colorize(ie));
			painter.drawPie(e, i * span, span);
			i += 1;
			}
		}

	painter.end();

	update();

	}


void NVB2DPtsVizDelegate::wheelEvent(QGraphicsSceneWheelEvent * event) {
	prepareGeometryChange();

	radius += radius * event->delta() / 400.0;

//	QRectF e;
//	e.setWidth(radius*2);
//	e.setHeight(radius*2);
//	foreach(NVBQuadTree::PointData pair, positions.points()) {
//		e.moveCenter(pair.first);
//		foreach(QVariant v, pair.second)
//			ellipses[v.toInt()]->setRect(e);
//		}

	redraw();

	}

void NVB2DPtsVizDelegate::setSource(NVBDataSource * source) {
	if (page) page->disconnect(this);

	if (cache) delete cache;

	if (source->type() != NVB::SpecPage) {
		page = 0;
		}
	else {
		page = (NVBSpecDataSource*)source;
		cache = new QPicture();

		//connect(page,SIGNAL(dataAdjusted()),SLOT(initEllipses())); // Changes in the curves do not matter
		connect(page, SIGNAL(dataChanged()), SLOT(initEllipses()));
		connect(page, SIGNAL(colorsAdjusted()), SLOT(redraw()));
		connect(page, SIGNAL(colorsChanged()), SLOT(redraw()));
		connect(page, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
		connect(page, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);

		if (radius == 0) {
			/*			qreal pw = page->occupiedArea().width()/sqrt(positions.size())/10;
						qreal ph = page->occupiedArea().height()/sqrt(positions.size())/10;
						radius = qMin(pw,ph);*/
			QRectF oa = page->occupiedArea();
			radius = (oa.width() + oa.height()) / sqrt(page->datasize().height()) / 3;

//       if (radius == 0) radius = (pw == 0) ? ph : pw;
			if (radius == 0) {
				if (scene()) radius = scene()->sceneRect().width() / 50;
				else radius = 1e-9; // 1nm should be OK
				}

//       NVBOutputError(QString("w: %1, h: %2, r: %3").arg(pw).arg(ph).arg(radius));
			}
		}

	initEllipses();
	}


