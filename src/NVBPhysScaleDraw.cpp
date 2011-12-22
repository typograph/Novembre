/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "NVBPhysScaleDraw.h"
#include "qwt_scale_widget.h"

void NVBPhysScaleDraw::updateMultiplier()
{
  QwtScaleDiv d = scaleDiv();
  int ol = NVBPhysValue::getPosMult(d.lowerBound(),-1,2);
  int oh = NVBPhysValue::getPosMult(d.upperBound(),-1,2);

  scaleorder = qMax(ol,oh);
  invalidateCache();
  scalewidget->setTitle(scalewidget->title().text().remove(QRegExp(" \\[.*\\]")) + QString(" [%1]").arg(scaledim.unitFromOrder(-scaleorder)));
  scalewidget->update();
}

NVBPhysScaleDraw::NVBPhysScaleDraw(NVBUnits dim, QwtScaleWidget * widget): QObject(),QwtScaleDraw(),scaledim(dim),scaleorder(0),scalewidget(widget)
{
    connect(widget,SIGNAL(scaleDivChanged()),this,SLOT(updateMultiplier()));
}

QwtText NVBPhysScaleDraw::label(double value) const
{
    return QwtScaleDraw::label(round(value*exp10(scaleorder)*1e+8)/1e+8);
}
