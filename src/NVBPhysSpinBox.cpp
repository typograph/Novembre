//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include "NVBPhysSpinBox.h"

QString NVBPhysSpinBox::textFromValue(double value) const {
	return NVBPhysValue(value, dim).toString(-1, 1, decimals());
	}

double NVBPhysSpinBox::valueFromText(const QString & text) const {
	double tvalue = NVBPhysValue(text).getValue(dim);
	normalizeValDim(tvalue, dim);
	return tvalue;
	}

NVBPhysSpinBox::NVBPhysSpinBox(NVBDimension dimension, QWidget * parent) : QDoubleSpinBox(parent), dim(dimension) {
	setMinimumWidth(fontMetrics().boundingRect("0.000 ZM").width());
	setMinimum(-100.);
	setMaximum(100.); // == 99.99999(9)
	setValue(0.);
	setDecimals(3);
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(emitPhysValue(double)));
	}

void NVBPhysSpinBox::stepBy(int steps) {
	double tvalue = value();

	if (tvalue == 0)
		tvalue = singleStep() * steps;
	else
		tvalue *= 1 + steps * 0.1;

//    renderValue();

	normalizeValDim(tvalue, dim);

	setValue(tvalue);
	}

void NVBPhysSpinBox::normalizeValDim(double & value, NVBDimension & dim) {
	if (value >= 100.) {
		value /= 1000;
		dim += 3;
		}
	else if (value < 0.1) {
		value *= 1000;
		dim -= 3;
		}
	}

QValidator::State NVBPhysSpinBox::validate(QString & input, int &) const {
//  since NVBPhysValue(QString) doesn't throw, we have to do it again

	if (QRegExp("[0-9\\-\\.]+ .+").exactMatch(input)) {
		NVBPhysValue tvalue(input);

		if (!(tvalue.getDimension() != dim))
			return QValidator::Acceptable;
		}

	return QValidator::Intermediate;
	}


void NVBPhysSpinBox::setPhysValue(const NVBPhysValue & value) {
	if (!(value.getDimension() != dim)) {
		double tvalue = value.getValue(dim);

		while (tvalue > 100. || tvalue < 0.1)
			normalizeValDim(tvalue, dim);

		setValue(tvalue);
		}
	}
