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
#ifndef NVBDISCRCOLORPAINTER_H
#define NVBDISCRCOLORPAINTER_H

#include "../core/NVBFilterDelegate.h"
#include "../core/NVBDiscrColoring.h"
#include "../core/NVBViewController.h"
#include "NVBColorButton.h"
#include "../core/NVBQuadTree.h"
#include <QList>
#include <QAction>
#include <QWidget>
#include <QRectF>
#include <QRubberBand>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QPainter>
#include <QPointer>
#include <QVector>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include "../core/NVBGraphicsItems.h"

class NVBSubstDiscrColorModel : public NVBDiscrColorModel {
		Q_OBJECT
	private:
		const NVBDiscrColorModel * provider;
		QList<QColor> colors;
	public:
		NVBSubstDiscrColorModel(NVBDiscrColorModel * source = 0);
		~NVBSubstDiscrColorModel() {;}
		virtual QColor colorize(int) const;

		void setColor(int index, QColor color, bool = true);
		void setColor(QList< int > indexes, QColor color);
	public slots:
		void setModel(const NVBDiscrColorModel * model);
	};

class NVBDiscrPainterViz : public QObject, public NVBFilteringGraphicsItem {
		Q_OBJECT
	protected:

		NVBQuadTree points;
		QVector< bool > touched;
		QBrush brush;
		NVBSpecDataSource * sprovider;

	public:
		NVBDiscrPainterViz(NVBSpecDataSource *): QObject(), NVBFilteringGraphicsItem(), sprovider(0) {;} // Do everything in subclasses
		virtual ~NVBDiscrPainterViz() {;}

	protected slots:
		virtual void setSource(NVBDataSource * source);
		virtual void refresh();

	public slots:
		virtual void setBrush(QBrush newbrush) {
			brush = newbrush;
			touched.fill(false);
			}
	signals:
		void pointsTouched(QList<int>);
	};

class NVBDiscrBrushPainterViz : public NVBDiscrPainterViz {
		Q_OBJECT
	private:
		bool active;
		QRectF rect;
		QRectF mouserect;
		bool showmouse;
	public:
		NVBDiscrBrushPainterViz(NVBSpecDataSource * source);
		virtual ~NVBDiscrBrushPainterViz();

		virtual inline QRectF boundingRect() const {
			if (scene() && !scene()->views().isEmpty()) {
				const QGraphicsView * v = scene()->views().at(0);
				return v->mapToScene(v->rect()).boundingRect();
				}

			return rect.adjusted(-mouserect.width() / 2, -mouserect.height() / 2, mouserect.width() / 2, mouserect.height() / 2);
			}
		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	protected slots:
		void refresh();

	protected:
		virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
//  virtual void keyReleaseEvent ( QKeyEvent * event );
		virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	public slots:
		void setBrush(QBrush newbrush) {
			NVBDiscrPainterViz::setBrush(newbrush);
			update(mouserect);
			}
	};

class NVBDiscrRectPainterViz : public NVBDiscrPainterViz {
		Q_OBJECT
	private:
		QRectF brect;
		QPoint rborigin;
		QPointF scorigin;
		QRubberBand * rubberBand;

	public:
		NVBDiscrRectPainterViz(NVBSpecDataSource * source);
		virtual ~NVBDiscrRectPainterViz();

		virtual inline QRectF boundingRect() const {
			if (scene() && !scene()->views().isEmpty()) {
				const QGraphicsView * v = scene()->views().at(0);
				return v->mapToScene(v->rect()).boundingRect();
				}

			return brect;
			}
		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	protected slots:
		void refresh();

	protected:
//  virtual void keyReleaseEvent ( QKeyEvent * event );
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	};

class NVBDiscrColorPainterDelegate : public NVBSpecFilterDelegate {
		Q_OBJECT
	private:
		NVBSpecDataSource * sprovider;
		NVBSubstDiscrColorModel * colors;
	public:
		NVBDiscrColorPainterDelegate(NVBSpecDataSource * source, NVBSubstDiscrColorModel * model);
		virtual ~NVBDiscrColorPainterDelegate() { if (colors) delete colors; }

		NVB_FORWARD_SPECDATA(sprovider);

		virtual inline const NVBDiscrColorModel * getColorModel()  const  { return colors; }

	protected slots:
		void parentColorsAboutToBeChanged();
		void parentColorsChanged();
		virtual void setSource(NVBDataSource * source);

	private :
		void connectSignals();

	};

class NVBDiscrColorPainter : public QWidget {
		Q_OBJECT
	private:
		NVBSubstDiscrColorModel * colors;
		NVBSpecDataSource * provider;
		NVBViewController * wparent;
		QPointer<NVBDiscrColorPainterDelegate> page;
		NVBDiscrPainterViz * painter;
		QColor ccolor;
		QActionGroup * tools;
		NVBColorButton * sliderColor;
		NVBColorButton * minSliderColor;
		NVBColorButton * maxSliderColor;
		void activatePainter(NVBDiscrPainterViz * viz);
	public:
		NVBDiscrColorPainter(NVBSpecDataSource * source, NVBViewController * wnd);
		virtual ~NVBDiscrColorPainter();

		NVBDataSource * filter() { return page; }
		static QAction * action();
	public slots:
		void getColor();
		void setColor(QColor color);
		void colorizePoints(QList<int> points);
		void activateBrushPainter();
		void activateRectPainter();
		void deactivatePainting();
		void vizDeactivationRequest();
		void setSource(NVBDataSource* source);
		void setSlidingColor(int);
	signals:
		void detach2DViz();
	};

#endif
