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


#ifndef NVBDISCRCOLORMODEL_H
#define NVBDISCRCOLORMODEL_H

#include <QObject>
#include <QList>
#include <QColor>

namespace NVBColoring {

class NVBDiscrColorModel : public QObject {
		Q_OBJECT
	public:
		NVBDiscrColorModel() {;}
		virtual ~NVBDiscrColorModel() {;}

		virtual QColor colorize(int) const = 0;
		virtual QList<QColor> colorize(int start, int end) const {
			if (start > end) return QList<QColor>();

			QList<QColor> list;

			for (int i = start; i <= end; i++)
				list.append(colorize(i));

			return list;
			}

//    virtual QString colorName(uint index) const = 0;
	signals:
		void adjusted();
	};

}

#endif
