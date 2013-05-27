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

#include "NVBPhysScaleDraw.h"
#include "qwt_scale_widget.h"

void NVBPhysScaleDraw::updateMultiplier() {
	QwtScaleDiv d = scaleDiv();
	int ol = NVBPhysValue::getPosMult(d.lowerBound(), -1, 2);
	int oh = NVBPhysValue::getPosMult(d.upperBound(), -1, 2);

	scaleorder = qMax(ol, oh);
	invalidateCache();
	scalewidget->setTitle(scalewidget->title().text().remove(QRegExp(" \\[.*\\]")) + QString(" [%1]").arg(scaledim.unitFromOrder(-scaleorder)));
	scalewidget->update();
	}

NVBPhysScaleDraw::NVBPhysScaleDraw(NVBUnits dim, QwtScaleWidget * widget): QObject(), QwtScaleDraw(), scaledim(dim), scaleorder(0), scalewidget(widget) {
	connect(widget, SIGNAL(scaleDivChanged()), this, SLOT(updateMultiplier()));
	}

QwtText NVBPhysScaleDraw::label(double value) const {
	return QwtScaleDraw::label(round(value * exp10(scaleorder) * 1e+8) / 1e+8);
	}
