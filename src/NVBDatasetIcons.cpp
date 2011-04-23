#include "NVBDatasetIcons.h"
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QPainter>
#include "NVBDataSource.h"
#include "NVBAxisSelector.h"
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
			NVBAxisSelector s;
			s.addCase(1).addAxis().need(2,NVBSelectorAxis::Units);
			s.addCase(2).addAxis().need(2);
			s.addCase(3).addAxis();
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
	selector.addCase().addAxis().need(2,NVBSelectorAxis::Units);
	selector.addCase().addAxis().need(2);
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
		delete ci;
		ci = 0;
		si.reset();
		}

	dset = dataset;
	
	if (dset) {
		si = selector.instantiate(dset);
		if (!si.isValid()) {
			NVBOutputError("Dataset has less than two axes");
			dset = 0;
			return;
			}
		ci = dset->colorMap()->instantiate(dset);
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

	if (dset && ci) {
		// get slice at (0,0) and draw it
		forSingleSliceAlong(si) {
			cache[QIcon::Normal] = ci->colorize(SLICE.data,QSize(si.matchedAxis(0).length(),si.matchedAxis(1).length()));
			}
		}
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
	selector.addAxis();
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
		instance = selector.instantiate(dset);
		if (!instance.isValid()) {
			NVBOutputError("Dataset has less than one axis!");
			dset = 0;
			return;
			}
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
