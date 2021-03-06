//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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

/// A functional dataset - returns a double for a multi-dimensional index
typedef double (* FillFunc)(axisindex_t, const axissize_t * );

/// Fill an array with values from a multi-dimensional function
void fillNArray(double * data, axisindex_t n, const axissize_t * sizes, FillFunc);

/// Get a slice at defined indexes (using pre-allocated buffer)
void sliceNArray(const double * const data, axisindex_t n, double * target, axisindex_t m, const axissize_t * sizes, const axisindex_t * sliceaxes, const axissize_t * slice, const axisindex_t * newaxes);

/// Get a slice at defined indexes (memory allocated by function)
double * sliceNArray(const double * const data, axisindex_t n, axisindex_t m, const axissize_t * sizes, const axisindex_t * sliceaxes, const axissize_t * slice, const axisindex_t * newaxes = 0);

/// Get a slice at defined indexes (QVector) (using pre-allocated buffer)
void sliceNArray(const double * const data, double * target, QVector<axissize_t> sizes, QVector<axisindex_t> sliceaxes, QVector<axissize_t> slice, QVector<axisindex_t> newaxes = QVector<axisindex_t>());

/// Get a slice at defined indexes (QVector) (memory allocated by function)
double * sliceNArray(const double * const data, QVector<axissize_t> sizes, QVector<axisindex_t> sliceaxes, QVector<axissize_t> slice, QVector<axisindex_t> newaxes = QVector<axisindex_t>());

/// Get a slice of NVBDataSet. The ownership of the slice is passed to the calling function.
double * sliceDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> sliceixs, QVector<axisindex_t> targetaxes = QVector<axisindex_t>());

typedef void (*transformFunc)(const double *, axisindex_t, const axissize_t *, axisindex_t, const axissize_t *, double * );

/// Transform data using a transform function
double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
                         axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
                         axisindex_t p, const axissize_t * newsizes,
                         transformFunc transform  );

//
//----------------

//---------------
// Now some helper functions

/// Calculates the product of all values in the array
axissize_t prod(axisindex_t n, const axissize_t * numbers);
/// Calculates the product of a subset of values in the array
axissize_t subprod(const axissize_t * numbers, axisindex_t m, const axisindex_t * ixs);

/// Calculates the product of all values in a QVector
inline axissize_t prod(QVector<axissize_t> numbers) { return prod(numbers.count(), numbers.constData()); }

/// Removes duplicate values from QList or QVector
template< typename Container >
void uniquify( Container & list ) {
	qSort(list);

	for(int i = 1; i < list.count(); i++) {
		if (list.at(i) == list.at(i - 1))
			list.removeAt(--i);
		}
	}

/// Returns complimentary axes for given \a sliceaxes
QVector<axisindex_t> targetaxes(axisindex_t n, QVector<axisindex_t> sliceaxes);
/// Returns a subset of \a sizes, selected at \a sliceaxes
QVector<axissize_t> subvector(QVector<axissize_t> sizes, QVector<axisindex_t> sliceaxes);

/// Reorders array axes
double* reorderNArray(const double * data, axisindex_t n, const axissize_t * sizes, const axisindex_t * neworder);

/// Averages all data in array \a data (function of type transformFunc)
void average(const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t nc, const axissize_t * coords, double * target);

/// Averages a dataset along \a axes and returns an array of double
double * averageDataSet(const NVBDataSet * data, QVector<axisindex_t> axes);

/**
 * \class NVBDataSlice
 * 
 * \brief The type of slice-loop variable
 * 
 */
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

	/// Creates a slice of \a dataset. The \a sliced axes are no longer in the result
	NVBDataSlice(const NVBDataSet * dataset, const QVector<axisindex_t> & sliced, const QVector<axisindex_t> & kept);
	~NVBDataSlice();

	/// Slices the dataset at \a indexes
	void calculate() ;

	/// The color, associated with the slice (one or more of the sliced axes has to have color maps)
	QColor associatedColor() const;
	///
	QVector<axissize_t> parentIndexes() const;
	};

class NVBSliceCounter {
		const NVBDataSet * dset;
		bool is_running;
		int step;
		NVBDataSlice slice;

		Q_DISABLE_COPY(NVBSliceCounter);

	public:
		NVBSliceCounter(const NVBDataSet * dataset, const QVector<axisindex_t> & sliced, const QVector<axisindex_t> & kept = QVector<axisindex_t>(), int maxCount = -1);

		~NVBSliceCounter();

		static bool stepIndexVector(QVector<axissize_t> & ixs, const QVector<axissize_t> & sizes, int step) ;

		void stepIndexVector();
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
	for(NVBSliceCounter _counter(dataset,sliced,kept,N); _counter.counting();_counter.stepIndexVector())

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
