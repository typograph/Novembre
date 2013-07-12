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
#include "NVBSpecMath.h"
#include "NVBAverageColor.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>

#include "../../icons/math.xpm"

class NVBXFwdCPtData : public QwtData {
	public:
		NVBXFwdCPtData(QwtData * data, QwtArray<double> *y) {
			d_d = data;
			d_y = y;
			}
		~NVBXFwdCPtData() {
			if (d_y) delete d_y;
			}

		void setY(QwtArray<double> *y) {
			if (d_y) delete d_y;

			d_y = y;
			};

		virtual QwtData *copy() const {
			return new NVBXFwdCPtData(d_d, new QwtArray<double>(*d_y));
			}

		virtual size_t size() const { return d_y->size();}
		virtual double x(size_t i) const { return d_d->x(i); };
		virtual double y(size_t i) const { return d_y->at(i); };

	private:
		QwtData *d_d;
		QwtArray<double> *d_y;
	};

NVBSpecMathWidget::NVBSpecMathWidget(NVBSpecMath::Mode mode, QWidget * parent): QWidget(parent) {
	setWindowTitle("Mathematics");

	QVBoxLayout * l = new QVBoxLayout(this);
	buttons = new QButtonGroup(this);

	connect(buttons, SIGNAL(buttonClicked(int)), this, SIGNAL(mathModeActivated(int)));

	// No averaging

	QRadioButton * btn = new QRadioButton("Normal", this);
	buttons->addButton(btn);
	buttons->setId(btn, NVBSpecMath::Normal);
	l->addWidget(btn);

	if (mode == NVBSpecMath::Normal) btn->setChecked(true);

	btn = new QRadioButton("1st derivative", this);
	buttons->addButton(btn);
	buttons->setId(btn, NVBSpecMath::FirstDerivative);
	l->addWidget(btn);

	if (mode == NVBSpecMath::FirstDerivative) btn->setChecked(true);

	btn = new QRadioButton("Normalized 1st derivative", this);
	buttons->addButton(btn);
	buttons->setId(btn, NVBSpecMath::NormalizedFirstDerivative);
	l->addWidget(btn);

	if (mode == NVBSpecMath::NormalizedFirstDerivative) btn->setChecked(true);

	l->setSizeConstraint(QLayout::SetFixedSize);

	setLayout(l);
//  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

	}

NVBSpecMathWidget::~ NVBSpecMathWidget() {
	}

NVBSpecMath::NVBSpecMath(NVBSpecDataSource * source):
	NVBSpecFilterDelegate(source), sprovider(0), mode(Normal) {
	connectSignals();
	followSource();
	}

void NVBSpecMath::setMode(int new_mode) {
	setMode((Mode)new_mode);
	}

void NVBSpecMath::setMode(Mode new_mode) {
	if (mode != new_mode) {
		emit dataAboutToBeChanged();
		mode = new_mode;

		switch (mode) {
			case Normal: {
				break;
				}

			case FirstDerivative: {
				clearFData();
				calculate1D();
				break;
				}

			case NormalizedFirstDerivative: {
				clearFData();
				calculate1Dn();
				break;
				}

			default : {
				mode = Normal;
				clearFData();
				}
			}

		emit dataChanged();
		}
	}

QAction * NVBSpecMath::action() {
	return new QAction(QIcon(_math_icon), QString("Math"), 0);
	}

QWidget * NVBSpecMath::widget() {
	NVBSpecMathWidget * widget = new NVBSpecMathWidget(mode);
	connect(widget, SIGNAL(mathModeActivated(int)), SLOT(setMode(int)));
	return widget;
	}

void NVBSpecMath::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	NVBSpecFilterDelegate::setSource(source);
	}

void NVBSpecMath::connectSignals() {
//### emit delegateReset();

	emit dataAboutToBeChanged();

	sprovider = (NVBSpecDataSource*)provider;

	mode = Normal;

	emit dataChanged();

	connect(provider, SIGNAL(dataAdjusted()), SLOT(parentDataAdjusted()));
	connect(provider, SIGNAL(dataAboutToBeChanged()), SLOT(parentDataAboutToBeChanged()));
	connect(provider, SIGNAL(dataChanged()), SLOT(parentDataChanged()));

	NVB_FORWARD_COLORSIGNALS(sprovider);

	}


void NVBSpecMath::calculate1D() {
	if (!sprovider) return;

	QList<QwtData*> sdata = sprovider->getData();

	double * dxc, * dyc; // caches;
	QwtArray<double> *ys;

	foreach(QwtData * curve, sprovider->getData()) {
		dxc = (double*)malloc(curve->size() * sizeof(double));
		dyc = (double*)malloc(curve->size() * sizeof(double));

		ys = new QwtArray<double>(curve->size());

		for (int i = curve->size() - 2; i >= 0 ; i--) {
			dxc[i] = curve->x(i + 1) - curve->x(i);
			dyc[i] = curve->y(i + 1) - curve->y(i);
			}

		(*ys)[0] = dyc[0] / dxc[0];
		(*ys)[curve->size() - 1] = dyc[curve->size() - 2] / dxc[curve->size() - 2];

		for (int i = curve->size() - 2; i > 0 ; i--)
			(*ys)[i] = (dyc[i] * dxc[i - 1] * dxc[i - 1] + dyc[i - 1] * dxc[i] * dxc[i]) / (dxc[i] * dxc[i - 1] * (curve->x(i + 1) - curve->x(i - 1)));

		fdata << new NVBXFwdCPtData(curve, ys);

		free(dxc);
		free(dyc);
		}

	}

void NVBSpecMath::calculate1Dn() {
	if (!sprovider) return;

	QList<QwtData*> sdata = sprovider->getData();

	double * dxc, * dyc; // caches;
	QwtArray<double> *ys;

	foreach(QwtData * curve, sprovider->getData()) {
		dxc = (double*)malloc(curve->size() * sizeof(double));
		dyc = (double*)malloc(curve->size() * sizeof(double));

		ys = new QwtArray<double>(curve->size());

		for (int i = curve->size() - 2; i >= 0 ; i--) {
			dxc[i] = curve->x(i + 1) - curve->x(i);
			dyc[i] = curve->y(i + 1) - curve->y(i);
			}

		if (curve->y(0) == 0)
			(*ys)[0] = 0;
		else
			(*ys)[0] = dyc[0] * curve->x(0) / dxc[0] / curve->y(0);

		int csm = curve->size() - 1;

		if (curve->y(csm) == 0)
			(*ys)[csm] = 0;
		else
			(*ys)[csm] = dyc[csm - 1] * curve->x(csm) / dxc[csm - 1] / curve->y(csm);

		for (int i = curve->size() - 2; i > 0 ; i--) {
			if (curve->y(i) == 0)
				(*ys)[i] = (*ys)[i + 1];
			else
				(*ys)[i] = (dyc[i] * dxc[i - 1] * dxc[i - 1] + dyc[i - 1] * dxc[i] * dxc[i]) * curve->x(i) / (dxc[i] * dxc[i - 1] * (curve->x(i + 1) - curve->x(i - 1)) * curve->y(i));
			}

		fdata << new NVBXFwdCPtData(curve, ys);

		free(dxc);
		free(dyc);
		}

	}

void NVBSpecMath::parentDataChanged() {
	switch (mode) {
		case FirstDerivative : {
			/*      emit dataAboutToBeChanged();
			      clearFData();*/
			calculate1D();
			break;
			}

		case NormalizedFirstDerivative : {
			/*      emit dataAboutToBeChanged();
			      freeFData();*/
			calculate1Dn();
			break;
			}

		case Normal:
		default :
			break;
		}

	emit dataChanged();
	}

void NVBSpecMath::parentDataAboutToBeChanged() {
	emit dataAboutToBeChanged();
	clearFData();
	}

void NVBSpecMath::parentDataAdjusted() {
	switch (mode) {
		case FirstDerivative : {
			emit dataAboutToBeChanged();
			clearFData();
			calculate1D();
			emit dataChanged();
			break;
			}

		case NormalizedFirstDerivative : {
			emit dataAboutToBeChanged();
			clearFData();
			calculate1Dn();
			emit dataChanged();
			break;
			}

		case Normal :
		default : {
			emit dataAdjusted();
			break;
			}
		}
	}
