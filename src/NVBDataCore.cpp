#include <stdlib.h>

#include "NVBDataCore.h"
#include "NVBDataSource.h"
#include "NVBMap.h"

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
 * @param sliceaxes The indexes of the slice axes in any order
 * @param slice The indexes on the sliced axes (in the same order as @a sizes)
 * @param newaxes The indexes of the original axes in the order they will be in @a target
 *
 * Example (NumPy notation):
 * @code
 * sliceNArray(d,4,t,2,[2,3,4,5],[0,3],[0,1],[2,1]) == d[0,:,:,1].T
 * @endcode
 */
void sliceNArray(const double * const data, axisindex_t n, double * target, axisindex_t m, const axissize_t * sizes, const axisindex_t * sliceaxes, const axissize_t * slice, const axisindex_t * newaxes) {
	
	if (!target) {
		NVBOutputError("NULL target pointer");
		return;
		}

	if (!data) {
		NVBOutputError("NULL data pointer");
		return;
		}

	if (n != m && (!sliceaxes || !slice)) {
		NVBOutputError("NULL sliceaxes/slice pointer");
		return;
		}

	if (m != 0 && !newaxes) {
		NVBOutputError("NULL newaxes pointer");
		return;
		}

	if (m > n) {
		NVBOutputError("Target has more axes than parent");
		return;
		}

#ifdef NVB_DEBUG // we can't check this every time when called from transformNArray

	for(axisindex_t i = 0; i<n-m;i++) {
		if (slice[i] >= sizes[sliceaxes[i]]) {
			NVBOutputError("Slice index outside range");
			return;
			}
		}

	bool * axix = (bool*)calloc(n,sizeof(bool));

	if (axix) {
		for(axisindex_t i = 0; i<m;i++) {
			if (axix[newaxes[i]]) {
				NVBOutputError(QString("Axis %1 used two times").arg(newaxes[i]));
				return;
				}
			else
				axix[newaxes[i]] = true;
			}
		
		for(axisindex_t i = 0; i<n-m;i++) {
			if (axix[sliceaxes[i]]) {
				NVBOutputError(QString("Axis %1 used two times").arg(sliceaxes[i]));
				return;
				}
			else
				axix[sliceaxes[i]] = true;
			}	
		
		free(axix);
		}
	else
		NVBOutputError("Not enough memory to check axes indexes");
	
#endif
	
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
 * @fn fillNArray
 *
 * Populates a preallocated piece of memory with data from an N-dimensional function
 *
 * @param data The original array
 * @param n The number of axes in @a data
 * @param sizes The sizes of the @a n axes in @a data
 * @param f The function of type f(int, const int*)
 *
 */
void fillNArray(double * data, axisindex_t n, const axissize_t * sizes, FillFunc f) {
	// The indexes
	axissize_t * ixs = (axissize_t*) calloc(n,sizeof(axissize_t));

	// index of data
	axissize_t indD = 0;

fillNArray_cycle:
	data[indD] = f(n,ixs);
	indD += 1;
	
	for(axisindex_t i = 0; i < n; i++) {
		if (ixs[i] == sizes[i] - 1)
			ixs[i] = 0;
		else {
			ixs[i] += 1;
			goto fillNArray_cycle;
			}
		}

///// Slicing done
	free(ixs);
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
 * \param data The array to be processed
 * \param n Number of axes in @a data
 * \param sizes Dimensions of the array (array of size \a n)
 * \param m Number of axes that stay unprocessed (those remain in the result)
 * \param sliceaxes Indexes of axes that stay (array of size \a m), in order they should appear in the result
 * \param targetaxes Indexes of axes that are passed to \a transform (array of size \a n-m), in order they should be for the transform function
 * \param p Number of new axes, as returned from @a transform function
 * \param newsizes Sizes of the piece after \a transfrom (array of size \a p)
 * \param transform The transformation function
 *
 * \return The result of the transform
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
 * \param sizes The sizes of axes in \a data (array of size \a n)
 * \param nc The number of slice coordinates
 * \param coords The coordinates of the slice (array of size \a nc)
 * \param target The array to keep results in (managed by calling function, size depends on transform)
 *
 * Example: Data copy\code
 *	axissize_t sz = prod(n,sizes);
 *	for (axisindex_t i = 0; i < sz; i++)
 *		target[i] = data[i];
 * \endcode
 *
 * Example: Normalization by first slice coordinate\code
 *	if (nc < 1) return;
 *	axissize_t sz = prod(n,sizes);
 *	for (axisindex_t i = 0; i < sz; i++)
 *		target[i] = data[i]/sqrt(coords[0]);
 * \endcode
 *
 * Tecnically, slice coordinates could be used to pass parameters to the transform function.
 * However, this supposes a custom \a transformNArray.
 * TODO Damn it, I can't think of any example which increases number of axes or uses slice coordinates ////
 */

double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
						void (*transform)(const double *, axisindex_t, const axissize_t *, axisindex_t, const axissize_t *, double * ) ) {

	if (!data) {
		NVBOutputError("NULL data pointer");
		return 0;
		}

	if (m > n) {
		NVBOutputError("Transform needs more axes than available");
		return 0;
		}

#ifdef NVB_DEBUG

	for(axisindex_t i = 0; i<m; i++) {
		if (sliceaxes[i] >= n) {
			NVBOutputError("Slice axis index outside range");
			return 0;
			}
		}

	for(axisindex_t i = 0; i<n-m;i++) {
		if (targetaxes[i] >= n) {
			NVBOutputError("Target axis index outside range");
			return 0;
			}
		}

	bool * axix = (bool*)calloc(n,sizeof(bool));

	if (axix) {
		for(axisindex_t i = 0; i<m;i++) {
			if (axix[sliceaxes[i]]) {
				NVBOutputError(QString("Axis %1 used two times").arg(sliceaxes[i]));
				return 0;
				}
			else
				axix[sliceaxes[i]] = true;
			}
		
		for(axisindex_t i = 0; i<n-m;i++) {
			if (axix[targetaxes[i]]) {
				NVBOutputError(QString("Axis %1 used two times").arg(targetaxes[i]));
				return 0;
				}
			else
				axix[targetaxes[i]] = true;
			}	
		
		free(axix);
		}
	else
		NVBOutputError("Not enough memory to check axes indexes");
	
#endif
	
	
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

void average(const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t , const axissize_t * , double * target) {
	double r = 0;
	axissize_t sz = prod(n,sizes);
	for (axissize_t i = 0; i < sz; i++)
		r += data[i];

	*target = r/sz;
}

/**
	* \fn averageDataSet
	*/

double * averageDataSet(const NVBDataSet * data, QVector<axisindex_t> axes) {

	QVector<axissize_t> sz = data->sizes();
	
	if (axes.count() == 0) {
		axissize_t msz = prod(sz.count(),sz.constData())*sizeof(double);
		double * tdata = (double*)malloc(msz);
		memcpy(tdata,data->data(),msz);
		return tdata;
		}
	
	return transformNArray(
		data->data(), data->nAxes(), sz.constData(),
		sz.count() - axes.count(), targetaxes(data->nAxes(),axes).constData(), axes.constData(), 
		0,  0,	average);
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

void targetaxes(axisindex_t n, axisindex_t m, const axisindex_t * sliceaxes, axisindex_t * otheraxes) {
	if (!otheraxes) {
		NVBOutputError("NULL buffer");
		return;
		}
		
	axisindex_t i, j, k=0;
	for (j = 0, i = 0; i<n; i++) {
		if (j >= (n-m) || sliceaxes[j] > i)
			otheraxes[k++] = i;
		else
			j += 1;
		}
	
}

QVector<axisindex_t> targetaxes(axisindex_t n, QVector<axisindex_t> sliceaxes) {
	QVector<axisindex_t> tgaxes;
	axisindex_t i, j;
	for (j = 0, i = 0; i<n; i++) {
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

double* sliceNArray(const double * const data, axisindex_t n, axisindex_t m, const axissize_t* sizes, const axisindex_t* sliceaxes, const axissize_t* slice, const axisindex_t* newaxes)
{
	double * tdata;
	if (!newaxes) {
		axisindex_t * tgaxes = (axisindex_t *) calloc(m,sizeof(axisindex_t));
		targetaxes(n,m,sliceaxes,tgaxes);
		axissize_t szp = subprod(sizes,m,tgaxes);
		tdata = (double *) malloc (szp*sizeof(double));
		sliceNArray(data,n,tdata,m,sizes,sliceaxes,slice,tgaxes);
		free(tgaxes);
		}
	else {
		axissize_t szp = subprod(sizes,m,newaxes);
		tdata = (double *) malloc (szp*sizeof(double));
		sliceNArray(data,n,tdata,m,sizes,sliceaxes,slice,newaxes);
		}
	return tdata;
}

double* sliceNArray(const double * const data, QVector< axissize_t > sizes, QVector< axisindex_t > sliceaxes, QVector< axissize_t > slice, QVector< axisindex_t > newaxes)
{	
	if (sliceaxes.count() != slice.count()) {
		NVBOutputError("Not enough or too many slice coordinates");
		return 0;
		}
	if (!newaxes.isEmpty() && sizes.count() !=  sliceaxes.count() + newaxes.count()) {
		NVBOutputError(QString("%1 slice axes and %2 result axes don't add up to %3 parent axes").arg(sliceaxes.count()).arg(newaxes.count()).arg(sizes.count()));
		return 0;		
		}
	
	return sliceNArray(data, sizes.count(), sizes.count() - sliceaxes.count(), sizes.constData(), sliceaxes.constData(), slice.constData(), newaxes.isEmpty() ? 0 : newaxes.constData());
}

void sliceNArray(const double * const data, double * target, QVector< axissize_t > sizes, QVector< axisindex_t > sliceaxes, QVector< axissize_t > slice, QVector< axisindex_t > newaxes)
{	
	if (sliceaxes.count() != slice.count()) {
		NVBOutputError("Not enough or too many slice coordinates");
		return;
		}
	if (!newaxes.isEmpty() && sizes.count() !=  sliceaxes.count() + newaxes.count()) {
		NVBOutputError(QString("%1 slice axes and %2 result axes don't add up to %3 parent axes").arg(sliceaxes.count()).arg(newaxes.count()).arg(sizes.count()));
		return;		
		}
	
	if (newaxes.isEmpty())
		newaxes = targetaxes(sizes.count(),sliceaxes);
	
	sliceNArray(data, sizes.count(), target, sizes.count() - sliceaxes.count(), sizes.constData(), sliceaxes.constData(), slice.constData(), newaxes.constData());
}


double * sliceDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> sliceixs, QVector<axisindex_t> tgaxes) {
//	axissize_t szd = prod(n,sizes);
// 	axissize_t szt = subprod(sizes,n-m,targetaxes);

	if (!data) return 0;

	return sliceNArray( data->data(), data->sizes(), sliceaxes, sliceixs, tgaxes);
		
}

NVBSliceCounter::NVBSliceCounter(const NVBDataSet* dataset, const QVector< axisindex_t >& sliceaxes, const QVector< axisindex_t >& tgaxes, int maxCount)
	: dset(dataset)
	, is_running(true)
	, slice(
			dataset,
			sliceaxes,
			dataset ? (tgaxes.isEmpty() ? targetaxes(dataset->nAxes(), sliceaxes) : tgaxes) : QVector<axisindex_t>())
	{
	useDataSet(dataset);
	if (!dataset) {
		NVBOutputError("NULL dataset");
		is_running = false;
		return;
		}

	if (maxCount <= 0)
		step = 1;
	else {
		step = qMax( (axissize_t)1, subprod(dataset->sizes().constData(),sliceaxes.count(),sliceaxes.constData())/maxCount);
		}


	slice.calculate();
	}
/*
NVBSliceCounter::NVBSliceCounter(const NVBSliceCounter & other)
 : dset(other.dset)
 , is_running(other.is_running)
 , slice(
			 dataset,
			 sliceaxes,
			 dataset ? (tgaxes.isEmpty() ? targetaxes(dataset->nAxes(), sliceaxes) : tgaxes) : QVector<axisindex_t>())	{
	useDataSet(dset);
	}
*/
NVBSliceCounter::~NVBSliceCounter() {
	releaseDataSet(dset);
	}

bool NVBSliceCounter::stepIndexVector(QVector< axissize_t >& ixs, const QVector< axissize_t >& sizes, int step) {
	if (step == 0) return true;

	for (axisindex_t i = 0; i < ixs.count(); i++) {
		step += ixs.at(i);
		ixs[i] = step % sizes.at(i);
		step /= sizes.at(i);
		if (step == 0) return true;
		}
/*		
		if (ixs.at(i) == sizes.at(i) - 1)
			ixs[i] = 0;
		else {
			ixs[i] += 1;
			return true;
			} */
	return false;
	}

void  NVBSliceCounter::stepIndexVector() {
	if ( (is_running = stepIndexVector(slice.indexes, slice.sizes, step)) )
		slice.calculate();
	}

/**
	* \fn NVBDataSlice::NVBDataSlice
	*
	* \param sizes
	*/
NVBDataSlice::NVBDataSlice(const NVBDataSet * ds, const QVector< axisindex_t >& sliced, const QVector< axisindex_t >& kept)
: dataset(ds)
, data(0)
, slicedAxes(sliced)
, sliceAxes(kept)
, indexes((axissize_t)sliced.count(),0)
	{
	useDataSet(dataset);
	if (!ds) NVBOutputError("Null dataset");
	sizes = subvector(ds->sizes(),sliced);
	}

NVBDataSlice::~NVBDataSlice() {
	releaseDataSet(dataset);
	if(data) free(data);
	}

void NVBDataSlice::calculate() {
	if (data) free(data);
	data = sliceDataSet(dataset, slicedAxes, indexes, sliceAxes);
	}

void operator+=(QColor a, const QColor & b) {
	if (!a.isValid())
		a = b;
	else {
		a.setBlueF((a.blueF() + b.blueF())/2);
		a.setGreenF((a.greenF() + b.greenF())/2);
		a.setRedF((a.redF() + b.redF())/2);
		}
}

QColor NVBDataSlice::associatedColor() const {
	if (!dataset) return Qt::black;

	QList<NVBAxisMapping> mappings;

	foreach(axisindex_t i, slicedAxes)
		foreach(NVBAxisMapping m, dataset->axisAt(i).maps())
			if ( m.map->mappingType() == NVBAxisMap::Color && ! mappings.contains(m))
				mappings << m;

	if (mappings.isEmpty()) return Qt::black;

	QColor c;
	QVector<axissize_t> ixs = parentIndexes();
	foreach(NVBAxisMapping m, mappings)
		c += (m.map->value(subvector(ixs,m.axes))).value<QColor>();

	return c;
	
	}

QVector<axissize_t> NVBDataSlice::parentIndexes() const {
	QVector<axissize_t> r(dataset->dataSource()->nAxes(),0);
	for (axisindex_t i = 0; i < dataset->dataSource()->nAxes(); i +=1 ) {
		axisindex_t k = dataset->indexAtParent(i);
		if (k>=0) r[i] = indexes.at(k);
		}
	return r;
}

NVBSliceSingle::NVBSliceSingle(const NVBDataSet* dataset, const QVector< axisindex_t >& sliceaxes, const QVector< axisindex_t >& tgaxes)
: slice(0)
{
	if (!dataset) 
		NVBOutputError("NULL dataset");
	else {
		slice = new NVBDataSlice(dataset, sliceaxes, dataset ? (tgaxes.isEmpty() ? targetaxes(dataset->nAxes(), sliceaxes) : tgaxes) : QVector<axisindex_t>());
		slice->calculate();
		}
}

NVBSliceSingle::~NVBSliceSingle() { killSlice(); }

