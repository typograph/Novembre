#include "NVBMap.h"
#include "NVBDataCore.h"
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include "NVBDataTransforms.h"

NVBColorInstance::NVBColorInstance ( const NVBColorMap* _map )
	: source(_map)
	, zmin(0)
	, zmax(1)
	, zscaler(NVBValueScaler<double,double>(0,1,0,1))
{
	if (!_map) NVBOutputError("No map supplied");
}

void NVBColorInstance::autoscale(const double * zs, axissize_t size) {
	double dmin, dmax;
	NVBMaxMinTransform::minmax(zs, 1, &size, dmin, dmax);
	setLimits(dmin,dmax);
}

void NVBColorInstance::autoscale(const double * zs, QSize d_wxh) {
	double dmin, dmax;
	axissize_t size = d_wxh.width() * d_wxh.height();
	NVBMaxMinTransform::minmax(zs, 1, &size, dmin, dmax);
	setLimits(dmin,dmax);
}

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

void NVBColorInstance::setLimits ( double vmin, double vmax ) {
	// It is impossible to set reasonable values if vmin == vmax
	if (vmin != vmax) {
		zscaler.change_input(zmin,zmax,vmin,vmax);
		zmin = vmin;
		zmax = vmax;
		}
	else { // Map to low limit
		zscaler.change_input(zmin,zmax,vmin,vmax+1);
		zmin = vmin;
		zmax = vmax+1;
		}
}

void NVBColorInstance::overrideLimits ( double vmin, double vmax ) {
	zmin = vmin;
	zmax = vmax;
}

QRgb NVBColorInstance::colorize ( double z ) const {
	double zn = zscaler.scale(z);
	if (zn < 0 || !source)
		zn = 0;
	else if (zn > 1)
		zn = 1;
	return source->colorize(zn);
}

NVBDataColorInstance::NVBDataColorInstance ( const NVBDataSet* _data, const NVBColorMap* _map )
	: NVBColorInstance(_map)
	, data(_data)
	, rescaleOnDataChange(true)
{
	useDataSet(data);
	xyAxes << 0 << 1;

	if (!data) NVBOutputError("No data supplied");
	else {
		setLimits(data->min(),data->max());
		
		if (data->nAxes() < 2)
			NVBOutputError("Trying to color a 1D-strip is not really meaningful");
		
		connect(data,SIGNAL(dataChanged()),this,SLOT(parentDataChanged()));
		connect(data,SIGNAL(dataReformed()),this,SLOT(parentDataReformed()));
		
		calculateSliceAxes();
		}
}

NVBDataColorInstance::NVBDataColorInstance(const NVBDataColorInstance & other)
	: QObject()
	,	NVBColorInstance(other.source)
	, data(other.data)
	, sliceAxes(other.sliceAxes)
	, xyAxes(other.xyAxes)
	, rescaleOnDataChange(other.rescaleOnDataChange)
	{
	useDataSet(data);

	if (data) {
		setLimits(data->min(),data->max());

		connect(data,SIGNAL(dataChanged()),this,SLOT(parentDataChanged()));
		connect(data,SIGNAL(dataReformed()),this,SLOT(parentDataReformed()));
		}
	}

NVBDataColorInstance & NVBDataColorInstance::operator=(const NVBDataColorInstance & other) {
	if (this != &other) {
		if (data) {
			disconnect(data,0,this,0);
			releaseDataSet(data);
			}

		sliceAxes = other.sliceAxes;
		xyAxes = other.xyAxes;
		rescaleOnDataChange = other.rescaleOnDataChange;
		data = other.data;

		if (data) {
			setLimits(data->min(),data->max());
			connect(data,SIGNAL(dataChanged()),this,SLOT(parentDataChanged()));
			connect(data,SIGNAL(dataReformed()),this,SLOT(parentDataReformed()));
			}
		}
	return *this;
	}

NVBDataColorInstance::~NVBDataColorInstance() {
	releaseDataSet(data);
	}

void NVBDataColorInstance::setImageAxes(QVector< axisindex_t > xy)
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


void NVBDataColorInstance::setImageAxes(axisindex_t x, axisindex_t y) {
	xyAxes[0] = x;
	xyAxes[1] = y;
	calculateSliceAxes();
	}

QVector<axisindex_t> NVBDataColorInstance::getSliceAxes() {
	return sliceAxes;
	}

void NVBDataColorInstance::setXAxis(axisindex_t x) {
	if (xyAxes.last() != x) {
		xyAxes[0] = x;
		calculateSliceAxes();
		}
	else
		NVBOutputError(QString("Axis %1 already used as Y").arg(x));
	}

void NVBDataColorInstance::setYAxis(axisindex_t y) {
	if (xyAxes.first() != y) {
		xyAxes[1] = y;
		calculateSliceAxes();
		}
	else
		NVBOutputError(QString("Axis %1 already used as X").arg(y));
	}

QPixmap NVBDataColorInstance::colorize(QVector<axissize_t> slice, QSize i_wxh) const {
	if (!data) return QPixmap();
	if (slice.isEmpty())
		slice.fill(0,data->nAxes()-2);
	if (i_wxh.isEmpty())
		i_wxh = QSize(data->sizeAt(xyAxes.first()),data->sizeAt(xyAxes.last()));
	double * tdata = sliceDataSet(data,sliceAxes,slice,xyAxes);
	QPixmap timg = NVBColorInstance::colorize(tdata,QSize(data->sizeAt(xyAxes.first()),data->sizeAt(xyAxes.last())),i_wxh);
	free(tdata);
	return timg;
}

QPixmap NVBDataColorInstance::colorize(NVBDataSet * dset, QVector<axissize_t> slice, QSize i_wxh)
{
	if (!dset) return QPixmap();
	NVBDataColorInstance * i = dset->colorInstance();
	if (!i) return QPixmap();
	QPixmap pm = i->colorize(slice,i_wxh);
	delete i;
	return pm;
}


void NVBDataColorInstance::calculateSliceAxes() {
	sliceAxes.clear();
	if (!data) return;
	for(axisindex_t i = 0; i < data->nAxes(); i++)
		if ( !xyAxes.contains(i) )
			sliceAxes << i;
}

void NVBDataColorInstance::parentDataChanged()
{
	if (rescaleOnDataChange)
		setLimits(data->min(), data->max());
}

void NVBDataColorInstance::parentDataReformed()
{
	if (xyAxes.first() >= data->nAxes() || xyAxes.last() >= data->nAxes())
		setImageAxes(0,1);
	// parentDataChanged is called anyway, since dataChanged() is emitted on any change
}
