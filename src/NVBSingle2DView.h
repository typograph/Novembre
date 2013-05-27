//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVB_SINGLE2DVIEW_H
#define NVB_SINGLE2DVIEW_H

#include <QtGui/QFrame>
#include <QtGui/QPixmap>
#include "NVBDataGlobals.h"

class NVBDataColorInstance;
class NVBDataSet;
class QPaintEvent;
class QResizeEvent;
class NVBAxisPhysMap;
class NVBAxes2DGridMap;
class NVBAxisWidget;

/**
 * A NVBSingle2DView is used by the browser to show individual pages when in application mode.
 * It converts an NVBDataSet to a picture.
 *
 * Unlike NVB2DIconEngine, this view respects maps on the axes.
 *
 **/

class NVBSingle2DView : public QFrame {
		Q_OBJECT
	private:
		NVBDataSet * plotData;
		NVBDataColorInstance * colors;
		QTransform transform;
		QPixmap cache;

//	QVector<axissize_t> slice;
		axisindex_t xi, yi;

		NVBAxes2DGridMap * map2D;
		NVBAxisPhysMap * mapX, * mapY;
		NVBAxisWidget * xAxis, * yAxis;

		int nx, ny;

	public:
		NVBSingle2DView(NVBDataSet* data, QWidget* parent);
		virtual ~NVBSingle2DView();

		virtual void paintEvent(QPaintEvent *);
		virtual void resizeEvent(QResizeEvent *);

	public slots:
		void setDataSet(NVBDataSet * data);
//	void setSliceIndexes(QVector<axissize_t> indexes = QVector<axissize_t>());
		inline void setXYAxes(QVector<axisindex_t> xy) {
			Q_ASSERT(xy.count() == 2);
			setXYAxes(xy.at(0), xy.at(1));
			}
		void setXYAxes(axisindex_t x, axisindex_t y);
		void regenerateImage();

	private slots:
		void dataSetDestoyed();
		void parentDataReformed();

	private:
		bool map(int x_widget, int y_widget, axissize_t& x_data, axissize_t& y_data);
	};

#endif
