#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBDataTransforms.h"
#include "NVBColorMaps.h"
#include "NVBAxisMaps.h"
#include "NVBFile.h"

NVBDataSet::NVBDataSet(NVBDataSource * parent,
			QString name,
			double * data,
			NVBUnits dimension,
			QVector<axisindex_t> axes,
			Type tp,
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

axissize_t NVBDataSet::sizeAt(axisindex_t i) const {
	return p->axis(as.at(i)).length();
	}

double NVBDataSet::min() const { return NVBMaxMinTransform::findMinimum(this); }
double NVBDataSet::max() const { return NVBMaxMinTransform::findMaximum(this); }

NVBDataComments NVBDataSet::comments() const {
	if (dataSource())
		return dataSource()->getAllComments();
	else
		return NVBDataComments();
}

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

NVBConstructableDataSource::NVBConstructableDataSource(NVBFile * orig)
 : NVBDataSource() 
 , o(orig)
{
	if (!o)
		NVBOutputError("Created without file");
}

NVBConstructableDataSource::~NVBConstructableDataSource() {
	foreach(NVBAxisMapping am, amaps) delete am.map;
	foreach(NVBDataSet * ds, dsets) delete ds;
	foreach(NVBColorMap * cm, cmaps) delete cm;
}

NVBAxis & NVBConstructableDataSource::addAxis(QString name, axissize_t length) {
	axs.append(NVBAxis(this,name,length));
	return axs.last();
	}

void NVBConstructableDataSource::addAxisMap(NVBAxisMap * map, QVector<axisindex_t> axes) { // Try the same with NVBAxisSelector
	if (axes.isEmpty())
		axes << axs.count()-1;
	amaps.append(NVBAxisMapping(map,axes));
	foreach(axisindex_t a, axes)
		axs[a].addMapping(amaps.last());
	}

NVBDataSet * NVBConstructableDataSource::addDataSet(QString name, double* data, NVBUnits dimension, QVector< axisindex_t > axes, NVBDataSet::Type type, NVBColorMap * map)
{
	if (axes.count() == 0)
		for(int i=0; i<axes.count(); i++)
			axes << i;
	dsets.append(new NVBDataSet(this,name,data,dimension,axes,type,map));
	return dsets.last();
}

void NVBAxis::addMapping(NVBAxisMapping mapping) {
	ms.append(mapping);
	if (mapping.map->mappingType() == NVBAxisMap::Physical)
		pm = dynamic_cast<NVBAxisPhysMap*>(mapping.map);
	}
	
const NVBAxis& NVBDataSet::axisAt(axisindex_t i) const {
	return p->axis(as.at(i));
	}

axisindex_t NVBAxis::parentIndex() const
 {
	if (!p) return -1;
	return p->axes().indexOf(*this);
}

NVBAxis NVBDataSource::axisByName(QString name) const
{
	for(axisindex_t i = 0; i<nAxes(); i+=1)
		if (axis(i).name() == name)
			return axis(i);

	return NVBAxis();
}

axisindex_t NVBDataSource::axisIndexByName(QString name) const
{
	for(axisindex_t i = 0; i<nAxes(); i+=1)
		if (axis(i).name() == name)
			return i;

	return -1;
}
