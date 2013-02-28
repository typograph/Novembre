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


#include "NVBDiscrColoring.h"
#include "NVBLogger.h"

NVBColoring::NVBRandomDiscrColorModel::NVBRandomDiscrColorModel(int ncolors): NVBDiscrColorModel(), nc(ncolors) {
	if (nc < 0) nc = 0;
	qsrand(nc);
	resize(nc);
	}

void NVBColoring::NVBRandomDiscrColorModel::resize(int ncolors) {
	for (int i = colors.length(); i < ncolors; i++)
		colors.append(newcolor());
}

QColor NVBColoring::NVBRandomDiscrColorModel::colorize(int index) const {
	if (index < 0) {
		NVBOutputPMsg("Negative color queried for.");
		return Qt::black;
		}
	else if (index >= nc) {
		NVBOutputPMsg("Expanding color base");

		for (int i = nc; i <= index; i++)
			colors.append(newcolor());
		}

	return colors.at(index);
	}

QColor NVBColoring::NVBRandomDiscrColorModel::newcolor() const {
	return QColor::fromRgb(qrand() % 255, qrand() % 255, qrand() % 255);
	}
