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
		static void minmax (const double * data, axisindex_t n, const axissize_t * sizes, double & dmin, double & dmax);
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
	axissize_t min_index (const double * data, axisindex_t n, const axissize_t * sizes);
	axissize_t max_index (const double * data, axisindex_t n, const axissize_t * sizes);
	void minmax (const double * data, axisindex_t n, const axissize_t * sizes, double & dmin, double & dmax);
	double findMinimum(const NVBDataSet * data);
	double findMaximum(const NVBDataSet * data);
	void findLimits(const NVBDataSet * data, double & dmin, double & dmax);
}

namespace NVBAverageTransform {
	double average (const double * data, axisindex_t n, const axissize_t * sizes);
}
#endif // NVBDATATRANSFORMS_H
