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
#include "NVBSpecSmooth.h"
#include <QAction>
#include "../../icons/smooth.xpm"

QwtArray< double > NVBSpecSmoother::xPoints() {
	if (equalSpacing) {
		const QwtData * muster = sprovider->getData().first();
		double x0 = muster->x(0);
		int count = muster->size();
		double xN = muster->x(count - 1);

		if (ptsFactor > 0)
			count += (count - 1) * ptsFactor;
		else
			count = (count + 1) / (1 - ptsFactor);

		double spacing = (xN - x0) / (count - 1);
		QwtArray<double> points(count, x0);

		for (int i = 0; i < count; i++)
			points[i] += i * spacing;

		return points;
		}
	else {
		const QwtData * muster = sprovider->getData().first();
		int count = muster->size();
		QwtArray<double> points;

		if (ptsFactor < 0) ptsFactor *= -1;

		points.reserve((count + ptsFactor) / (ptsFactor + 1));

		for (int i = 0; i < count; ++i += ptsFactor)
			points << muster->x(i);

		return points;
		}
	}

void NVBSpecSmoother::connectSignals() {
	emit dataAboutToBeChanged();

	sprovider = (NVBSpecDataSource*)provider;

	calculate();

	emit dataChanged();

	connect(provider, SIGNAL(dataAdjusted()), SLOT(calculate()));
	connect(provider, SIGNAL(dataChanged()), SLOT(calculate()));

	NVB_FORWARD_COLORSIGNALS(sprovider);
	}

void NVBSpecSmoother::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	NVBSpecFilterDelegate::setSource(source);
	}

void NVBSpecSmoother::calculate() {
	clearFData();

	tdata = xPoints();
	foreach(QwtData * curve, sprovider->getData()) {
		QwtArray<double> ydata;
		ydata.reserve(tdata.size());
		foreach(double t, tdata)
		ydata << wrap(t, curve);
		fdata << new QwtArrayData(tdata, ydata);
		}

	}


void NVBSpecSmoother::setEqualSpacing(bool equal) {
	if (equalSpacing != equal) {
		equalSpacing = equal;
		emit dataAboutToBeChanged();
		calculate();
		emit dataChanged();
		}
	}

void NVBSpecSmoother::setSubdivisions(int div) {
	if (ptsFactor != div) {
		ptsFactor = div;
		emit dataAboutToBeChanged();
		calculate();
		emit dataChanged();
		}
	}

void NVBSpecSmoother::setWidth(const NVBPhysValue & width) {
	env_width = width.getValue(sprovider->tDim());
	emit dataAboutToBeChanged();
	calculate();
	emit dataChanged();
	}

QSize NVBSpecSmoother::datasize() const {
	return QSize(tdata.size(), fdata.size());
	}

// FIXME the class is temporarily optimized for circular averaging.
// Change parameters to normal smooth (i.e. false,0 ) for normal smoothing
// as soon as scripting is implemented

NVBSpecSmoother::NVBSpecSmoother(NVBSpecDataSource * data): NVBSpecFilterDelegate(data) {
	if (data->datasize().width() < 200) {
		equalSpacing = false;
		ptsFactor = 0;
		}
	else {
		equalSpacing = true;
		ptsFactor = (int) - ceil((data->datasize().width() - 100) / 100);
		}

	env_width = data->boundingRect().width() / qMin(data->datasize().width(), 200);
	connectSignals();
	followSource();
	}

NVBSpecSmoother::~ NVBSpecSmoother() {
	clearFData();
	}

#include <QAction>

QAction * NVBSpecSmoother::action() {
	return new QAction(QIcon(_smooth_icon), QString("Smooth data"), 0);
	}

QWidget * NVBSpecSmoother::widget() {
	NVBSpecSmoothWidget * widget = new NVBSpecSmoothWidget(sprovider, this);
	connect(widget, SIGNAL(widthChanged(NVBPhysValue)), SLOT(setWidth(const NVBPhysValue &)));
	connect(widget, SIGNAL(subdivChanged(int)), SLOT(setSubdivisions(int)));
	connect(widget, SIGNAL(homogenityChanged(bool)), SLOT(setEqualSpacing(bool)));
	return widget;
	}

double NVBSpecSmoother::wrap(double center, const QwtData * data) {
//   QwtArray<double> edata;
//   edata.resize(data.size());

	double c = -2 / pow(env_width, 2);
	double n = 0;

	double y = 0;

	for (int i = data->size() - 1; i >= 0; i--) {
		double xx = data->x(i) - center;

		if (fabs(xx) < env_width) {
			double env = exp(c * pow(xx, 2));
			n += env;
			y += env * data->y(i);
			}
		}

	return y / n;
	}

double NVBSpecSmoother::envelope(double /*x*/) {
	return 0;
	}

#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include "../core/NVBPhysSpinBox.h"


NVBSpecSmoothWidget::NVBSpecSmoothWidget(NVBSpecDataSource* source, NVBSpecSmoother * parent): QWidget() {
	setWindowTitle("Smoothing");

	QVBoxLayout * l = new QVBoxLayout(this);

	QHBoxLayout * l2 = new QHBoxLayout();

	l2->addWidget(new QLabel("Envelop width", this));

	NVBPhysSpinBox * wdth = new NVBPhysSpinBox(source->tDim(), this);
	wdth->setPhysValue(NVBPhysValue(parent->env_width, source->tDim()));
	connect(wdth, SIGNAL(valueChanged(const NVBPhysValue &)), this, SIGNAL(widthChanged(NVBPhysValue)));
	l2->addWidget(wdth);

	l->addLayout(l2);

	QCheckBox * eqbox = new QCheckBox("Space points equally", this);
	eqbox->setChecked(parent->equalSpacing);
	connect(eqbox, SIGNAL(toggled(bool)), this, SIGNAL(homogenityChanged(bool)));
	l->addWidget(eqbox);

	QHBoxLayout * l3 = new QHBoxLayout();

	l3->addWidget(new QLabel("Subdivisions", this));

	QSpinBox * ptsdiv = new QSpinBox(this);
	ptsdiv->setRange(-source->datasize().width(), 100);
	ptsdiv->setValue(parent->ptsFactor);
	connect(ptsdiv, SIGNAL(valueChanged(int)), this, SIGNAL(subdivChanged(int)));
	l3->addWidget(ptsdiv);

	l->addLayout(l3);

	}

