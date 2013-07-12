//
// Copyright 2006 Timofey <typograph@elec.ru>
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


#include "NVBGraphicsItems.h"
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include "NVBDimension.h"

// #include <QDebug>

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400

/// Workaround for a buggy Qt 4.3 QTransform.isInvertible()

QTransform invertQTransform(const QTransform & t) {

	double rdet = 1 / t.det();
	return QTransform(
	         (t.m22() * t.m33() - t.m23() * t.m32()) * rdet, // 11
	         (t.m32() * t.m13() - t.m33() * t.m12()) * rdet, // 12
	         (t.m12() * t.m23() - t.m13() * t.m22()) * rdet, // 13
	         (t.m23() * t.m31() - t.m21() * t.m33()) * rdet, // 21
	         (t.m33() * t.m11() - t.m31() * t.m13()) * rdet, // 22
	         (t.m13() * t.m21() - t.m11() * t.m23()) * rdet, // 23
	         (t.m21() * t.m32() - t.m22() * t.m31()) * rdet, // 31
	         (t.m31() * t.m12() - t.m32() * t.m11()) * rdet, // 32
	         (t.m11() * t.m22() - t.m12() * t.m21()) * rdet // 33
	       );
	}

#endif

bool NVBFilteringGraphicsItem::sceneEventFilter(QGraphicsItem * watched, QEvent * event) {
	if (!watched) return false;

	switch (event->type()) {
//     case QEvent::GraphicsSceneContextMenu :
//     case QEvent::GraphicsSceneDragEnter :
//     case QEvent::GraphicsSceneDragLeave :
//     case QEvent::GraphicsSceneDragMove :
//     case QEvent::GraphicsSceneDrop :
//     case QEvent::GraphicsSceneHelp :
//     case QEvent::GraphicsSceneMove :
//     case QEvent::GraphicsSceneResize :
		case QEvent::GraphicsSceneHoverEnter :
		case QEvent::GraphicsSceneHoverLeave :
		case QEvent::GraphicsSceneHoverMove : {
			QGraphicsSceneHoverEvent * e = (QGraphicsSceneHoverEvent*) event;
//       if (boundingRect().contains(e->scenePos())) {
//         QGraphicsSceneHoverEvent * new_e(e);

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
			e->setPos(invertQTransform(sceneTransform()).map(e->scenePos()));
#else
			e->setPos(mapFromScene(e->scenePos()));
#endif

#if QT_VERSION >= 0x040400
			e->setLastPos(mapFromScene(e->lastScenePos()));
#endif
			sceneEvent(e);
//         delete new_e;
			return true;
			/*        }
			      return false;*/
			}

		case QEvent::GraphicsSceneMouseDoubleClick :
		case QEvent::GraphicsSceneMouseMove :
		case QEvent::GraphicsSceneMousePress :
		case QEvent::GraphicsSceneMouseRelease : {
			QGraphicsSceneMouseEvent * e = (QGraphicsSceneMouseEvent*) event;

			if (mapRectToScene(boundingRect()).contains(e->scenePos())) {
//         QGraphicsSceneMouseEvent * new_e(e);
#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
				QTransform i = invertQTransform(sceneTransform());
				e->setPos(i.map(e->scenePos()));
				e->setLastPos(i.map(e->lastScenePos()));
				e->setButtonDownPos(Qt::LeftButton, i.map(e->buttonDownScenePos(Qt::LeftButton)));
				e->setButtonDownPos(Qt::RightButton, i.map(e->buttonDownScenePos(Qt::RightButton)));
				e->setButtonDownPos(Qt::MidButton, i.map(e->buttonDownScenePos(Qt::MidButton)));
#else
				e->setPos(mapFromScene(e->scenePos()));
				e->setLastPos(mapFromScene(e->lastScenePos()));
				e->setButtonDownPos(Qt::LeftButton, mapFromScene(e->buttonDownScenePos(Qt::LeftButton)));
				e->setButtonDownPos(Qt::RightButton, mapFromScene(e->buttonDownScenePos(Qt::RightButton)));
				e->setButtonDownPos(Qt::MidButton, mapFromScene(e->buttonDownScenePos(Qt::MidButton)));
#endif
				sceneEvent(e);
//         delete new_e;
				return true;
				}

			return false;
			}

		case QEvent::GraphicsSceneWheel: {
			QGraphicsSceneWheelEvent * e = (QGraphicsSceneWheelEvent*) event;

			if (mapRectToScene(boundingRect()).contains(e->scenePos())) {
//         QGraphicsSceneWheelEvent * new_e(e);
#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
				e->setPos(invertQTransform(sceneTransform()).map(e->scenePos()));
#else
				e->setPos(mapFromScene(e->scenePos()));
#endif
				sceneEvent(e);
//         delete new_e;
				return true;
				}

			return false;
			}

		default :
			return false;
		}

	return false;
	}

void NVBFullGraphicsItem::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *) {
	Q_UNUSED(painter);
	}

bool NVBFullGraphicsItem::sceneEvent(QEvent * event) {
	event->accept();
	return true;
	}

NVBFilteringGraphicsItem::NVBFilteringGraphicsItem(): QGraphicsItem() {
#if QT_VERSION >= 0x040400
	setAcceptHoverEvents(true);
#else
	setAcceptsHoverEvents(true);
#endif
	}

NVBFullGraphicsItem::NVBFullGraphicsItem(): QGraphicsItem() {
#if QT_VERSION >= 0x040400
	setAcceptHoverEvents(true);
#else
	setAcceptsHoverEvents(true);
#endif

	setZValue(100000000);
	}
