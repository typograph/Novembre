#include "NVBDataTransform.h"

NVBDataTransform::NVBDataTransform()
{
}

virtual void NVBDataTransform::operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const {

}

static double NVBDataTransform::min (const double * data, axisindex_t n, const axissize_t * sizes) {
	axissize_t szd = prod(n,sizes);
	double min = data[0];
	for (axissize_t i = 1; i<szd; i++)
		if (data[i] < min)
			min = data[i];
	return min;
}

static double NVBDataTransform::max (const double * data, axisindex_t n, const axissize_t * sizes) {
	axissize_t szd = prod(n,sizes);
	double max = data[0];
	for (axissize_t i = 1; i<szd; i++)
		if (data[i] > max)
			max = data[i];
	return max;
}

static double NVBDataTransform::findMinimum(NVBDataSet * data) {
	return NVBDataTransform::min(data->data(),data->nAxes(),data->sizes().constData());
}

static double NVBDataTransform::findMaximum(NVBDataSet * data) {
	return NVBDataTransform::max(data->data(),data->nAxes(),data->sizes().constData());
}

static double NVBDataTransform::findMinimum(NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> slice) {
	double * tdata = sliceDataSet(data,sliceaxes,slice);
	QVector<axissize_t> tgsizes = subvector(dsizes,sliceaxes);
	double mindata = NVBDataTransform::min(tdata,tgsizes.count(),tgsizes.constData());
	free(tdata);
	return mindata;
}

static double  NVBDataTransform::findMaximum(NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axisindex_t> targetaxes) {
	double * tdata = sliceDataSet(data,sliceaxes,slice);
	QVector<axissize_t> tgsizes = subvector(dsizes,sliceaxes);
	double maxdata = NVBDataTransform::max(tdata,tgsizes.count(),tgsizes.constData());
	free(tdata);
	return maxdata;
}

static double * NVBDataTransform::findMinima(NVBDataSet * data, QVector<axisindex_t> sliceaxes);
static double * NVBDataTransform::findMaxima(NVBDataSet * data, QVector<axisindex_t> sliceaxes);
