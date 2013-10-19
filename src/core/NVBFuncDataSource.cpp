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


#include "NVBFuncDataSource.h"
#include "NVBUnits.h"
#include "NVBAxisMaps.h"

NVBFuncDataSource::NVBFuncDataSource() : NVBConstructableDataSource(0)
{

}

NVBFuncDataSource::~NVBFuncDataSource()
{

}

void NVBFuncDataSource::addAxis(QString name, axissize_t resolution, NVBUnits dim, double start, double end)
{
	NVBConstructableDataSource::addAxis(name,resolution);
	anameix.insert(name,axs.count()-1);
	
	if (!dim.isValid()) return;
	
	if (start == end) end = resolution;
	NVBConstructableDataSource::addAxisMap( new NVBAxisPhysMap(start,(end-start)/(resolution-1),dim), QVector<axisindex_t>() << (nAxes()-1));
}

void NVBFuncDataSource::addMap(NVBAxisMap* map, QStringList axes)
{
	QVector<axisindex_t> ixs;
	foreach(QString aname, axes) {
		if (anameix.contains(aname)) {
			ixs << anameix.value(aname);
			}
		else
			return;
		}
	NVBConstructableDataSource::addAxisMap(map,ixs);
}

const NVBDataSet * NVBFuncDataSource::addDataSet(QString name, NVBUnits dim, FillFunc f, QStringList axes, NVBDataSet::Type type )
{
	QVector<axisindex_t> ixs;
	QVector<axissize_t> szs;
	foreach(QString aname, axes) {
		if (anameix.contains(aname)) {
			ixs << anameix.value(aname);
			szs << axs.at(ixs.last()).length();
			}
		else
			return 0;
		}

	axissize_t sza = prod(szs.count(),szs.constData());
		
	double * data = (double*) malloc(sza * sizeof(double));

	fillNArray(data,szs.count(),szs.constData(),f);
	
	NVBConstructableDataSource::addDataSet(name,data,dim,NVBDataComments(),ixs,type);

	return dsets.last();
}
