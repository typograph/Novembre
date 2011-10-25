#include "NVBDatasetIcons.h"
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QPainter>
#include "NVBDataSource.h"
#include "NVBAxisSelector.h"
#include "NVBMap.h"
#include "NVBDataCore.h"
#include "NVBforeach.h"
#include "NVBFile.h"
#include "NVBAxisMaps.h"

#define CASE_SAME_UNITS 1
#define CASE_2_AXES 2

/*
 * TODO This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set) {
	if (!set) return QIcon();
	
	switch (set->type()) {
		case NVBDataSet::Spectroscopy :
			return QIcon(new NVB1DIconEngine(set));
		case NVBDataSet::Topography :
			return QIcon(new NVB2DIconEngine(set));
		default:
			NVBAxisSelector s;
			s.addCase(1).addAxis().needMore(1,NVBSelectorRules::SameUnits);
			s.addCase(2).setDataMinAxes(2);
			s.addCase(3).setDataNAxes(1);
			switch (s.instantiate(set).matchedCase()) {
				case 0:
					NVBOutputError("Dataset with no axes");
					return QIcon();
				case 1: // Two axes with the same units
					return QIcon(new NVB2DIconEngine(set));
				default:
					return QIcon(new NVB1DIconEngine(set));
				}
		}
}

QList<QIcon> createDataIcons(const NVBFile * file) {
	QList<QIcon> icons;
	NVB_FOREACH(NVBDataSource * ds, file)
		Q_FOREACH(NVBDataSet * dset, ds->dataSets())
			icons << createDatasetIcon(dset);
	return icons;
}

QIcon createSpecOverlayIcon(const NVBDataSet * set, const NVBDataSet * axessource, const NVBAxisMapping & mapping) {
	if (!set) return QIcon();
	
	switch (set->type()) {
		case NVBDataSet::Spectroscopy :
			return QIcon(new NVB1DIconEngine(set));
		case NVBDataSet::Topography :
			break;
		default:
			NVBAxisSelector s;
			s.addCase(1).addAxis().needMore(1,NVBSelectorRules::SameUnits);
			s.addCase(2).setDataMinAxes(2);
			s.addCase(3).setDataNAxes(1);
			switch (s.instantiate(set).matchedCase()) {
				case 0:
					NVBOutputError("Dataset with no axes");
					return QIcon();
				case 1: // Two axes with the same units
					break;
				default:
					return QIcon(new NVB1DIconEngine(set));
				}
		}
	
	if (!axessource || !mapping.map)
		return QIcon(new NVB2DIconEngine(set));
	
	if (mapping.axes.count() == 1)
		return QIcon(new NVBMixTSIconEngine(set,dynamic_cast<NVBAxisPointMap*>(mapping.map),axessource->axisAt(mapping.axes.first()).length()));
	if (mapping.axes.count() == 2)
		return QIcon(new NVBMixTSIconEngine(set,dynamic_cast<NVBAxes2DGridMap*>(mapping.map),axessource->axisAt(mapping.axes.first()).length(),axessource->axisAt(mapping.axes.last()).length()));
	
	return QIcon(new NVB2DIconEngine(set));
}

QList<QIcon> createSpecOverlayIcons(const NVBFile* file) {
		
	NVBAxisSelector s;
	s.addCase(1).setDataType(NVBDataSet::Spectroscopy).addMapByValueType(NVBAxisMap::Point); // One axis for data taken on different points
	// Note that although setting data type seems unnecessary, we might encounter Topography dataset on this axis...
	s.addCase(2).setDataType(NVBDataSet::Spectroscopy).addAxisByUnits(NVBUnits("m")).needMore(1); // Two axes for grid

	// ^ Although two metric axes seems reasonable, we might encounter spectroscopy taken
	// on different axes, that, nevertheless, match the axes that topography will be displayed on
	// FIXME Choose the two axes based on the topography axes.
	// E.g. s.addCase(2).setDataType(NVBDataSet::Spectroscopy).setDataMinAxes(3)
	// This would choose all potentially suitable spectroscopy pages
	
	NVBSelectorFileInstance inst = s.instantiate(file);
	
	// If no such spectroscopy exists
	if (inst.matchedInstances().isEmpty()) return createDataIcons(file);
	
	// Datasources that match, take themselves as map.
	// Datasources that match several times (or 1 & 2) use first type 1 match
	
	// Datasources that didn't match 1 or 2 get one of the others
	
	QList<QIcon> icons;

	NVBAxisMapping pointmap;
	const NVBDataSet * pointdata;
	
	if (!inst.matchedInstances(1).isEmpty()) {
		NVBSelectorDataInstance dinst = inst.matchedInstances(1).first().matchedInstances(1).first();
		pointmap = dinst.matchedMaps().first();
		pointdata = dinst.matchingData();
		}
	else {
		NVBSelectorDataInstance dinst = inst.matchedInstances(2).first().matchedInstances(2).first();
		pointmap.map = new NVBAxes2DGridMap(dinst.matchedAxis(0).physMap(),dinst.matchedAxis(1).physMap());
		pointmap.axes = dinst.matchedAxes();
		pointdata = dinst.matchingData();
		}
	
	int ix;
	
	NVB_FOREACH(NVBDataSource * ds, file) {
		
		// Check for matches belonging to this datasource
		if ((ix = inst.matchedDatasources(1).indexOf(ds)) >= 0) {
			NVBSelectorDataInstance dinst = inst.matchedInstances(1).at(ix).matchedInstances(1).first();
			pointmap = dinst.matchedMaps().first();
			pointdata = dinst.matchingData();
			}
		else if ((ix = inst.matchedDatasources(2).indexOf(ds)) >= 0) {
			NVBSelectorDataInstance dinst = inst.matchedInstances(2).at(ix).matchedInstances(2).first();
			pointmap.map = new NVBAxes2DGridMap(dinst.matchedAxis(0).physMap(),dinst.matchedAxis(1).physMap());
			pointmap.axes = dinst.matchedAxes();
			pointdata = dinst.matchingData();
			}
		
		Q_FOREACH(NVBDataSet * dset, ds->dataSets())
			icons << createSpecOverlayIcon(dset,pointdata,pointmap);
		}

	if (inst.matchedInstances(1).isEmpty())
		delete pointmap.map;

	return icons;
}

NVB2DIconEngine::NVB2DIconEngine(const NVBDataSet* dataset)
	: QObject()
	, QIconEngineV2()
	, dset(0)
{
	cache << QPixmap() << QPixmap() << QPixmap() << QPixmap();
	selector.addCase(CASE_SAME_UNITS).addAxis().needMore(1,NVBSelectorAxis::SameUnits);
	selector.addCase(CASE_2_AXES).setDataMinAxes(2);
	setSource(dataset);
}

NVB2DIconEngine::NVB2DIconEngine(const NVB2DIconEngine & other)
 : QObject()
 , QIconEngineV2()
 , dset(0)
 , cache(other.cache)
 , selector(other.selector)
	{
	setSource(other.dset);
	}

NVB2DIconEngine & NVB2DIconEngine::operator=(const NVB2DIconEngine & other) {
	if (this != &other) {
		selector = other.selector;
		setSource(other.dset);
		}
	return *this;
	}

NVB2DIconEngine::~NVB2DIconEngine() {
	setSource(0);	 // disconnects dataset
//	redrawCache(); // cleans up cache list
}

void NVB2DIconEngine::setSource(const NVBDataSet * dataset) {
	if (dset) {
		disconnect(dset,0,this,0);
		releaseDataSet(dset);
		dset = 0;
		if (ci) delete ci;
		ci = 0;
		si.reset();
		}

	dset = dataset;
	
	if (dset) {
		useDataSet(dset);
		si = selector.instantiate(dset);
		if (!si.isValid()) {
			NVBOutputError("Dataset has less than two axes");
			dset = 0;
			return;
			}
		ci = dset->colorInstance();
		connect(dset, SIGNAL(dataChanged()), SLOT(redrawCache()) );
		connect(dset, SIGNAL(destroyed()), SLOT(setSource()) );
		}
	redrawCache();
}

void NVB2DIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State) {
	if (!dset) return;
	
	if (cache.at(QIcon::Normal).isNull())
		redrawCache();
	if (cache.at(mode).isNull()) {
		QStyleOption opt(0);
		opt.palette = QApplication::palette();
		cache[mode] = QApplication::style()->generatedIconPixmap(mode,cache.at(QIcon::Normal),&opt);
		}

	QPixmap p = cache.at(QIcon::Normal);
	if (!p.isNull()) {
		QSize z = p.size();
		z.scale(rect.size(),Qt::KeepAspectRatio);
		QRect nrect = QRect(QPoint(),z);
		nrect.moveCenter(rect.center());
		painter->drawPixmap(nrect,p);
	}
}

void NVB2DIconEngine::redrawCache()
{
	cache.clear();
	cache << QPixmap() << QPixmap() << QPixmap() << QPixmap();

	if (dset && ci) {
		// get slice at (0,0) and draw it
		forSingleSliceAlong(si) {
			cache[QIcon::Normal] = ((NVBColorInstance*)ci)->colorize(SLICE.data,QSize(si.matchedAxis(0).length(),si.matchedAxis(1).length()));
			}
		}
}

NVB1DIconEngine::NVB1DIconEngine(const NVBDataSet* dataset)
	: QObject()
  , QIconEngineV2()
	, dset(0)
{
	selector.addAxis();
	setSource(dataset);
}

NVB1DIconEngine::NVB1DIconEngine(const NVB1DIconEngine & other)
 : QObject()
 , QIconEngineV2()
 , dset(0)
 , selector(other.selector)
{
	setSource(0);
}

NVB1DIconEngine & NVB1DIconEngine::operator=(const NVB1DIconEngine & other) {
	if (this != &other) {
		selector = other.selector;
		setSource(other.dset);
		}
	return *this;
	}

NVB1DIconEngine::~NVB1DIconEngine()
{
	setSource(0);	 // disconnects dataset
}

void NVB1DIconEngine::setSource(const NVBDataSet* dataset)
{
	if (dset) {
//		dset->disconnect(this);
		releaseDataSet(dset);
		dset = 0;
		cache.clear();
		instance.reset();
		}

	dset = dataset;

	if (dset) {
		instance = selector.instantiate(dset);
		if (!instance.isValid()) {
			NVBOutputError("Dataset has less than one axis!");
			dset = 0;
			return;
			}
		useDataSet(dset);
		connect(dset, SIGNAL(dataChanged()), SLOT(redrawCache()) );
		connect(dset, SIGNAL(destroyed()), SLOT(setSource()) );
		}
}

void NVB1DIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode , QIcon::State )
{
	if (!dset) return;
	if (!instance.isValid()) return;

	if (!cache.contains(rect.size()))
		cache.insert(rect.size(), drawCacheAt(rect.size()));

	painter->drawPixmap(rect.topLeft(),cache.value(rect.size()));
}

QPixmap NVB1DIconEngine::drawCacheAt(QSize size) {
	QPixmap pxm(size);
	pxm.fill();
	QPainter p;
	p.begin(&pxm);

	// Big question - how many curves do we draw ?
	// FIXME This should, technically, be a user setting
	// Let's be crazy and draw everything

	NVBValueScaler<double,int> h(dset->min(),dset->max(),0,size.height()-1);

	axissize_t maxw = instance.matchedAxis(0).length();

	if ((axissize_t) size.width() > maxw) { // Lines TODO Think about drawing splines
		NVBValueScaler<axissize_t,int> w(0,maxw-1,0,size.width()-1);
		p.setRenderHint(QPainter::Antialiasing, true);
		forNSlicesAlong(instance,100) {
			p.setPen(QPen(SLICE.associatedColor()));
			for (axissize_t j = maxw-1; j > 0; j--)
				p.drawLine(w.scale(j-1),h.scale(SLICE.data[j-1]),w.scale(j),h.scale(SLICE.data[j]));
			}
		}
	else if ((axissize_t) size.width() < maxw) { // Draw select points
		NVBValueScaler<int,axissize_t> w(0,size.width()-1,0,maxw-1);
		p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::NonCosmeticDefaultPen , false);
		forNSlicesAlong(instance,100) {
			p.setPen(QPen(SLICE.associatedColor()));
			for (axissize_t j = size.width()-1; j > 0; j--)
				p.drawLine(j-1,h.scale(SLICE.data[w.scale(j-1)]),j,h.scale(SLICE.data[w.scale(j)]));
			}
		}
	else {
		forNSlicesAlong(instance,100) { // draw 1:1
			p.setPen(QPen(SLICE.associatedColor()));
			for (axissize_t j = maxw-1; j > 0; j--)
				p.drawLine(j-1,h.scale(SLICE.data[j-1]),j,h.scale(SLICE.data[j]));
			}
		}
	p.end();
	
	return pxm;
}

NVBMixTSIconEngine::NVBMixTSIconEngine(const NVBDataSet* topo, const NVBAxisPointMap* points, axissize_t npnts )
: NVB2DIconEngine(topo)
{
	if (!points) {
		NVBOutputError("A topography page and spectroscopy locations are needed");
		return;
		}
	if (!si.isValid() || si.matchedCase() != CASE_SAME_UNITS)
		NVBOutputError("Cannot map physical points to pixels");
	else {
		const NVBAxisPhysMap * xm = si.matchedAxis(0).physMap();
		const NVBAxisPhysMap * ym = si.matchedAxis(1).physMap();
		if (!xm || !ym) return; // Shouldn't happen

		NVBUnits tu = xm->units();
		if (!tu.isComparableWith(points->value(0).dimension()))
			return; // FIXME What if the topography has 3 same-units axes or 2 pairs, and the second matches?
		
		NVBValueScaler<double,double> xmapper(
			xm->value(0).getValue(tu),
			xm->value(si.matchedAxis(0).length() - 1).getValue(tu),
			0,
			si.matchedAxis(0).length()-1
			);
		
		NVBValueScaler<double,double> ymapper(
			ym->value(0).getValue(tu),
			ym->value(si.matchedAxis(1).length() - 1).getValue(tu),
			0,
			si.matchedAxis(1).length()-1
			);
		
		for (axissize_t p = 0; p < npnts; p++) {
			QPointF tp = points->value(p).point(tu);
			this->points << QPointF(xmapper.scale(tp.x()),ymapper.scale(tp.y()));
			}
		}
}

NVBMixTSIconEngine::NVBMixTSIconEngine(const NVBDataSet* topo, const NVBAxes2DGridMap* points, axissize_t nptsx, axissize_t nptsy)
: NVB2DIconEngine(topo)
{
	if (!topo || !points) {
		NVBOutputError("A topography page and spectroscopy locations are needed");
		return;
		}
	if (!si.isValid() || si.matchedCase() != CASE_SAME_UNITS)
		NVBOutputError("Cannot map physical points to pixels");
	else {
		const NVBAxisPhysMap * xm = si.matchedAxis(0).physMap();
		const NVBAxisPhysMap * ym = si.matchedAxis(1).physMap();
		if (!xm || !ym) return; // Shouldn't happen
		NVBUnits tu = xm->units();
		if (!tu.isComparableWith(points->origin().dimension()))
			return; // FIXME What if the topography has 3 same-units axes or 2 pairs, and the second matches?
			
		NVBValueScaler<double,double> xmapper(xm->value(0).getValue(tu),xm->value(si.matchedAxis(0).length() - 1).getValue(tu),0,si.matchedAxis(0).length()-1);
		NVBValueScaler<double,double> ymapper(ym->value(0).getValue(tu),ym->value(si.matchedAxis(1).length() - 1).getValue(tu),0,si.matchedAxis(1).length()-1);
		for (axissize_t x = 0; x < nptsx; x++)
			for (axissize_t y = 0; y < nptsy; y++) {
				QPointF tp = points->value(x,y).point(tu);
				this->points << QPointF(xmapper.scale(tp.x()),ymapper.scale(tp.y()));
				}
		}

}

QPixmap NVB2DIconEngine::colorizeWithPlaneSubtraction(NVBDataSet * page) {
	const double * pdata = page->data();

	double xnorm = 0, ynorm = 0;

	int iw = page->axisAt(0).length();
	int ih = page->axisAt(1).length();
	int sz = iw*ih;

	for(int i=0; i < sz; i += iw)
		xnorm += pdata[i] - pdata[i+iw-1];
	for(int i=0; i < iw; i += 1)
		ynorm += pdata[i] - pdata[i+sz-ih];

	xnorm /= (iw-1)*ih;
	ynorm /= iw*iw*(ih-1);

	double * ndata = (double *) malloc(sz*sizeof(double));

	double zmin = pdata[0], zmax = pdata[0];

	for(int i=0; i < iw; i += 1)
		for(int j=0; j < sz; j += iw) {
			ndata[i+j] = pdata[i+j] + xnorm*i + ynorm*j;
			zmin = qMin(zmin,ndata[i+j]);
			zmax = qMax(zmax,ndata[i+j]);
			}

	NVBColorInstance * rm = new NVBColorInstance(page->colorMap());
	rm->setLimits(zmin,zmax);

	QPixmap i = rm->colorize( ndata , QSize() );
	delete rm;
	free(ndata);
	return i;
}

void NVBMixTSIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
	NVB2DIconEngine::paint(painter,rect,mode,state);
	
	// Paint dots

	painter->save();
	if (mode == QIcon::Disabled) {
		painter->setPen(QPen(Qt::black));
		painter->setBrush(Qt::black);
		}
	else {
		painter->setPen(QPen(Qt::blue));
		painter->setBrush(Qt::blue);
		}

	foreach( QPointF p, points)
		painter->drawEllipse(p,2,2);

	painter->restore();
}
