//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#include "NVBCurveModel.h"

int NVBCurveModel::rowCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);

	if (!provider) return 0;

	return provider->datasize().height();
	}

QVariant NVBCurveModel::data(const QModelIndex & index, int role) const {
	if (!provider || !index.isValid()) return QVariant();

	switch (role) {
		case Qt::DisplayRole :
			return QString("Curve %1").arg(index.row());

		case Qt::DecorationRole :
			return QColor(provider->getColorModel()->colorize(index.row()));

		default :
			return QVariant();
		}
	}

NVBCurveModel::NVBCurveModel(NVBSpecDataSource * source): QAbstractListModel(), provider(source) {

	if (!provider) {
		NVBOutputError("NULL page supplied");
		return;
		}

	connect(provider, SIGNAL(dataChanged()), SLOT(resetModel()));
	connect(provider, SIGNAL(colorsChanged()), SLOT(updateModel()));
	connect(provider, SIGNAL(colorsAdjusted()), SLOT(updateModel()));
//  connect(provider,SIGNAL(dataChanged()),SLOT());

	}

void NVBCurveModel::updateModel() {
	emit dataChanged(index(0), index(rowCount()));
	}

void NVBCurveModel::resetModel() {
	reset();
	}
