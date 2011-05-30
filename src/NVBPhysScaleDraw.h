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


#ifndef NVBPHYSSCALEDRAW_H
#define NVBPHYSSCALEDRAW_H

#include <QtCore/QObject>
#include <qwt6/qwt_scale_draw.h>
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
