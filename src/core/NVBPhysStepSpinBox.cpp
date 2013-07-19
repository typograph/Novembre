//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#include "NVBPhysStepSpinBox.h"
#include <QtGui/QLineEdit>

QString NVBPhysStepSpinBox::textFromValue(int value) const {
	return NVBPhysValue(dataScaler.scale(value), dim).toString(0);
	}

NVBPhysStepSpinBox::NVBPhysStepSpinBox( NVBUnits dimension, double start, double end, int steps, QWidget * parent) : QSpinBox(parent), dim(dimension) {
	lineEdit()->setReadOnly(true);

	setMinimumWidth(fontMetrics().boundingRect("0.000 ZM").width());
	setLimits(start, end, steps);
	setValue(0);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(emitPhysValue(int)));
	}

void NVBPhysStepSpinBox::setLimits(double start, double end, int steps) {
	dataScaler = NVBValueScaler<int, double>(0, steps - 1, start, end);
	setMinimum(0);
	setMaximum(steps - 1); // == 99.99999(9)
	}

