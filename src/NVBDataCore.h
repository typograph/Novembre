#ifndef NVBDATACORE_H
#define NVBDATACORE_H

#include <QVector>

#include "NVBDataSource.h"

/**
	* \class NVBDataTransform
	*
	* Transforms datasets using \fn transfromNArray, but is a bit higher order.
	* Subclass in your filters.
	*/
class NVBDataTransform {
	protected:
		QList<NVBAxis> as;
		QVector<int> resultSize;
		virtual double singleValueTransform(double a) const = 0;
	public:
		NVBDataTransform(QList<NVBAxis> axes):as(axes) {;}

		/// Tranforms a dataset using this transform
		NVBDataSet * transformDataSet(const NVBDataSet * data, QVector<int> sliceaxes, QVector<int> targetaxes = QVector<int>());

		/// The transform function for \a transformNArray
		virtual void operator() (const double * data, int n, const int * sizes, int m, const int * slice, double * target) const;
};

/// Get a slice at defined indexes
void sliceNArray(const double * const data, int n, double * target, int m, const int * sizes, const int * sliceaxes, const int * slice, const int * newaxes);

/// Transform data using a transform function
//template <typename Transform>
double * transformNArray(const double * data, int n, const int * sizes,
																							int m, const int * sliceaxes, const int * targetaxes,
																							int p, const int * newsizes,
																							const NVBDataTransform & transform );

double * transformNArray(const double * data, int n, const int * sizes,
																							int m, const int * sliceaxes, const int * targetaxes,
																							int p, const int * newsizes,
						void (*transform)(const double *, int, const int *, int, const int *, double * ) );


/// average data arrray
void average(const double * data, int n, const int * sizes, int nc, const int * coords, double * target);

/// Reorder array axes
double* reorderNArray(const double * data, int n, const int * sizes, const int * neworder);

#endif
