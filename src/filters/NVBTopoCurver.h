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
#ifndef NVBTOPOCURVER_H
#define NVBTOPOCURVER_H

#include <QWidget>
#include "../core/NVBFilterDelegate.h"
#include "../core/NVBDiscrColoring.h"

class NVBTopoCurverWidget;

class NVBTopoCurver : public NVBSpecFilterDelegate {
		Q_OBJECT
	private:
		enum Mode { XT, YT };

		NVB3DDataSource * tprovider;
		QList<QPointF> spositions;
		QList<QwtData*> sdata;
		NVBConstDiscrColorModel * scolors;

		Mode mode;

		friend class NVBTopoCurverWidget;

	public :
		NVBTopoCurver(NVB3DDataSource* source);
		~NVBTopoCurver();

		virtual inline double getZMin() const {return tprovider->getZMin();}
		virtual inline double getZMax() const {return tprovider->getZMax();}

		NVB_FORWARD_TOPODIM(tprovider);

		virtual NVBDimension tDim() const;

		virtual const NVBDiscrColorModel * getColorModel() const { return scolors; }

		virtual QList<QPointF> positions() const { return spositions; }

		virtual QList<QwtData*> getData() const { return sdata; }

		virtual QSize datasize() const;

		virtual inline QRectF occupiedArea() const { return tprovider->position(); }

		NVBTopoCurverWidget * widget();

	public slots:
		virtual void setSource(NVBDataSource * source);
		void setMode(NVBTopoCurver::Mode mode);

	private:
		void connectSignals();

	protected slots:
		void generateCurves();
	};

class NVBTopoCurverWidget : public QWidget {
		Q_OBJECT
	public :
		NVBTopoCurverWidget();
		virtual ~NVBTopoCurverWidget() {;}
	private slots:
		void remapMode(int mode);
	signals:
		void curvingModeActivated(NVBTopoCurver::Mode);
	};

#endif
