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
		const NVBDataSet * addDataSet(QString name, NVBUnits dim, FillFunc f, QStringList axes, NVBDataSet::Type type = NVBDataSet::Undefined);

};

#endif
