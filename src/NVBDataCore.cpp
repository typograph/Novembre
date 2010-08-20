#include <stdlib.h>

#include "NVBDataCore.h"

// More laconic version would be this:
// void sliceNArray(const double * const data, int n, double * target, int m, const int * sizes, const int * slice, const int * newaxes = 0);

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
void sliceNArray(const double * const data, int n, double * target, int m, const int * sizes, const int * sliceaxes, const int * slice, const int * newaxes) {
	// The collect-map of sizes
	int * mn;

	mn = (int*) calloc(n,sizeof(int));
	mn[0] = 1;
	for (int i = 1; i < n; i++)
		mn[i] = mn[i-1]*sizes[i-1];

	// The coefficients in front of indexes for data and target
	int * ad, * at;

	ad = (int*) calloc(m,sizeof(int));
	for(int i = 0; i < m; i++)
		ad[i] = mn[newaxes[i]];

	at = (int*) calloc(m,sizeof(int));
	at[0] = 1;
	for(int i = 1; i < m; i++)
		at[i] = at[i-1]*sizes[newaxes[i-1]];

	// The indexes
	int * ixs;

	ixs = (int*) calloc(m,sizeof(int));

	// The index for sliced axes

	// The shift due to slicing
	int dshift = 0;

	for(int i = 0; i < n-m; i++)
		dshift += slice[i]*mn[sliceaxes[i]];

	///// Preparations finished

	// helper indexes
	int indD = 0, indT = 0;

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

	for(int i = 0; i < m; i++)
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

int prod(int n, const int * numbers) {
	register int r = 1;
	for(int i=0; i<n; i++)
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

int subprod(const int * numbers, int m, const int * ixs) {
	register int r = 1;
	for(int i=0; i < m; i++)
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
 * void transform(const double * data, int n, const int * sizes, int nc, const int * coords, double * target)
 * \endcode
 *
 * \param data The chunk to work on
 * \param n The number of axes in \a data
 * \param sizes The sizes of axes in \a data
 * \param nc The number of extra coordinates
 * \param coords The coordinates of the slice
 * \param target The array to kepp results in
 */

double * transformNArray(const double * data, int n, const int * sizes,
																							int m, const int * sliceaxes, const int * targetaxes,
																							int p, const int * newsizes,
						void (*transform)(const double *, int, const int *, int, const int *, double * ) ) {

	int szd = prod(n,sizes);
	int szt = subprod(sizes,n-m,targetaxes);
	int szp = prod(p,newsizes);
	
	double * result;

	result = (double *) malloc (szd*szp/szt*sizeof(double));

	if (result == 0) return 0;
	
	double * target;

	target = (double *) malloc (szt*sizeof(double));

	if (target == 0) {
		free(result);
		return 0;
		}

	int * targetsizes = (int*)malloc((n-m)*sizeof(int));

	if (targetsizes == 0) {
		free(result);
		free(target);
		return 0;
		}

	for(int i = 0; i < n - m; i++)
		targetsizes[i] = sizes[targetaxes[i]];

	int indR = 0;
	int * slice = (int*)calloc(m,sizeof(int));

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

	for(int i = 0; i < m; i++)
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

double * transformNArray(const double * data, int n, const int * sizes,
																							int m, const int * sliceaxes, const int * targetaxes,
																							int p, const int * newsizes,
																							const NVBDataTransform & transform ) {
	int szd = prod(n,sizes);
	int szt = subprod(sizes,n-m,targetaxes);
	int szp = prod(p,newsizes);

	double * result;

	result = (double *) malloc (szd*szp/szt*sizeof(double));

	if (result == 0) return 0;

	double * target;

	target = (double *) malloc (szt*sizeof(double));

	if (target == 0) {
		free(result);
		return 0;
		}

	int * targetsizes = (int*)malloc((n-m)*sizeof(int));

	if (targetsizes == 0) {
		free(result);
		free(target);
		return 0;
		}

	for(int i = 0; i < n - m; i++)
		targetsizes[i] = sizes[targetaxes[i]];

	int indR = 0;
	int * slice = (int*)calloc(m,sizeof(int));

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

	for(int i = 0; i < m; i++)
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

void average(const double * data, int n, const int * sizes, int nc, const int * coords, double * target) {
	double r = 0;
	int sz = prod(n,sizes);
	for (int i = 0; i < sz; i++)
		r += data[i];

	*target = r/sz;
}

/**
	* \fn averageDataSet
	*/

double * averageDataSet(const NVBDataSet & data, QVector<quint16> axes) {
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
double* reorderNArray(const double * data, int n, const int * sizes, const int * neworder) {
	double * r = (double*) malloc(prod(n,sizes)*sizeof(double));
	sliceNArray(data,n,r,n,sizes,0,0,neworder);
	return r;
}

NVBDataSet * NVBDataTransform::transformDataSet(NVBDataSource * source, const NVBDataSet * data, QVector<quint16> sliceaxes, QVector<quint16> targetaxes = QVector<quint16>()) {
	double * tdata = transformNArray(data->data(),data->numberOfAxes(),data->sizes().data(),sliceaxes.count(),sliceaxes.data(),targetaxes.data(),as.count(),resultSize.data(),*this);
	NVBDataSet target = new NVBDataSet(source,data->name(),tdata,0);
}

/** \fn NVBDataTransform::operator()
	*
	* The function that is passed to \a transformNArray.
	* \sa transform
	*
	* Redefine in subclasses to provide custom behaviour.
	* The default implementation applies \a singleValueTransform to every element.
	*/
void NVBDataTransform::operator() (const double * data, int n, const int * sizes, int m, const int * slice, double * target) const {
	int sza = prod(n,sizes);
	for (int i = 0; i<sza; i++)
		target[i] = singleValueTransform(data[i]);
}
