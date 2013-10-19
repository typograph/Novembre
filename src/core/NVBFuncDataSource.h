//
// Copyright 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBFUNCDATASOURCE_H
#define NVBFUNCDATASOURCE_H

#include "NVBDataSource.h"
#include <QtCore/QMap>
#include "NVBDataCore.h"

class NVBFuncDataSource : public NVBConstructableDataSource {
	private:
		QMap<QString, axisindex_t> anameix;
	public:
		NVBFuncDataSource();
    virtual ~NVBFuncDataSource();

		void addAxis(QString name, axissize_t resolution, NVBUnits dim, double start = 0, double end = 0);
//		void addFuncMap(NVBAxisMap* map, QStringList axes);
		void addMap(NVBAxisMap* map, QStringList axes);
		const NVBDataSet * addDataSet(QString name, NVBUnits dim, FillFunc f, QStringList axes, NVBDataSet::Type type = NVBDataSet::Undefined);

};

#endif
