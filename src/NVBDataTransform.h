#ifndef NVBDATATRANSFORM_H
#define NVBDATATRANSFORM_H

#include "NVBDataCore.h"

/**
	* \class NVBDataTransform
	*
	* Transforms datasets using \fn transfromNArray, but is a bit higher order.
	* Subclass in your filters.
	*/
class NVBDataTransform {
	protected:
		QList<NVBAxis> as;
		QVector<axissize_t> resultSize;
		virtual double singleValueTransform(double a) const = 0;
	public:
		NVBDataTransform(QList<NVBAxis> axes):as(axes) {;}

		/// Tranforms a dataset using this transform
		NVBDataSet * transformDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axisindex_t> targetaxes = QVector<axisindex_t>());

		/// The transform function for \a transformNArray
		virtual void operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const;
};

/// Transform data using a transform function
//template <typename Transform>
double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
																							const NVBDataTransform & transform );

#endif