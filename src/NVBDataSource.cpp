#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBDataTransforms.h"
#include "NVBColorMaps.h"
#include "NVBAxisMaps.h"
#include "NVBFile.h"
#include "QtCore/QCoreApplication"

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
	, zmin(1)
	, zmax(-1)
	{
		connect(this,SIGNAL(dataChanged()),SLOT(invalidateCaches()));
		connect(parent,SIGNAL(axesResized()),this,SIGNAL(dataReformed()));
		connect(parent,SIGNAL(dataReformed()),this,SIGNAL(dataReformed()));
	}

NVBDataSet::~NVBDataSet() {
	if (d) free(d);
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

double NVBDataSet::min() const { getMinMax(); return zmin; }
double NVBDataSet::max() const { getMinMax(); return zmax; }

void NVBDataSet::getMinMax() const
{
	if (zmin > zmax) {
		NVBMaxMinTransform::findLimits(this,zmin,zmax);
		}
}


void NVBDataSet::invalidateCaches()
{
	asizes.clear();
	zmin = 1;
	zmax = -1;
}


NVBVariant NVBDataSet::getComment(const QString & key, bool recursive) const {
	
	if (comments.contains(key))
		return comments.value(key);
	
	if (recursive && dataSource())
		return dataSource()->getComment(key);
	
	return NVBVariant();
}

const NVBColorMap* NVBDataSet::colorMap() const {
	if (clr)
		return clr;
	return dataSource()->defaultColorMap();
}

NVBDataColorInstance* NVBDataSet::colorInstance() const
{
	if (const NVBColorMap * m = colorMap())
		return m->instantiate(this);
	return 0;
}

NVBAxisMapping NVBDataSet::mapFromSource(const NVBAxisMap* map) const
{
	NVB_ASSERT(p,"Dataset without parent");
	return mapFromSource(p->getMapping(map));
}

NVBAxisMapping NVBDataSet::mapFromSource(const NVBAxisMapping& mapping) const
{
	NVB_ASSERT(p,"Dataset without parent");
	return NVBAxisMapping(mapping.map, mapFromSource(mapping.axes));
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
//	return new NVBGrayRampColorMap(); // TODO : move to NVBToolsFactory

	NVBColorMap * m = QCoreApplication::instance()->property("DefaultGradient").value<NVBColorMap*>();
	if (m) return m;
	return new NVBGrayRampColorMap();

//	return new NVBRGBMixColorMap(
//				new NVBGrayStepColorMap(
//					QList<double>() << 0 << 0.12549 << 0.360784 << 0.596078 << 0.737255 << 1,
//					QList<double>() << 0 << 0.466667 << 0.776471 << 0.933333 << 0.976471 << 0.976471
//					),
//				new NVBGrayStepColorMap(
//					QList<double>() << 0 << 0.0666667 << 0.184314 << 0.415686 << 0.713725 << 0.909804 << 1,
//					QList<double>() << 0 << 0 << 0.0823529 << 0.376471 << 0.815686 << 0.972549 << 0.976471
//					),
//				new NVBGrayStepColorMap(
//					QList<double>() << 0 << 0.333333 << 0.627451 << 0.862745 << 1,
//					QList<double>() << 0 << 0 << 0.298039 << 0.835294 << 0.984314
//					)
//				);

//	return new NVBHSVWheelColorMap(0.00555,0.163589,0.96796,0.90137,0.32778,1);
}

void NVBDataSource::setOutputRequirements(NVBAxesProps axesprops) {
	outputAxesProps = axesprops;
	if (parent()) parent()->setOutputRequirements(inputRequirements());
}

NVBVariant NVBDataSource::collectComments(const QString& key) const
{
	NVBVariant v = getComment(key,false);
	if (v.isValid()) return v;
	
	NVBVariantList l;
	foreach(NVBDataSet * s, this->dataSets()) {
		NVBVariant v = s->getComment(key,false);
		if (v.isValid())
			l << v;
		}

	if (l.isEmpty())
		return NVBVariant();
	else if (l.count() == 1)
		return l.first();
	else
		return NVBVariant(l);
	
}

NVBAxisMapping NVBDataSource::getMapping(const NVBAxisMap* map) const
{
	for(axisindex_t i = 0; i<nAxes(); i++)
		foreach(NVBAxisMapping m, axis(i).maps())
			if (m.map == map)
				return m;
	return NVBAxisMapping();
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
		NVBOutputVPMsg("Created without file");
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

NVBDataSet * NVBConstructableDataSource::addDataSet(QString name, double* data, NVBUnits dimension, NVBDataComments datacomments, QVector< axisindex_t > axes, NVBDataSet::Type type, NVBColorMap * map)
{
	if (axes.count() == 0)
		for(int i=0; i<this->axes().count(); i++)
			axes << i;
	dsets.append(new NVBDataSet(this,name,data,dimension,axes,type,map));
	dsets.last()->comments = datacomments;
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
NVBVariant NVBDataSource::getComment(const QString& key, bool recursive) const {
	if (comments.contains(key))
		return comments.value(key);
		
	if (recursive && origin()) {
		NVBVariant comment = origin()->getComment(key);
		if (comment.isValid())
			return comment;
		}
	
	if (parent())
		return parent()->getComment(key,recursive);
	
	return NVBVariant();
}

/**
 * If you add comments directly to the dataset, some of the comments might get duplicated
 * between datasets. It is more useful to put the common comments into NVBDataSource,
 * and the individual ones into datasets.
 * \a newComments parameter will be filtered.
 * \li If a comment from \a newComments is already in the datasource and is the same, it will be removed from \anewComments.
 * \li If a comment from \a newComments is already in the datasource and is different, the comment in the datasource will be propagated to all already included datasets, and will be left in \a newComments
 * \li If a comment is new, it will just stay in \a newComments
 * \li If datasource has no comments, \a newComments will become the new comment list and the original object will be cleared
 * 
 * Intended use:
 * \code
 * NVBDataComments c;
 * ...
 * c.insert(...);
 * ...
 * NVBConstructableDataSource * d = new NVBConstructableDataSource;
 * d->filterAddComments(c);
 * d->addDataSet(...,c,...)
 * \endcode
 **/
void NVBConstructableDataSource::filterAddComments(NVBDataComments & _cms)
{
	if (_cms.isEmpty())
		return;
	
	if (dataSets().count() == 0) {
		comments.unite(_cms);
		_cms.clear();
		return;
		}
		
	if (comments.isEmpty()) {
		// It can happen, actually - if more than 2 sets have the same set of different comments
//		NVBOutputPMsg("Adding first comments to a non-empty datasource. This was probably not intended.");
		return;
		}
		
	foreach (QString key, comments.keys())
		if (_cms.contains(key) && _cms.value(key) == comments.value(key))
				_cms.remove(key);
		else {
			foreach(NVBDataSet * set, dsets)
				set->comments.insert(key,comments.value(key));
			comments.remove(key);
			}
			
	return;
}

NVBDataComments NVBDataSource::getAllComments() const {
	if (!parent()) 
		return comments;
	else
		return NVBDataComments(comments).unite(parent()->comments);
}
