#include "NVBDataTransforms.h"
#include "NVBDataCore.h"
#include "NVBDataSource.h"

/*NVBMaxMinTransform::NVBMaxMinTransform()
{
}
*/
/*
void NVBMaxMinTransform::operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const {

}
*/

double NVBMaxMinTransform::min (const double * data, axisindex_t n, const axissize_t * sizes) {
	if (!data) return 0;
	axissize_t szd = prod(n,sizes);
	double min = data[0];
	for (axissize_t i = 1; i<szd; i++)
		if (data[i] < min)
			min = data[i];
	return min;
}

double NVBMaxMinTransform::max (const double * data, axisindex_t n, const axissize_t * sizes) {
	if (!data) return 0;
	axissize_t szd = prod(n,sizes);
	double max = data[0];
	for (axissize_t i = 1; i<szd; i++)
		if (data[i] > max)
			max = data[i];
	return max;
}

double NVBMaxMinTransform::findMinimum(const NVBDataSet * data) {
	return NVBMaxMinTransform::min(data->data(),data->nAxes(),data->sizes().constData());
}

double NVBMaxMinTransform::findMaximum(const NVBDataSet * data) {
	return NVBMaxMinTransform::max(data->data(),data->nAxes(),data->sizes().constData());
}

/*
static double NVBMaxMinTransform::findMinimum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> slice) {
	double * tdata = sliceDataSet(data,sliceaxes,slice);
	QVector<axissize_t> tgsizes = subvector(dsizes,sliceaxes);
	double mindata = NVBMaxMinTransform::min(tdata,tgsizes.count(),tgsizes.constData());
	free(tdata);
	return mindata;
}

static double  NVBMaxMinTransform::findMaximum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> targetaxes) {
	double * tdata = sliceDataSet(data,sliceaxes,slice);
	QVector<axissize_t> tgsizes = subvector(dsizes,sliceaxes);
	double maxdata = NVBMaxMinTransform::max(tdata,tgsizes.count(),tgsizes.constData());
	free(tdata);
	return maxdata;
}

static double * NVBMaxMinTransform::findMinima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes) {
}

static double * NVBMaxMinTransform::findMaxima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes) {
}
*/
