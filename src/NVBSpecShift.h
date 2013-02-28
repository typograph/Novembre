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
#ifndef NVBSPECSHIFT_H
#define NVBSPECSHIFT_H

#include "NVBFilterDelegate.h"
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QWidget>

class QButtonGroup;
class NVBSpecShiftWidget;

class NVBSpecShift : public NVBSpecFilterDelegate {
		Q_OBJECT
	public:

	private:
		NVBSpecDataSource * sprovider;

		QList<QwtData*> fdata;

		double xshift, yshift;

	private slots:

		void buildFData();

		void clearFData() {
			while (!fdata.isEmpty())
				delete fdata.takeLast();
			}

		void parentDataAboutToBeChanged();
		void parentDataChanged();

	public:

		NVBSpecShift(NVBSpecDataSource* source);
		virtual ~NVBSpecShift() { ; }

		virtual inline double getZMin() const {return boundingRect().bottom();}
		virtual inline double getZMax() const {return boundingRect().top();}


		NVB_FORWARD_SPECDIM(sprovider);
		NVB_FORWARD_SPECCOLORS(sprovider);

		virtual inline QList<QPointF> positions() const { return sprovider->positions(); }

		virtual inline QList<QwtData*> getData() const { return fdata; }

		virtual inline QSize datasize() const { return sprovider->datasize(); }

		virtual QRectF boundingRect() const { return sprovider->boundingRect().translated(-xshift, -yshift);}

		virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

		static QAction * action();
		QWidget * widget();

	public slots:

		virtual void setSource(NVBDataSource * source);

		void moveYtoCenterZero();
		void moveXtoCenterZero();
		void addBias();
		void subtractBias();
		void resetShifts();

	private :
		void connectSignals();
	};

class NVBSpecShiftWidget : public QWidget {
		Q_OBJECT
	private:
	private slots:
	public:
		NVBSpecShiftWidget(QWidget * parent = 0);
		virtual ~NVBSpecShiftWidget();
	signals:
		void delegateReset();
		void smashY();
		void smashX();
		void subtractBias();
		void addBias();
	};

#endif

