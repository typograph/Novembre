#ifndef NVBDATACORE_H
#define NVBDATACORE_H

#include <QtCore/QVector>
#include <QtGui/QColor>
#include "NVBDataGlobals.h"
#include "NVBLogger.h"

class NVBDataSet;

// #include "NVBDataSource.h"

//----------------
// First, multi-purpose functions

/*
 * This function is incomplete.
 *
 * One of the things is that the function to be called has to be called
 * with an array of variables, f([a,b,c]) instead of more natural f(a,b,c).
 * The latter, however, requires some platform-dependent (?) assembly code,
 * and I don't know how to do it.
 *
 * Second thing is that this function has to be called with integer parameters.
 * The <logical> thing would be to call it with double or NVBPhysValue. But
 * this depends so strongly on axismaps available (e.g., what to do with 2D?),
 * that it pains me to think about it.
 *
 * Therefore I leave it incomplete.
 */

typedef double (* FillFunc)(axisindex_t, const axissize_t * );

/// Fill the array with values from a multi-dim. function
void fillNArray(double * data, axisindex_t n, const axissize_t * sizes, FillFunc);

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

class NVBDataSet;

/// Get a slice of NVBDataSet. The ownership of the slice is passed to the calling function.
double * sliceDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> sliceixs, QVector<axisindex_t> targetaxes = QVector<axisindex_t>());

double * averageDataSet(const NVBDataSet * data, QVector<axisindex_t> axes);

struct NVBDataSlice {
	/// The dataset that is sliced
	const NVBDataSet * dataset;
	/// Data block of the current slice
	double * data;
	/// Indices of sliced axes of \a dataset
	const QVector<axisindex_t> slicedAxes;
	/// Indices of remaining axes of \a dataset
	const QVector<axisindex_t> sliceAxes;
	/// Indexes of the slice on \a slicedAxes
	QVector<axissize_t> indexes;
	/// Sizes of \a slicedAxes
	QVector<axissize_t> sizes;

	NVBDataSlice(const NVBDataSet * dataset, const QVector<axisindex_t> & sliced, const QVector<axisindex_t> & kept);
	~NVBDataSlice() { if(data) free(data); }

	/// Slices the dataset at \a indexes
	void calculate() ;

	QColor associatedColor() const;
	QVector<axissize_t> parentIndexes() const;
};

class NVBSliceCounter {
	const NVBDataSet * dset;
	bool is_running;
	NVBDataSlice slice;

	public:
		NVBSliceCounter(const NVBDataSet * dataset, const QVector<axisindex_t> & sliced, const QVector<axisindex_t> & kept = QVector<axisindex_t>());

		~NVBSliceCounter() ;

		static bool stepIndexVector(QVector<axissize_t> & ixs, const QVector<axissize_t> & sizes, int step) ;
		
		void stepIndexVector(int step = 1);
		inline bool counting() { return is_running; }

		inline const NVBDataSlice & getSlice() const { return slice; }
};

class NVBSliceSingle {
	NVBDataSlice * slice;

	public:
		NVBSliceSingle(const NVBDataSet * dataset, const QVector<axisindex_t> & sliced, const QVector<axisindex_t> & kept = QVector<axisindex_t>()) ;

		~NVBSliceSingle();

		inline bool hasSlice() { return slice != 0; }
		inline void killSlice() {if (slice) { delete(slice); slice = 0;} }
		inline const NVBDataSlice & getSlice() const { return *slice; }
};

/**
 *
 * @def forEachSlice(dataset,slicedaxes,keptaxes)
 *
 * Runs a cycle on all slices in dataset on slicedaxes. Variable \a SLICE
 * of type NVBDataSlice is available inside the cycle.
 */

#define forEachSlice(dataset,sliced,kept) \
	for(NVBSliceCounter _counter(dataset,sliced,kept); _counter.counting(); _counter.stepIndexVector())

/**
 *
 * @def forNSlices(dataset,N,slicedaxes,keptaxes)
 *
 * Runs a cycle on N slices in dataset on slicedaxes. Variable \a SLICE
 * of type NVBDataSlice is available inside the cycle.
 */

#define forNSlices(dataset,N,sliced,kept) \
	for(NVBSliceCounter _counter(dataset,sliced,kept); _counter.counting(); _counter.stepIndexVector(subprod(dataset->sizes().constData(),sliced.count(),sliced.constData())/N))

/**
 *
 * @def forSingleSlice(dataset,slicedaxes,keptaxes)
 *
 * Calculates a single slice in dataset on slicedaxes. Variable \a SLICE
 * of type NVBDataSlice is available inside the cycle.
 */
#define forSingleSlice(dataset,sliced,kept) \
	for(NVBSliceSingle _counter(dataset,sliced,kept); _counter.hasSlice(); _counter.killSlice())
		
#define SLICE _counter.getSlice()

#endif
