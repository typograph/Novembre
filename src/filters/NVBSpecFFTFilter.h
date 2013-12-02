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
#ifndef NVBSPECFFTFILTER_H
#define NVBSPECFFTFILTER_H

#include "../core/NVBFilterDelegate.h"
#include <QWidget>
#include <QAction>

#include <fftw3.h>

class QCheckBox;
class QComboBox;
class QSlider;

class NVBSpecFFTFilterWidget : public QWidget {
		Q_OBJECT
	private:
		QComboBox *filters;
		QSlider *low, *high;
	public:
		NVBSpecFFTFilterWidget(int n, int f_low, int f_high, int mode);
	signals :
		void modeChanged(int mode);
		void rangeChanged(int low, int high);
	public slots:
		void reset();
		void setFreqRange(int n);
	private slots:
		void lowFreqChanged(int freq);
		void highFreqChanged(int freq);
		void modeIndexChanged(int index);
	};

class NVBSpecFFTFilter : public NVBSpecFilterDelegate {
	Q_OBJECT
	public:
		enum Filter {
			None = 0
		,	LowPass = 1
		,	HighPass = 2
		,	BandPass = 3
		,	BandStop = 4
			};
	private :
		
		NVBSpecDataSource * sprovider;
		Filter mode;
		int f_low, f_high;

		double * fft_data;
		double * fft_cdata;
		fftw_plan plan_direct, plan_inverse;
		
		QList<QwtData*> sdata;
	public  :
		NVBSpecFFTFilter(NVBSpecDataSource * source);
		virtual ~NVBSpecFFTFilter() {;}

		NVB_FORWARD_SPECDIM(sprovider);
		NVB_FORWARD_SPECCOLORS(sprovider);
		NVB_FORWARD_COMMENTS(sprovider);

		virtual inline QList<QPointF> positions() const { return sprovider->positions(); }

		virtual inline QList<QwtData*> getData() const {
// 			if (mode == None)
// 				return sprovider->getData();
// 			else
				return sdata;
			}

		virtual inline QSize datasize() const { return sprovider->datasize(); }

		virtual inline QRectF boundingRect() const {
// 			if (mode == None)
// 				return sprovider->boundingRect();
// 			else
				return NVBSpecDataSource::boundingRect();
			}
		virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

		QWidget * widget();
		static QAction * action();

	public slots:
		void setFilterFreq(int low, int high);
		void setMode(int new_mode);
		virtual void setSource(NVBDataSource * source);

	private slots:
		void clearData();
		void rebuildData();
		void recalculateData();

	private:
		void connectSignals();

	signals:
		void freqRangeReset(int n);
	};

#endif
