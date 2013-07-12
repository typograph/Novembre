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
#ifndef NVBCIRCAVERAGER_H
#define NVBCIRCAVERAGER_H

#include "../core/NVBDataSource.h"
#include "../core/NVBDiscrColoring.h"
#include "../core/NVBFilterDelegate.h"
#include "../core/NVBGraphicsItems.h"

#include <QRect>
#include <QPoint>
#include <QRectF>
#include <QPointF>

class NVBViewController;
class QAction;

class NVBBullsEyeViz : public QObject, public NVBFilteringGraphicsItem {
		Q_OBJECT
	private:
		NVB3DDataSource * provider;
		double radius;
		QPointF center;
	public:
		NVBBullsEyeViz(NVB3DDataSource * leveler);
		virtual ~NVBBullsEyeViz() {;}

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget * = 0);
		virtual QRectF boundingRect() const { return QRectF(QPointF(), provider->resolution()); }

	protected:
		virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
		virtual void keyReleaseEvent(QKeyEvent * event);
		virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

	signals:
		void pointSelected(QPoint);
		void selectionBroken();

	};

class NVBCircAverager : public NVBSpecDataSource {
		Q_OBJECT
	private:
		NVB3DDataSource * provider;
		NVBDimension ld, hd;
		QwtArrayData * av_data;
		QPointF pt_at;

		NVBViewController * pr_window;

		NVBBullsEyeViz * my_bullseye;
		NVBConstDiscrColorModel * colors;

	public:
		NVBCircAverager(NVB3DDataSource * topo, NVBViewController * wnd);
		~NVBCircAverager();

		NVBVizUnion bullseye();

		static QAction * action();

		virtual QString name() const { return QString("Circular average"); }

		virtual NVBDimension xDim() const { return ld; }
		virtual NVBDimension yDim() const { return ld; }
		virtual NVBDimension zDim() const { return hd; }
		virtual NVBDimension tDim() const { return ld; }

		virtual const NVBDiscrColorModel * getColorModel()  const { return colors; }

		virtual QList<QPointF> positions() const { return av_data ? (QList<QPointF>() << pt_at) : QList<QPointF>();}
		virtual QList<QwtData*> getData() const { return av_data ? (QList<QwtData*>() << av_data) : QList<QwtData*>();}

		virtual QSize datasize() const { return QSize(av_data->size(), av_data ? 1 : 0);}

	private slots:
		void averageAround(QPoint);

	public slots:
		void vizDeactivationRequest();

	signals:
		void detach2DViz();
	};
#endif
