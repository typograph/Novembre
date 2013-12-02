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
#include "NVBSpecFFTFilter.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QIcon>

#include <qwt_data.h>

#include "../../icons/fft.xpm"

class XFwdData : public QwtData {
	private:
		const QwtData * xprovider;
		const double * yprovider;
	public:
		XFwdData(const QwtData * other, const double * ydata)
			: QwtData()
			, xprovider(other)
			, yprovider(ydata)
			{}
	
	virtual QwtData * copy () const { return new XFwdData(xprovider, yprovider); }
	virtual size_t size () const { return xprovider->size(); }
	virtual double x (size_t i) const { return xprovider->x(i); }
	virtual double y (size_t i) const { return yprovider[i]; }
// 	virtual QwtDoubleRect boundingRect () const ;
};

NVBSpecFFTFilterWidget::NVBSpecFFTFilterWidget(int n, int f_low, int f_high, int mode): QWidget() {

	setLayout(new QVBoxLayout(this));
	QHBoxLayout * l = new QHBoxLayout();
	layout()->addItem(l);
	l->addWidget(new QLabel("Filter",this));
	filters = new QComboBox(this);
	filters->addItems(QStringList() << "None" << "Lowpass" << "Highpass" << "Bandpass" << "Bandstop");
	connect(filters, SIGNAL(currentIndexChanged(int)), SLOT(modeIndexChanged(int)));
	filters->setCurrentIndex(mode);
	l->addWidget(filters);
	
	l = new QHBoxLayout();
	layout()->addItem(l);
	l->addWidget(new QLabel("Low",this));
	low = new QSlider(Qt::Horizontal,this);
	l->addWidget(low);
	connect(low,SIGNAL(valueChanged(int)),SLOT(lowFreqChanged(int)));
	
	l = new QHBoxLayout();
	layout()->addItem(l);
	l->addWidget(new QLabel("High",this));
	high = new QSlider(Qt::Horizontal,this);
	l->addWidget(high);
	connect(high,SIGNAL(valueChanged(int)),SLOT(highFreqChanged(int)));
	
	setFreqRange(n);
	low->setValue(f_low);
	high->setValue(f_high);
	
	}

void NVBSpecFFTFilterWidget::setFreqRange(int n) {
	low->setMaximum(n/2-1);
	high->setMaximum(n/2-1);
	}
	
void NVBSpecFFTFilterWidget::modeIndexChanged(int i) {
	NVBSpecFFTFilter::Filter m = (NVBSpecFFTFilter::Filter) i;
	low->setEnabled(m != NVBSpecFFTFilter::HighPass && m != NVBSpecFFTFilter::None);
	high->setEnabled(m != NVBSpecFFTFilter::LowPass && m != NVBSpecFFTFilter::None);
	emit modeChanged(i);
	}
	
void NVBSpecFFTFilterWidget::lowFreqChanged(int freq) {
	if (high->value() < freq)
		high->setValue(freq);
	else
		emit rangeChanged(freq, high->value());
	}

void NVBSpecFFTFilterWidget::highFreqChanged(int freq) {
	if (low->value() > freq)
		low->setValue(freq);
	else
		emit rangeChanged(low->value(), freq);
	}

	
QWidget * NVBSpecFFTFilter::widget() {
	NVBSpecFFTFilterWidget * w = new NVBSpecFFTFilterWidget(sprovider->datasize().width(), f_low, f_high, mode);
	connect(w, SIGNAL(modeChanged(int)), SLOT(setMode(int)));
	connect(w, SIGNAL(rangeChanged(int,int)), SLOT(setFilterFreq(int,int)));
	connect(this, SIGNAL(freqRangeReset(int)), w, SLOT(setFreqRange(int)));
	return w;
	}


NVBSpecFFTFilter::NVBSpecFFTFilter(NVBSpecDataSource * source)
	: NVBSpecFilterDelegate(source)
	, sprovider(0)
	, mode(None)
	, f_low(-1)
	, f_high(-1)
	, fft_data(0)
	, fft_cdata(0) {

	connectSignals();
	followSource();

	}
	
void NVBSpecFFTFilter::setMode(int i) {
	Filter new_mode = (Filter) i;
	if (mode != new_mode) {
		bool changed = mode == None || new_mode == None;
		if (changed) emit dataAboutToBeChanged();
		mode = new_mode;
		if (mode !=  None) recalculateData();
		if (changed) emit dataChanged();
		}
	}

void NVBSpecFFTFilter::setSource(NVBDataSource * source) {
	if (provider) provider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	clearData();

	NVBSpecFilterDelegate::setSource(source);

	}

void NVBSpecFFTFilter::connectSignals() {
	sprovider = (NVBSpecDataSource*)provider;

	connect(provider, SIGNAL(dataAboutToBeChanged()), SLOT(clearData()));
	connect(provider, SIGNAL(dataAdjusted()), SLOT(recalculateData()));
	connect(provider, SIGNAL(dataChanged()), SLOT(rebuildData()));
	
	rebuildData();

	}

void NVBSpecFFTFilter::recalculateData() {
	QSize s = sprovider->datasize();
	int n = s.width();
	
	memcpy(fft_data, fft_cdata, sizeof(double)*s.width()*s.height());
	
	switch(mode) {
		case LowPass:
			for (int i=0; i<s.height(); i++)
				memset(fft_data + i*n + f_low + 1,0,sizeof(double) * (n - 2*f_low - 1));
			break;
		case HighPass:
			for (int i=0; i<s.height(); i++) {
				memset(fft_data + i*n,0,sizeof(double) * (f_high + 1));
				memset(fft_data + i*n + n - f_high + 1,0,sizeof(double) * (f_high));
				}
			break;
		case BandPass:
			for (int i=0; i<s.height(); i++) {
				// High pass @ f_low
				memset(fft_data + i*n,0,sizeof(double) * (f_low + 1));
				memset(fft_data + i*n + n - f_low + 1,0,sizeof(double) * (f_low));
				// Low pass @ f_high
				memset(fft_data + i*n + f_high + 1,0,sizeof(double) * (n - 2*f_high - 1));
				}
			break;
		case BandStop:
			for (int i=0; i<s.height(); i++) {
				memset(fft_data + i*n + f_low + 1,0,sizeof(double) * (f_high - f_low));
				memset(fft_data + i*n + n - f_high + 1,0,sizeof(double) * (f_high - f_low));
				}
			break;
		default:
			break;
			
	}
	
	fftw_execute(plan_inverse);
	
	emit dataAdjusted();
}
	
void NVBSpecFFTFilter::rebuildData() {
	
	QSize s = sprovider->datasize();
	
	int w = s.width();
	
	if (f_low < 0 || f_low > w/2-1) f_low = w/4;
	if (f_high < 0 || f_high > w/2-1) f_high = w/4;
	
	
	fftw_r2r_kind k = FFTW_R2HC;
	
	fft_cdata = (double*) fftw_malloc(sizeof(double) * s.width() * s.height());
	plan_direct = fftw_plan_many_r2r(1, &w, s.height(),
                                      fft_cdata, 0, 1, s.width(),
                                      fft_cdata, 0, 1, s.width(),
                                      &k, FFTW_ESTIMATE);

	k = FFTW_HC2R;
	
	fft_data = (double*) fftw_malloc(sizeof(double) * s.width() * s.height());
	plan_inverse = fftw_plan_many_r2r(1, &w, s.height(),
                                      fft_data, 0, 1, s.width(),
                                      fft_data, 0, 1, s.width(),
                                      &k, FFTW_MEASURE);
	
	for (int i=0; i<s.height(); i++) {
		// Create curve
		const QwtData * d = sprovider->getData().at(i);
		sdata << new XFwdData(d,fft_data + i*s.width());
		for (int j=0; j < w; j++)
			fft_cdata[i*s.width() + j] = d->y(j);
		}

	
	
	// Calculate direct FFT
	fftw_execute(plan_direct);
	
	// Normalize FFT
	int n = w*s.height();
	for (int i=0;i<n;i++)
		fft_cdata[i] /= w;
	
	// Calculate inverse FFT
	recalculateData();
	
	emit freqRangeReset(w);
	emit dataChanged();
	}

void NVBSpecFFTFilter::clearData() {
	emit dataAboutToBeChanged();

	while (!sdata.isEmpty()) delete sdata.takeFirst();

	if (fft_data) {
		fftw_destroy_plan(plan_inverse);
		fftw_free(fft_data);
		fft_data = 0;
		}
		
	if (fft_cdata) {
		fftw_destroy_plan(plan_direct);
		fftw_free(fft_cdata);
		fft_cdata = 0;
		}
	}

QAction * NVBSpecFFTFilter::action() {
	return new QAction(QIcon(_fft_apply), "1D FFT filter", 0);
	}
	
void NVBSpecFFTFilter::setFilterFreq(int low, int high) {
	if (low == f_low && mode == LowPass) { f_high = high; return; }
	if (high == f_high && mode == HighPass) { f_low = low; return; }
	if (low != f_low || high != f_high) {
		f_low = low;
		f_high = high;
		if (mode != None)
			recalculateData();
		}
	}
