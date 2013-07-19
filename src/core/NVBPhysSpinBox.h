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

#ifndef NVBPHYSSPINBOX_H
#define NVBPHYSSPINBOX_H

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QValidator>
#include "NVBUnits.h"

class NVBPhysSpinBox : public QDoubleSpinBox {
		Q_OBJECT
	private:
// I know this is ugly, but Trolltech doesn't leave me any other choice
// I'll change it when they change the interface to QAbstractSpinBox;
		mutable NVBUnits dim;
		static void normalizeValDim(double & value, NVBUnits & dim);

	private slots:
		void emitPhysValue(double v) { emit valueChanged(NVBPhysValue(v, dim)); }

	protected:

		virtual StepEnabled stepEnabled () const
			{ return QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled ;}

		void stepBy ( int steps );

		virtual QString textFromValue ( double value ) const;
		virtual double valueFromText ( const QString & text ) const;

		virtual QValidator::State validate( QString & input, int & pos ) const;

	public:
		NVBPhysSpinBox( NVBUnits dimension, QWidget * parent = 0 );
		virtual ~NVBPhysSpinBox() {;}

		NVBPhysValue physValue() const { return NVBPhysValue(value(), dim); }

	public slots:
		void setPhysValue(const NVBPhysValue & value);

	signals:
//  void valueChanged(double value);
		void valueChanged(NVBPhysValue value);
	};


#endif
