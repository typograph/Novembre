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


#ifndef NVBPOSLABEL_H
#define NVBPOSLABEL_H

#include <QtGui/QLabel>
#include "NVBUnits.h"

class QAbstractSlider;
class NVBAxisPhysMap;

class NVBPosLabel : public QLabel {
	Q_OBJECT;
	private:
		NVBAxisPhysMap * vmap;
	public:
		explicit NVBPosLabel(QAbstractSlider * slider, NVBAxisPhysMap * map, QWidget* parent = 0);
	public slots:
		void setLabelFor(int);
};

#endif // NVBPOSLABEL_H
