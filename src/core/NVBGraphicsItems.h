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


#ifndef NVBGRAPHICSITEM_H
#define NVBGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
QTransform invertQTransform(const QTransform & t);
#endif

class NVBFullGraphicsItem : public QGraphicsItem {
	private :
//  QGraphicsItem ** subcontrol;
	public :
		NVBFullGraphicsItem();
		virtual ~NVBFullGraphicsItem() {;}
		virtual inline QRectF boundingRect() const {
			static bool protect = false;

			if (protect) return QRectF();

			if (!scene()) return QRectF();

			protect = true;
			QRectF rect = scene()->sceneRect();
			protect = false;
			return rect;
			}
		virtual void paint(QPainter * , const QStyleOptionGraphicsItem * , QWidget *  = 0);

	protected :
		virtual bool sceneEvent(QEvent * event);
		/*
		  virtual bool sceneEvent ( QEvent * event ) {
		    if (!*subcontrol) return false;
		    return (*subcontrol)->sceneEvent(event);
		    }
		*/
	};


class NVBFilteringGraphicsItem : public QGraphicsItem {
	protected :
		bool sceneEventFilter(QGraphicsItem * watched, QEvent * event);
	public :
		NVBFilteringGraphicsItem();
		virtual ~NVBFilteringGraphicsItem() {;}
	};

#endif
