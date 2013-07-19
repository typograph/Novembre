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

#ifndef NVBMUTABLEGRADIENTS_H
#define NVBMUTABLEGRADIENTS_H

#include "NVBColorWidgets.h"
#include "NVBGradientMenu.h"

class NVBMutableGradientAction : public NVBGradientAction {
		Q_OBJECT
	protected:
		NVBColorSelectMenu * colorMenu;
	public:
		NVBMutableGradientAction(NVBColorMap * gradient, QObject * parent );
		virtual ~NVBMutableGradientAction() {;}

	public slots:
		virtual void setColor(QColor color);
	};

class NVBRGBRampColorMap;

class NVBBlackToColorGradientAction : public NVBMutableGradientAction {
		Q_OBJECT
	private:
		NVBRGBRampColorMap * pcolors;
	public:
		NVBBlackToColorGradientAction(QObject * parent);
		virtual ~NVBBlackToColorGradientAction() {;}
	public slots:
		virtual void setColor(QColor color);
	};

class NVBGrayStepColorMap;

class NVBBlackToColorToWhiteGradientAction : public NVBMutableGradientAction {
		Q_OBJECT
	private:
		NVBGrayStepColorMap * red;
		NVBGrayStepColorMap * green;
		NVBGrayStepColorMap * blue;
	public:
		NVBBlackToColorToWhiteGradientAction(QObject * parent);
		virtual ~NVBBlackToColorToWhiteGradientAction() {;}
	public slots:
		virtual void setColor(QColor color);
	};

#endif // NVBMUTABLEGRADIENTS_H
