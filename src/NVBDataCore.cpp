#include <stdlib.h>

#include "NVBDataCore.h"

// More laconic version would be this:
// void sliceNArray(const double * const data, axisindex_t n, double * target, axisindex_t m, const axissize_t * sizes, const axissize_t * slice, const axisindex_t * newaxes = 0);

/**
 * @fn sliceNArray
 *
 * Populates a preallocated piece of memory with data from an N-dimensional array
 *
 * @param data The original array
 * @param n The number of axes in @a data
 * @param target The resulting array
 * @param m The number of axes in @a target
 * @param sizes The sizes of the @a n axes in @a data
 * @param sliceaxes The indexes of the slice axes in the order they are in @a data
 * @param slice The indexes on the sliced axes (in the order they are in @a data)
 * @param newaxes The indexes of the original axes in the order they will be in @a target
 *
 * Example (NumPy notation):
 * @code
 * sliceNArray(d,4,t,2,[2,3,4,5],[0,3],[0,1],[2,1]) == d[0,:,:,1].T
 * @endcode
 */
void sliceNArray(const double * const data, axisindex_t n, double * target, axisindex_t m, const axissize_t * sizes, const axisindex_t * sliceaxes, const axissize_t * slice, const axisindex_t * newaxes) {
	// The collect-map of sizes
	axissize_t * mn;

	mn = (axissize_t*) calloc(n,sizeof(axissize_t));
	mn[0] = 1;
	for (axisindex_t i = 1; i < n; i++)
		mn[i] = mn[i-1]*sizes[i-1];

	// The coefficients in front of indexes for data and target
	axissize_t * ad, * at;

	ad = (axissize_t*) calloc(m,sizeof(axissize_t));
	for(axisindex_t i = 0; i < m; i++)
		ad[i] = mn[newaxes[i]];

	at = (axissize_t*) calloc(m,sizeof(axissize_t));
	at[0] = 1;
	for(axisindex_t i = 1; i < m; i++)
		at[i] = at[i-1]*sizes[newaxes[i-1]];

	// The indexes
	axissize_t * ixs;

	ixs = (axissize_t*) calloc(m,sizeof(axissize_t));

	// The index for sliced axes

	// The shift due to slicing
	axissize_t dshift = 0;

	for(axisindex_t i = 0; i < n-m; i++)
		dshift += slice[i]*mn[sliceaxes[i]];

	///// Preparations finished

	// helper indexes
	axissize_t indD = 0, indT = 0;

/*

 "Clever" code

 We need to cycle an array of indexes in a single cycle.

 So, the <for> loop restarts the cycle if one of the indexes
 can just be incremented. All preceding indexes will reset.

 Now, if we come to the last index, and it makes a full cycle,
 it means we are done, and the <for> loop ends the "natural" way.

*/
	
sliceNArray_cycle:
	target[indT] = data[indD + dshift];

	for(axisindex_t i = 0; i < m; i++)
		if (ixs[i] == sizes[newaxes[i]] - 1) {
			ixs[i] = 0;
			indD -= ad[i]*(sizes[newaxes[i]]-1);
			indT -= at[i]*(sizes[newaxes[i]]-1);
			}
		else {
			ixs[i] += 1;
			indD += ad[i];
			indT += at[i];
			goto sliceNArray_cycle;
			}

///// Slicing done
	free(mn);
	free(ixs);
	free(ad);
	free(at);
	return;

}

/**
 * \internal
 * \fn prod
 *
 * Calculates a product of numbers in an integer array \a numbers of size \a n
 *
 */

axissize_t prod(axisindex_t n, const axissize_t * numbers) {
	register axissize_t r = 1;
	for(axisindex_t i=0; i<n; i++)
		r *= numbers[i];
	return r;
}

/**
 * \internal
 * \fn subprod
 *
 * Calculates a product of \a m numbers in an integer array \a numbers with indexes from \a ixs
 *
 */

axissize_t subprod(const axissize_t * numbers, axisindex_t m, const axisindex_t * ixs) {
	register axissize_t r = 1;
	for(axisindex_t i=0; i < m; i++)
		r *= numbers[ixs[i]];
	return r;
}

/**
 * @fn transformNArray
 *
 * Feeds slices of @a data to @a transform. Returns a joined array of slices
 *
 * \param data The array to be processe
 * \param n Number of axes in this array
 * \param sizes Dimensions of the array
 * \param m Number of axes that stay
 * \param sliceaxes Indexes of axes that stay
 * \param targetaxes Indexes of axes that are passed to \a transform
 * \param p Number of new axes
 * \param newsizes Sizes of the piece after \a transfrom
 * \param transform The transformation function
 *
 * \return The result of the transforms
 *
 * \sa transform
 */

/**
 * \fn transform
 *
 * The transformation function should move and transform the data between two arrays.
 * Definition: \code
 * void transform(const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t nc, const axissize_t * coords, double * target)
 * \endcode
 *
 * \param data The chunk to work on
 * \param n The number of axes in \a data
 * \param sizes The sizes of axes in \a data
 * \param nc The number of extra coordinates
 * \param coords The coordinates of the slice
 * \param target The array to kepp results in
 */

double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
						void (*transform)(const double *, axisindex_t, const axissize_t *, axisindex_t, const axissize_t *, double * ) ) {

	axissize_t szd = prod(n,sizes);
	axissize_t szt = subprod(sizes,n-m,targetaxes);
	axissize_t szp = prod(p,newsizes);
	
	double * result;

	result = (double *) malloc (szd*szp/szt*sizeof(double));

	if (result == 0) return 0;
	
	double * target;

	target = (double *) malloc (szt*sizeof(double));

	if (target == 0) {
		free(result);
		return 0;
		}

	axissize_t * targetsizes = (axissize_t*)malloc((n-m)*sizeof(axissize_t));

	if (targetsizes == 0) {
		free(result);
		free(target);
		return 0;
		}

	for(axisindex_t i = 0; i < n - m; i++)
		targetsizes[i] = sizes[targetaxes[i]];

	axissize_t indR = 0;
	axissize_t * slice = (axissize_t*)calloc(m,sizeof(axissize_t));

	if (slice == 0) {
		free(result);
		free(target);
		free(targetsizes);
		return 0;
		}

transformNArray_cycle:
	sliceNArray(data, n, target, n-m, sizes, sliceaxes, slice, targetaxes);
	transform(target,n-m,targetsizes,m,slice,result + indR);
	indR += szp;

	for(axisindex_t i = 0; i < m; i++)
		if (slice[i] == sizes[sliceaxes[i]] - 1)
			slice[i] = 0;
		else {
			slice[i] += 1;
			goto transformNArray_cycle;
			}

	free(slice);
	free(targetsizes);
	free(target);

	return result;
}

/**
 * \fn average
 *
 * A transform-type function, that averages all the provided data. Output shape (0).
 *
 * \sa transform
 * \sa transfromNArray
 */

void average(const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t nc, const axissize_t * coords, double * target) {
	double r = 0;
	axissize_t sz = prod(n,sizes);
	for (axisindex_t i = 0; i < sz; i++)
		r += data[i];

	*target = r/sz;
}

/**
	* \fn averageDataSet
	*/

double * averageDataSet(const NVBDataSet & data, QVector<axisindex_t> axes) {
//	axissize_t szd = prod(n,sizes);
// 	axissize_t szt = subprod(sizes,n-m,targetaxes);
	QVector<axissize_t> dsizes = data->sizes();

	QVector<axisindex_t> tgaxes = targetaxes(dsizes.count(),sliceaxes);

 	axissize_t szp = subprod(dsizes.constData(),tgaxes.count(),tgaxes.constData());

	double * tdata = (double *) malloc (szp*sizeof(double));

	sliceNArray(
		data->data(),dsizes.count(),
		tdata,dsizes.count()-sliceaxes.count(),
		dsizes().constData(),
		sliceaxes.constData(),
		sliceixs.constData(),
		tgaxes.constData()
		);

	return tdata;

	transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
									axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
									axisindex_t p, const axissize_t * newsizes,	average);
return 0;
}

/**
 * \fn reorderNArray
 *
 * Changes the order of axes in an array
 *
 * \param data The original array
 * \param n The number of array dimensions
 * \param sizes The shape of \a data
 * \param neworder The new indexes of old axes
 * \return The transformed array
 */
double* reorderNArray(const double * data, axisindex_t n, const axissize_t * sizes, const axisindex_t * neworder) {
	double * r = (double*) malloc(prod(n,sizes)*sizeof(double));
	sliceNArray(data,n,r,n,sizes,0,0,neworder);
	return r;
}

QVector<axisindex_t> targetaxes(axisindex_t n, QVector<axisindex_t> sliceaxes) {
	QVector<axisindex_t> tgaxes;
	for (axisindex_t j = 0, axisindex_t i = 0; i<n; i++) {
		if (j >= sliceaxes.count() || sliceaxes.at(j) > i)
			tgaxes << i;
		else
			j+=1;
		}
	return tgaxes;
}

QVector<axissize_t> subvector(QVector<axissize_t> sizes, QVector<axisindex_t> sliceaxes) {
	QVector<axissize_t> tgsizes;
	foreach(axisindex_t j, sliceaxes)
		tgsizes << sizes.at(j);
	return tgsizes;
}

double * sliceDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> sliceixs, QVector<axisindex_t> targetaxes = QVector<axisindex_t>()) {
//	axissize_t szd = prod(n,sizes);
// 	axissize_t szt = subprod(sizes,n-m,targetaxes);
	QVector<axissize_t> dsizes = data->sizes();

	QVector<axisindex_t> tgaxes = targetaxes(dsizes.count(),sliceaxes);

 	axissize_t szp = subprod(dsizes.constData(),tgaxes.count(),tgaxes.constData());

	double * tdata = (double *) malloc (szp*sizeof(double));

	sliceNArray(
		data->data(),dsizes.count(),
		tdata,dsizes.count()-sliceaxes.count(),
		dsizes().constData(),
		sliceaxes.constData(),
		sliceixs.constData(),
		tgaxes.constData()
		);
		
	return tdata;
}
