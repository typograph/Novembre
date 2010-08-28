#include "NVBDataTransform.h"

double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
																							const NVBDataTransform & transform ) {
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

NVBDataSet * NVBDataTransform::transformDataSet(NVBDataSource * source, const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axisindex_t> targetaxes = QVector<axisindex_t>()) {
	double * tdata = transformNArray(data->data(),data->nAxes(),data->sizes().data(),sliceaxes.count(),sliceaxes.data(),targetaxes.data(),as.count(),resultSize.data(),*this);
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
void NVBDataTransform::operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axissize_t * slice, double * target) const {
	axissize_t sza = prod(n,sizes);
	for (axisindex_t i = 0; i<sza; i++)
		target[i] = singleValueTransform(data[i]);
}
