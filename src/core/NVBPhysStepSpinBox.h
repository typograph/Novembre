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


#ifndef NVBPHYSSPINBOX_H
#define NVBPHYSSPINBOX_H

#include <QSpinBox>
#include <QValidator>
#include "NVBDimension.h"
#include "dimension.h"

class NVBPhysStepSpinBox : public QSpinBox {
		Q_OBJECT
	private:
		mutable NVBDimension dim;
		scaler<int, double> dataScaler;

	private slots:
		void emitPhysValue(int v) { emit valueChanged(NVBPhysValue(dataScaler.scale(v), dim)); }

	protected:
		virtual QString textFromValue(int value) const;
	public:
		NVBPhysStepSpinBox(NVBDimension dimension, double start, double end, int steps, QWidget * parent = 0);
		virtual ~NVBPhysStepSpinBox() {;}

		NVBPhysValue physValue() const { return NVBPhysValue(value(), dim); }

		void setLimits(double start, double end, int steps);

	signals:
		void valueChanged(NVBPhysValue value);
	};


#endif
