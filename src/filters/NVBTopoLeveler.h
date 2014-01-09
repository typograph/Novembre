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
#ifndef NVBTOPOLEVELER_H
#define NVBTOPOLEVELER_H

#include "../core/NVBFilterDelegate.h"
#include "../core/NVBContColoring.h"
#include "../core/NVBViewController.h"
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QWidget>
#include <QRectF>
#ifdef WITH_2DVIEW
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include "../core/NVBGraphicsItems.h"
#endif

class NVBTrackingRescaleColorModel : public NVBRescaleColorModel {
		Q_OBJECT
	protected:
		double pzmin, pzmax;
	protected slots:
		virtual void parentAdjusted();
	public:
		NVBTrackingRescaleColorModel(const NVBContColorModel * model): NVBRescaleColorModel(model) {
			// this works, because the parent constructor calls its own parentAdjusted
			pzmin = model->zMin();
			pzmax = model->zMax();
			}
		NVBTrackingRescaleColorModel(double vmin, double vmax, double pvmin, double pvmax): NVBRescaleColorModel(vmin, vmax) {
			pzmin = pvmin;
			pzmax = pvmax;
			}
		virtual ~NVBTrackingRescaleColorModel() {;}
	};

class NVBTopoLeveler;

#ifdef WITH_2DVIEW
class NVBTopoLevelerViz : public QObject, public NVBFilteringGraphicsItem {
		Q_OBJECT
	private:
		NVB3DDataSource * provider;
		bool threepointing;
		QList<QRectF> points;
		QRectF mouserect;
	public:
		NVBTopoLevelerViz(NVB3DDataSource * leveler);
		virtual ~NVBTopoLevelerViz() {;}

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
		virtual QRectF boundingRect() const {
			/*    if (scene())
			#if QT_VERSION > 0x040299 && QT_VERSION < 0x040400
			      return invertQTransform(sceneTransform()).map(scene()->sceneRect()).boundingRect();
			#else
			      return mapFromScene(scene()->sceneRect()).boundingRect();
			#endif*/
			return QRectF(QPointF(), provider->resolution());
			}

	protected:
		virtual void wheelEvent(QGraphicsSceneWheelEvent * event);
		virtual void keyReleaseEvent(QKeyEvent * event);
		virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

	public slots:
		void activatePointSelection();
		void deactivatePointSelection();
		virtual void setSource(NVBDataSource *);
	signals:
		void pointsSelected(QRectF, QRectF, QRectF);
		void selectionBroken();

	};
#endif

class NVBTopoLeveler : public NVB3DFilterDelegate {
		Q_OBJECT
	public:
		enum Mode {
		  NoLeveling = 0,
		  LineLeveling,
		  OffsetLeveling,
		  LineSlopeLeveling,
#ifdef WITH_2DVIEW
		  ThreePointsLeveling,
#endif
		  Parabola
		};

	private:
		NVB3DDataSource * tprovider;
		NVBTrackingRescaleColorModel * colors;
#ifdef WITH_2DVIEW
		NVBTopoLevelerViz * vizcontroller;
#endif

		double * fdata;
		Mode mode;

		bool isLeveling;
		NVBViewController * view;

		void reset(bool resetmode = false);
		void levelByLine();
		void levelByOffset();
		void levelByLineSlope();
		void levelWithParabola();

#ifdef WITH_2DVIEW
		QRect discretizeRect(QRectF rect);
		double getAverageOnDRect(QRect rect);
#endif

//  void levelingIndexes(QList< QList< int > >);

	private slots:
		void recalculate();
		void getMinMax();
#ifdef WITH_2DVIEW
		void levelByThreePoints(QRectF, QRectF, QRectF);
		void stopInteractiveMode();
		// This is the forced one;
		void vizDeactivationRequest();
#endif
		void parentColorsAboutToBeChanged();
		void parentColorsChanged();
	public:

		NVBTopoLeveler(NVB3DDataSource* source, NVBViewController * wnd);
		virtual ~NVBTopoLeveler() {
#ifdef WITH_2DVIEW
			vizDeactivationRequest();
#endif

			if (colors) delete colors;

//    emit objectChanged(provider,this);
			}

		virtual inline double getZMin() const {
			if (isLeveling)
				return zMin;
			else
				return tprovider->getZMin();
			}
		virtual inline double getZMax() const {
			if (isLeveling)
				return zMax;
			else
				return tprovider->getZMax();
			}

		virtual inline const NVBContColorModel * getColorModel() const {
			if (isLeveling)
				return colors;
			else
				return tprovider->getColorModel();
			}

		virtual inline NVBDimension xDim() const { return tprovider->xDim();}
		virtual inline NVBDimension yDim() const { return tprovider->yDim();}
		virtual inline NVBDimension zDim() const { return tprovider->zDim();}

		virtual const double * getData() const {
			if (isLeveling)
				return fdata;
			else
				return tprovider->getData();
			}
		virtual double getData(int x, int y) const {
			if (isLeveling)
				return fdata[x + y * resolution().width()];
			else
				return tprovider->getData(x, y);
			}

		virtual inline QSize resolution() const { return tprovider->resolution(); }
		virtual inline QRectF position() const { return tprovider->position(); }
		virtual inline double rotation() const { return tprovider->rotation(); }

		static QAction * action();
		QWidget * widget();
//   NVBVizUnion viz();
	signals:
		void delegateReset();
#ifdef WITH_2DVIEW
		void detach2DViz();
#endif

	public slots:
		void autoScaleColors();
		void setMode(NVBTopoLeveler::Mode);
		virtual void setSource(NVBDataSource * source);

	private :
		void connectSignals();
	};

Q_DECLARE_METATYPE(NVBTopoLeveler::Mode);

class NVBTopoLevelerWidget : public QWidget {
		Q_OBJECT
	private:
		QActionGroup * actionCnt;
	private slots:
		void noLevelingModeActivated() { emit levelingModeActivated(NVBTopoLeveler::NoLeveling); }
		void lineLevelingModeActivated() { emit levelingModeActivated(NVBTopoLeveler::LineLeveling); }
		void offsetLevelingModeActivated() { emit levelingModeActivated(NVBTopoLeveler::OffsetLeveling); }
		void lineSlopeLevelingModeActivated() { emit levelingModeActivated(NVBTopoLeveler::LineSlopeLeveling); }
		void parabolaModeActivated() { emit levelingModeActivated(NVBTopoLeveler::Parabola); }
#ifdef WITH_2DVIEW
		void threePointsLevelingModeActivated() { emit levelingModeActivated(NVBTopoLeveler::ThreePointsLeveling); }
#endif
	public:
		NVBTopoLevelerWidget(NVBTopoLeveler::Mode mode, NVB::ViewType vtype,  QWidget * parent = 0);
		virtual ~NVBTopoLevelerWidget();
	signals:
		void levelingModeActivated(NVBTopoLeveler::Mode);
	public slots:
		void reset();
	};

#endif
