#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBDataTransforms.h"

NVBDataSet::NVBDataSet(NVBDataSource * parent,
						QString name,
						double * data,
						NVBDimension dimension,
						QVector<axisindex_t> axes,
						NVBColorMap * colormap)
	:	QObject()
	,	p(parent)
	,	n(name)
	,	d(data)
	,	dim(dimension)
	,	as(axes)
	, clr(colormap)
	, t(tp)
	{;}

NVBDataSet::~NVBDataSet() {
	free(d);
	}

QVector<axissize_t> NVBDataSet::sizes() const {
	if (asizes.isEmpty()) {
		foreach (axisindex_t i, as)
			asizes << p->axis(i).length();
		}
	return asizes;
	}

axissize_t NVBDataSet::sizeAt(int i) const {
	return p->axis(as.at(i)).length();
	}

double NVBDataSet::min() const { return NVBMaxMinTransform::findMinimum(this); }
double NVBDataSet::max() const { return NVBMaxMinTransform::findMaximum(this); }

inline NVBDataComments NVBDataSet::comments() const { return dataSource()->getAllComments(); }

const NVBColorMap* NVBDataSet::colorMap() const {
	if (clr)
		return clr;
	return dataSource()->defaultColorMap();
}

void useDataSource(const NVBDataSource* source) {
  if (!source) return;
  source->refCount++;
}

void releaseDataSource(const NVBDataSource* source) {
  if (!source) return;
  source->refCount--;
  if (!source->refCount) delete const_cast<NVBDataSource *>(source);
}

/**
	* \class NVBDataSource
	*
	*
	*/

NVBDataSource::NVBDataSource() {
}

NVBDataSource::~NVBDataSource() {
}

const NVBColorMap* NVBDataSource::defaultColorMap() const {
	return new NVBGrayRampColorMap(); // TODO : move to NVBToolsFactory
}

void NVBDataSource::setOutputRequirements(NVBAxesProps axesprops) {
	outputAxesProps = axesprops;
	if (parent()) parent()->setOutputRequirements(inputRequirements());
}

/**
	* \class NVBConstructableDataSource
	*
	*
	*/

NVBConstructableDataSource::NVBConstructableDataSource() : NVBDataSource() {
}

NVBConstructableDataSource::~NVBConstructableDataSource() {
	foreach(NVBAxisMap * am, amaps) delete am;
	foreach(NVBDataSet * ds, dsets) delete ds;
	foreach(NVBColorMap * cm, cmaps) delete cm;
}

NVBAxis & NVBConstructableDataSource::addAxis(QString name, axissize_t length) {
	axs.append(NVBAxis(this,name,length));
	return axs.last();
	}

void NVBConstructableDataSource::addAxisMap(NVBAxisMap * map, axisindex_t axis) { // Try the same with NVBAxisSelector
	if (axis == -1)
		axis = axs.count()-1;
	amaps.append(map);
	axs[axis].addMapping(map);
	}

void NVBConstructableDataSource::addDataSet(QString name, double * data, NVBDimension dimension, QVector<axisindex_t> axes)  {
	if (axes.count() == 0)
		for(int i=0; i<axes.count(); i++)
			axes << i;
	dsets.append(new NVBDataSet(this,name,data,dimension,axes));
	}
