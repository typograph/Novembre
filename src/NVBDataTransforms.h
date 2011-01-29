#ifndef NVBDATATRANSFORMS_H
#define NVBDATATRANSFORMS_H

#include <QtCore/QVector>
#include "NVBDataGlobals.h"

class NVBDataSet;
/*
class NVBMaxMinTransform : public NVBDataTransform {
	private:
		virtual double singleValueTransform(double) const { return 0; }
		static double min (const double * data, axisindex_t n, const axissize_t * sizes);
		static double max (const double * data, axisindex_t n, const axissize_t * sizes);
	public:
		NVBMaxMinTransform();
		
		static double findMinimum(const NVBDataSet * data);
		static double findMinimum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> targetaxes);
		static double * findMinima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes);
		static double findMaximum(const NVBDataSet * data);
		static double findMaximum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> targetaxes);
		static double * findMaxima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes);

		virtual void operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const;
};
*/

namespace NVBMaxMinTransform {
double min (const double * data, axisindex_t n, const axissize_t * sizes);
double max (const double * data, axisindex_t n, const axissize_t * sizes);
double findMinimum(const NVBDataSet * data);
double findMaximum(const NVBDataSet * data);
}
#endif // NVBDATATRANSFORMS_H
