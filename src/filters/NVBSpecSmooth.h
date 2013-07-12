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
#ifndef NVBSPECSMOOTH_H
#define NVBSPECSMOOTH_H

#include "../core/NVBFilterDelegate.h"

class QAction;

class NVBSpecSmoother : public NVBSpecFilterDelegate {
		Q_OBJECT
	private:
		NVBSpecDataSource * sprovider;
		double env_width;
		bool equalSpacing;
		int ptsFactor;
		QwtArray<double> tdata;

		QwtArray<double> xPoints();
		double wrap(double center, const QwtData * data);
		double envelope(double x);

		QList<QwtData*> fdata;

	public:
		NVBSpecSmoother(NVBSpecDataSource * data);
		virtual ~NVBSpecSmoother();

		virtual inline double getZMin() const {return boundingRect().bottom();}
		virtual inline double getZMax() const {return boundingRect().top();}

		NVB_FORWARD_SPECDIM(sprovider);
		NVB_FORWARD_SPECCOLORS(sprovider);
		NVB_FORWARD_COMMENTS(sprovider);

		virtual inline QList<QPointF> positions() const { return sprovider->positions(); }

		virtual QList<QwtData*> getData() const { return fdata; }

		virtual QSize datasize() const;

		virtual inline QRectF boundingRect() const { return NVBSpecDataSource::boundingRect(); }
		virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

		static QAction * action();
		QWidget * widget();

	public slots:
		virtual void setSource(NVBDataSource * source);

		void setWidth(const NVBPhysValue & width);
		void setEqualSpacing(bool equal);
		void setSubdivisions(int div);

	private slots:

//   void reset(bool resetmode = false);
//   void averageAll();
//   void averagePos();
//   void averageColors();
//
//   void resetMode();
		void clearFData() {
			while (!fdata.isEmpty())
				delete fdata.takeLast();
			}

		void calculate();

//   void parentDataAdjusted();
//   void parentDataAboutToBeChanged();
//   void parentDataChanged() ;

	private :
		void connectSignals();

		friend class NVBSpecSmoothWidget;

	};

#include <QWidget>

class NVBSpecSmoothWidget : public QWidget {
		Q_OBJECT

	public:
		NVBSpecSmoothWidget(NVBSpecDataSource* source, NVBSpecSmoother * parent);

	signals:
		void widthChanged(NVBPhysValue);
		void subdivChanged(int);
		void homogenityChanged(bool);

	};

#endif
