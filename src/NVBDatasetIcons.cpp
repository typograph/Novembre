#include "NVBDatasetIcons.h"
#include <QtGui/qapplication.h>
#include <QtGui/qstyle.h>
#include <QtGui/qstyleoption.h>
#include <QtGui/QPainter>
#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBDataCore.h"

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set) {

}

NVB2DIconEngine::NVB2DIconEngine(NVBDataSet* dataset)
	: QObject()
#if QT_VERSION >= 0x040300
  , QIconEngineV2()
#else
  , QIconEngine()
#endif
	, dset(0)
{
	cache << 0 << 0 << 0 << 0;
	setSource(dataset);
}

virtual NVB2DIconEngine::~NVB2DIconEngine() {
	setSource(0);	 // disconnects dataset
	redrawCache(); // cleans up cache list
}

void NVB2DIconEngine::setSource(NVBDataSet * dataset = 0) {
	if (dset) {
		dset->disconnect(this);
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
	
	if (!cache.at(QIcon::Normal))
		redrawCache();
	if (!cache.at(mode)) {
		QStyleOption opt(0);
		opt.palette = QApplication::palette();
		cache.at(mode) = QApplication::style()->generatedIconPixmap(mode,cache.at(QIcon::Normal),&opt);
		}

	QPixmap * p = cache.at(mode);
	if (p || p = cache.at(QIcon::Normal)) {
		QSize z = p->size();
		z.scale(rect.size(),Qt::KeepAspectRatio);
		QRect nrect = QRect(QPoint(),z);
		nrect.moveCenter(rect.center());
		painter->drawPixmap(nrect,*p);
	}
}

void NVB2DIconEngine::redrawCache()
{
	foreach(QPixmap *p,cache)
		if (p) delete p;
	cache.clear();
	cache << 0 << 0 << 0 << 0;

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
	setSource(dataset);
}

NVB1DIconEngine::~NVB1DIconEngine()
{
	setSource(0);	 // disconnects dataset
}

void NVB1DIconEngine::setSource(NVBDataSet* dataset = 0)
{
	if (dset) {
		dset->disconnect(this);
		dset = 0;
		}

	dset = dataset;

	if (dset) {
//		connect(dset, SIGNAL(dataChanged()), SLOT(redrawCache()) );
		connect(dset, SIGNAL(destroyed()), SLOT(setSource()) );
		}
}

void NVB1DIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode , QIcon::State )
{
	if (!dset) return;

	QVector<axisindex_t> sliceAxes;
	for(axisindex_t i = 0; i < dset->nAxes(); i += 1)
		sliceAxes << i;
	sliceAxes.remove(taxis);

	QVector<axissize_t> sliceixs(dset->nAxes()-1,0);

	// Big question - how many curves do we draw ?
	// This should, technically, be a user setting
	// Let's be crazy and draw everything
	
	NVBValueScaler<double,int> h(dset->min(),dset->max(),0,rect.height()-1);
	NVBValueScaler<double,int> w(0,dset->sizeAt(taxis)-1,0,rect.width()-1);
	
	forAllSlices(dset,sliceAxes) {

    painter.setPen(QPen(colorlist.at(i)));


    for ( int j=1; j<page->datasize().width(); j++) {
      painter.drawLine(w.scale(data->x(j-1)),h.scale(data->y(j-1)),w.scale(data->x(j)),h.scale(data->y(j)));
      }
		
	}
	
}
