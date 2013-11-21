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


#include "NVBPages.h"

//------------- NVB3DPage ---------------------

NVB3DPage::~ NVB3DPage() {
	if (colors)
		delete colors;

	if (data)
		free(data);
	}

bool NVB3DPage::setColorModel(NVBContColorModel * colorModel) {
	if (colorModel) {
		emit colorsAboutToBeChanged();

		if (colors) delete colors;

		colors = colorModel;
		connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));
		emit colorsChanged();
		}

	return (bool)colorModel;
	}

bool NVB3DPage::setData(double * newdata) {
	if (newdata) {
		emit dataAboutToBeChanged();

		if (data)
			free(data);

		data = newdata;
		getMinMax();
		emit dataChanged();
		}

	return (bool)newdata;
	}

void NVB3DPage::getMinMax() {
	if (data)
		getMemMinMax<double>(data, _resolution.width()*_resolution.height() , zMin, zMax);
	}

//------------- NVBSpecPage -------------------

NVBSpecPage::~ NVBSpecPage() {
	while (!_data.isEmpty())
		delete _data.takeFirst();

	if (colors) delete colors;
	}


bool NVBSpecPage::setColorModel(NVBDiscrColorModel * colorModel) {
	if (colorModel) {
		emit colorsAboutToBeChanged();

		if (colors) delete colors;

		colors = colorModel;
		connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));
		emit colorsChanged();
		}

	return (bool)colorModel;
	}

bool NVBSpecPage::setData(QList< QwtData * > data) {
	if (data.isEmpty()) return false;

	emit dataAboutToBeChanged();

	while (!_data.isEmpty())
		delete _data.takeFirst();

	_data = data;
	getMinMax();
	emit dataChanged();
	return true;
	}


bool NVBSpecPage::setData(int y, QwtData * data) {
	if (data) {
		emit dataAboutToBeChanged();
		_data.replace(y, data);
		getMinMax();
		emit dataChanged();
		}

	return (bool)data;
	}

void NVBSpecPage::getMinMax() {
	zMin = 1e+120;
	zMax = -1e+120;

	for (int i = _datasize.height() - 1; i >= 0; i--) {
		QwtDoubleRect r = _data.at(i)->boundingRect().normalized();

		if (zMin > r.top()) zMin = r.top();

		if (zMax < r.bottom()) zMax = r.bottom();
		}
	}


