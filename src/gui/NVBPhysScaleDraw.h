//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBPHYSSCALEDRAW_H
#define NVBPHYSSCALEDRAW_H

#include <QtCore/QObject>
#include <qwt_scale_draw.h>
#include "NVBUnits.h"

class QwtScaleWidget;

class NVBPhysScaleDraw : public QObject, public QwtScaleDraw {
		Q_OBJECT
	protected:
		NVBUnits scaledim;
		int scaleorder;
		QwtScaleWidget * scalewidget;

	protected slots:
		virtual void updateMultiplier();

	public:
		NVBPhysScaleDraw(NVBUnits dim, QwtScaleWidget * widget);
		virtual ~NVBPhysScaleDraw() {;}
		virtual QwtText label( double value ) const;
	};

#endif // NVBPHYSSCALEDRAW_H
