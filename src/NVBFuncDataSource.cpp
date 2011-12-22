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
	if (start == end) end = resolution;
	NVBConstructableDataSource::addAxisMap( new NVBAxisPhysMap(start,(end-start)/(resolution-1),dim), QVector<axisindex_t>() << (nAxes()-1));
	anameix.insert(name,axs.count()-1);
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


