//
// C++ Implementation: NVBIconVizDelegates
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBIconVizDelegates.h"
#include "NVBContColoring.h"

QImage * colorizeWithPlaneSubtraction(NVB3DDataSource * page) {
	const double * pdata = page->getData();

	double xnorm = 0, ynorm = 0;

	int iw = page->resolution().width();
	int ih = page->resolution().height();
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

	NVBRescaleColorModel * rm = new NVBRescaleColorModel(page->getColorModel());
	rm->setLimits(zmin,zmax);

	QImage * i = dynamic_cast<NVBContColorModel*>(rm)->colorize( ndata , page->resolution() );
	delete rm;
	free(ndata);
	return i;
}

NVBTopoIconDelegate::NVBTopoIconDelegate(NVBDataSource * source):NVBIconVizDelegate(source)
{
  if (source->type() != NVB::TopoPage) throw;
  page = (NVB3DDataSource*)(source);
//  if (!page) throw nvberr_no_sense;
  redrawCache();
}

void NVBTopoIconDelegate::redrawCache()
{
  if (cache) delete cache;
	cache = colorizeWithPlaneSubtraction(page);
}

void NVBTopoIconDelegate::setSource(NVBDataSource * source)
{
  if (page) page->disconnect(this);
  if (source->type() != NVB::TopoPage) {
    delete cache;
    cache = new QImage();
    page = 0;
    }
  else {
    page = (NVB3DDataSource*)(source);
    connectSource(source);
//  if (!page) throw nvberr_no_sense;
    redrawCache();
    }
}

NVBSpecIconDelegate::NVBSpecIconDelegate(NVBDataSource * source):NVBIconVizDelegate(source)
{
  if (source->type() != NVB::SpecPage) throw;
  page = (NVBSpecDataSource *)source;

  cache = new QImage();
}

void NVBSpecIconDelegate::paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state)
{
  if (rect.size().width() > cache->size().width() || rect.size().height() > cache->size().height())
    redrawCache(rect.size());
  NVBIconVizDelegate::paint(painter, rect, mode, state);
}

void NVBSpecIconDelegate::redrawCache(const QSize & size)
{
  if (!page) return;

  QList<QwtData *> datalist = page->getData();
  QList<QColor> colorlist = page->colors();

  if (cache) delete cache;
  if (size.isEmpty()) {
    cache = new QImage();
    return;
    }
  cache = new QImage(size,QImage::Format_ARGB32);
	if (!cache) {
		NVBOutputError("Not enough memory to recereate cache");
		return;
		}
  cache->fill(0x00FFFFFF);

//  NVBValueScaler<double,int> h(page->getZMax(),page->getZMin(),0,npoints-1);

  QPainter painter;
  painter.begin(cache);

  for (int i=0; i<datalist.size(); i++) {

    QwtData * data = datalist.at(i);
    QRectF zRect = data->boundingRect().normalized();

    painter.setPen(QPen(colorlist.at(i)));
  
    NVBValueScaler<double,int> h(zRect.bottom(),zRect.top(),0,size.height()-1);
    NVBValueScaler<double,int> w(zRect.left(),zRect.right(),0,size.width()-1);
    
    for ( int j=1; j<page->datasize().width(); j++) {
      painter.drawLine(w.scale(data->x(j-1)),h.scale(data->y(j-1)),w.scale(data->x(j)),h.scale(data->y(j)));
      }
  
    }
  painter.end();
}

void NVBSpecIconDelegate::setSource(NVBDataSource * source)
{
  if (page) page->disconnect(this);
  page = 0;

  if (source->type() != NVB::SpecPage) {
    delete cache;
    cache = new QImage();
    }
  else {
    page = (NVBSpecDataSource*)(source);
    connectSource(source);
    redrawCache();
    }
}
