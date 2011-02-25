#include "NVBDatasetIcons.h"
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QPainter>
#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBDataCore.h"

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set) {
	if (!set) return QIcon();
	
	switch (set->type()) {
		case NVBDataSet::Spectroscopy :
			return QIcon(new NVB1DIconEngine(set));
		case NVBDataSet::Topography :
			return QIcon(new NVB2DIconEngine(set));
		default:
			switch (set->nAxes()) {
				case 0:
					NVBOutputError("Dataset with no axes");
					return QIcon();
				case 1:
					return QIcon(new NVB1DIconEngine(set));
				default: // FIXME - actually, 2D is the default option only if the dimensions of first two axes is the same
					return QIcon(new NVB2DIconEngine(set));
				}
		}
}

NVB2DIconEngine::NVB2DIconEngine(const NVBDataSet* dataset)
	: QObject()
#if QT_VERSION >= 0x040300
  , QIconEngineV2()
#else
  , QIconEngine()
#endif
	, dset(0)
{
	cache << QPixmap() << QPixmap() << QPixmap() << QPixmap();
	setSource(dataset);
}

NVB2DIconEngine::~NVB2DIconEngine() {
	setSource(0);	 // disconnects dataset
	redrawCache(); // cleans up cache list
}

void NVB2DIconEngine::setSource(const NVBDataSet * dataset) {
	if (dset) {
		disconnect(dset,0,this,0);
		dset = 0;
		delete i;
		i = 0;
		}

	dset = dataset;
	
	if (dset) {
		i = dset->colorMap()->instantiate(dset);
		connect(dset, SIGNAL(dataChanged()), SLOT(redrawCache()) );
		connect(dset, SIGNAL(destroyed()), SLOT(setSource()) );
		}
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

	if (dset && i) cache[QIcon::Normal] = i->colorize();
}

NVB1DIconEngine::NVB1DIconEngine(const NVBDataSet* dataset)
	: QObject()
#if QT_VERSION >= 0x040300
  , QIconEngineV2()
#else
  , QIconEngine()
#endif
	, dset(0)
{
	selector.addAxisByIndex(0);
	setSource(dataset);
}

NVB1DIconEngine::~NVB1DIconEngine()
{
	setSource(0);	 // disconnects dataset
}

void NVB1DIconEngine::setSource(const NVBDataSet* dataset)
{
	if (dset) {
		disconnect(dset,0,this,0);
		dset = 0;
		cache.clear();
		instance.reset();
		}

	dset = dataset;

	if (dset) {
		connect(dset, SIGNAL(dataChanged()), SLOT(redrawCache()) );
		connect(dset, SIGNAL(destroyed()), SLOT(setSource()) );
		instance = selector.instantiate(dset);
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
		forEachSliceAcross(instance) {
			p.setPen(QPen(instance.associatedColor(SLICE)));
			for (axissize_t j = maxw-1; j > 0; j--)
				p.drawLine(w.scale(j-1),h.scale(SLICE.data[j-1]),w.scale(j),h.scale(SLICE.data[j]));
			}
		}
	else if ((axissize_t) size.width() < maxw) { // Draw select points
		NVBValueScaler<int,axissize_t> w(0,size.width()-1,0,maxw-1);
		forEachSliceAcross(instance) {
			p.setPen(QPen(instance.associatedColor(SLICE)));
			for (axissize_t j = size.width()-1; j > 0; j--)
				p.drawLine(j-1,h.scale(SLICE.data[w.scale(j-1)]),j,h.scale(SLICE.data[w.scale(j)]));
			}
		}
	else {
		forEachSliceAcross(instance) { // draw 1:1
			p.setPen(QPen(instance.associatedColor(SLICE)));
			for (axissize_t j = maxw-1; j > 0; j--)
				p.drawLine(j-1,h.scale(SLICE.data[j-1]),j,h.scale(SLICE.data[j]));
			}
		}
	p.end();
	
	return pxm;
}
	

