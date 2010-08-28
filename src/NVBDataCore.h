#ifndef NVBDATACORE_H
#define NVBDATACORE_H

#include <QtCore/QVector>

#include "NVBDataSource.h"

//----------------
// First, multi-purpose functions

/// Get a slice at defined indexes
void sliceNArray(const double * const data, axisindex_t n, double * target, axisindex_t m, const axissize_t * sizes, const axisindex_t * sliceaxes, const axissize_t * slice, const axisindex_t * newaxes);

/// Transform data using a transform function
double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
						void (*transform)(const double *, axisindex_t, const axissize_t *, axisindex_t, const axissize_t *, double * ) );

//
//----------------

//---------------
// Now some helper functions

axissize_t prod(axisindex_t n, const axissize_t * numbers);
axissize_t subprod(const axissize_t * numbers, axisindex_t m, const axisindex_t * ixs);

QVector<axisindex_t> targetaxes(axisindex_t n, QVector<axisindex_t> sliceaxes);
QVector<axissize_t> subvector(QVector<axissize_t> sizes, QVector<axisindex_t> sliceaxes);

/// Reorder array axes
double* reorderNArray(const double * data, axisindex_t n, const axissize_t * sizes, const axisindex_t * neworder);

/// average data arrray
void average(const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t nc, const axissize_t * coords, double * target);

/// Get a slice of NVBDataSet. The ownership of the slice is passed to the calling function.
double * sliceDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> sliceixs, QVector<axisindex_t> targetaxes = QVector<axisindex_t>());

double * averageDataSet(const NVBDataSet & data, QVector<axisindex_t> axes);


#endif
