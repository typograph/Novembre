#ifndef NVBDATATRANSFORMS_H
#define NVBDATATRANSFORMS_H

#include <QtCore/QVector>
#include "NVBDataCore.h"

class NVBDataSet;

class NVBMaxMinTranform : public NVBDataTransform {
	private:
		virtual double singleValueTransform(double a) const { return 0; }
		static double min (const double * data, axisindex_t n, const axissize_t * sizes);
		static double min (const double * data, axisindex_t n, const axissize_t * sizes);
	public:
		static double  findMinimum(NVBDataSet * data, QVector<axisindex_t> sliceaxes = QVector<axisindex_t>(), QVector<axisindex_t> targetaxes = QVector<axisindex_t>());
		static double * findMinima(NVBDataSet * data, QVector<axisindex_t> sliceaxes);
		static double  findMaximum(NVBDataSet * data, QVector<axisindex_t> sliceaxes = QVector<axisindex_t>(), QVector<axisindex_t> targetaxes = QVector<axisindex_t>());
		static double * findMaxima(NVBDataSet * data, QVector<axisindex_t> sliceaxes);

		virtual void operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const;
}

#endif // NVBDATATRANSFORMS_H
