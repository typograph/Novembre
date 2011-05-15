#include "NVBMap.h"
#include "NVBDataCore.h"
#include <QtGui/QImage>
#include <QtGui/QPixmap>

NVBColorInstance::NVBColorInstance ( const NVBDataSet* _data, const NVBColorMap* _map )
	: source(_map)
	, data(_data)
	, zmin(_data->min())
	, zmax(_data->max())
	, zscaler(NVBValueScaler<double,double>(zmin,zmax,0,1))
{
	xyAxes << 0 << 1;
	
	if (data->nAxes() < 2)
		NVBOutputError("Trying to color a 1D-strip is not really meaningful");
	calculateSliceAxes();
}

void NVBColorInstance::setImageAxes(QVector< axisindex_t > xy)
{
	if (xy.size() != 2) {
		NVBOutputError(QString("XY has incorrect length - %1").arg(xy.size()));
		return;
		}
		
	if (xy.first() == xy.last()) {
		NVBOutputError(QString("Cannot set %1 as both X and Y").arg(xy.first()));
		return;		
		}
		
	xyAxes = xy;
	calculateSliceAxes();
}


void NVBColorInstance::setImageAxes(axisindex_t x, axisindex_t y) {
	xyAxes[0] = x;
	xyAxes[1] = y;
	calculateSliceAxes();
	}

QVector<axisindex_t> NVBColorInstance::getSliceAxes() {
	return sliceAxes;
	}

void NVBColorInstance::setXAxis(axisindex_t x) {
	if (xyAxes.last() != x) {
		xyAxes[0] = x;
		calculateSliceAxes();
		}
	else
		NVBOutputError(QString("Axis %1 already used as Y").arg(x));
	}

void NVBColorInstance::setYAxis(axisindex_t y) {
	if (xyAxes.first() != y) {
		xyAxes[1] = y;
		calculateSliceAxes();
		}
	else
		NVBOutputError(QString("Axis %1 already used as X").arg(y));
	}

QPixmap NVBColorInstance::colorize(QVector<axissize_t> slice, QSize i_wxh) const {
	if (slice.isEmpty())
		slice.fill(0,data->nAxes()-2);
	if (i_wxh.isEmpty())
		i_wxh = QSize(data->sizeAt(xyAxes.first()),data->sizeAt(xyAxes.last()));
	double * tdata = sliceDataSet(data,sliceAxes,slice,xyAxes);
	QPixmap timg = colorize(tdata,QSize(data->sizeAt(xyAxes.first()),data->sizeAt(xyAxes.last())),i_wxh);
	free(tdata);
	return timg;
}

#include <QtCore/QDebug>

QPixmap NVBColorInstance::colorize(const double * zs, QSize d_wxh, QSize i_wxh) const {
	if (!zs) return QPixmap();

	if (!i_wxh.isValid()) i_wxh = d_wxh;

	QImage result(i_wxh,QImage::Format_ARGB32);
	result.fill(0x00FFFFFF);

	if (i_wxh != d_wxh) {
		NVBValueScaler<int,int> w(0,i_wxh.width(),0,d_wxh.width());
		NVBValueScaler<int,int> h(0,i_wxh.height(),0,d_wxh.height());
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++)
				result.setPixel(i,j,colorize(zs[w.scale(i)+ d_wxh.width()*h.scale(j)]));
		}
	else {
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++)
				result.setPixel(i,j,colorize(zs[i+i_wxh.width()*j]));
		}

	return QPixmap::fromImage(result);
}

void NVBColorInstance::calculateSliceAxes() {
	sliceAxes.clear();
	for(axisindex_t i = 0; i < data->nAxes(); i++)
		if ( !xyAxes.contains(i) )
			sliceAxes << i;
}

void NVBColorInstance::setLimits ( double vmin, double vmax ) {
  zscaler.change_input(zmin,zmax,vmin,vmax);
  zmin = vmin;
  zmax = vmax;
}

void NVBColorInstance::overrideLimits ( double vmin, double vmax ) {
  zmin = vmin;
  zmax = vmax;
}

QRgb NVBColorInstance::colorize ( double z ) const {
	double zn = zscaler.scale(z);
	if (zn < 0)
		zn = 0;
	else if (zn > 1)
		zn = 1;
	return source->colorize(zn);
}
