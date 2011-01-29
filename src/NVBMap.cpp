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
	, axisH(0)
	, axisV(1)
{
	if (data->nAxes() < 2)
		NVBOutputError("Trying to color a 1D-strip is not really meaningful");
	calculateSliceAxes();
}

void NVBColorInstance::setImageAxes(axisindex_t x, axisindex_t y) {
	axisH = x;
	axisV = y;
	calculateSliceAxes();
	}

QVector<axisindex_t> NVBColorInstance::getSliceAxes() {
	return sliceAxes;
	}

void NVBColorInstance::setXAxis(axisindex_t x) {
	axisH = x;
	calculateSliceAxes();
	}

void NVBColorInstance::setYAxis(axisindex_t y) {
	axisV = y;
	calculateSliceAxes();
	}

QPixmap * NVBColorInstance::colorize(QVector<axissize_t> slice = QVector<axissize_t>(), QSize i_wxh = QSize()) const {
	if (slice.isEmpty())
		slice.fill(0,data->nAxes()-2);
	if (i_wxh.isEmpty())
		i_wxh = QSize(data->sizeAt(axisH),data->sizeAt(axisV));
	double * tdata = sliceDataSet(data,sliceAxes,slice);
	QPixmap * timg = colorize(tdata,QSize(data->sizeAt(axisH),data->sizeAt(axisV)),i_wxh);
	free(tdata);
	return timg;
}

QPixmap * NVBColorInstance::colorize(const double * zs, QSize d_wxh, QSize i_wxh) const {
	if (!zs) return 0;

	if (!i_wxh.isValid()) i_wxh = d_wxh;

	QImage * result = new QImage(i_wxh,QImage::Format_ARGB32);
	if (!result) return 0;
	result->fill(0x00FFFFFF);

	if (i_wxh != d_wxh) {
		NVBValueScaler<int,int> w(0,i_wxh.width(),0,d_wxh.width());
		NVBValueScaler<int,int> h(0,i_wxh.height(),0,d_wxh.height());
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++) {
					result->setPixel(i,j,colorize(zs[w.scaleInt(i)+ d_wxh.width()*h.scaleInt(j)]));
				}
		}
	else {
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++) {
					result->setPixel(i,j,colorize(zs[i+i_wxh.width()*j]));
				}
		}

	QPixmap * p = new QPixmap(result);
	delete result;
	return p;
}

void NVBColorInstance::calculateSliceAxes() {
	sliceAxes.clear();
	for(axisindex_t i = 0; i < data->nAxes(); i++)
		if ( i!= axisH && i!= axisV )
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
