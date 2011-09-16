#include "NVBDataTransform.h"

/**
	\class NVBDataTransform

	NVBDataTransform is to be an alternative to the old NVBFileDelegate interface.
	It goes on supposing that most data-altering filters act on slices of data
	in the same way. Thus, you just have to define a transform operation for
	an individual slice (which may be slice coordinate-dependent).

	This should make the work of implementing a filter less cumbersome, as simple
	operations like plane subtraction or averaging are straightforward. The main
	bottleneck for plugin implementation is now the act of applying the transform
	to NVBDataSource.

	NVBDataSource is a curious beast. Basically, it brings together interdependent
	data; interdependent in the sense that they have the same position in physical
	world and share axes (and also all info that goes with them). The data within
	a datasource must change together. Say we add a color map to the curve position axis.
	This map affects all data that depends on this axis. Or we lower the resolution
	of the data, by doing a median 2x2, that changes the length of one (or more)
	of our axes. This change again affects all data in the source.

	On the other hand, the user doesn't care so much about the data being interconnected.
	If we subtract a plane from backward topography, we do not expect the forward topography
	to have the same plane subtracted, and, supposedly, we want our spectroscopy, taken
	on the same axes, intact.

	Most of the transforms, however, seem to be reasonably immune to this problem.
	E.g. 3-point plane subtraction. It is true that the plane subtracted from forward
	and backward topographies must be different, but the 3 points selected for subtraction
	are, most likely, the same. Radial averaging, FFT transforms or profile extraction can
	(and should) easily be done on all related datasets (read: on all datasets in a datasource).

	This decision has a backlash. though -- only related datasets should be in datasource.
	I whould like to have more freedom and connections -- if topography _and_ spectroscopy
	were really taken at the same points, there is no real reason to have two datasources.

	Then beware of TheBigQuestion : imagine we have f/b topography and I(U) spectroscopy
	on a 128 x 128 grid at 5 different temperatures. Now a crazy scientist does radial average.
	Which datasets should feel the pain? Radial average collapses x & y axes into an R axis,
	which is not even uniform.
	
	Possible toptions include:
	1) All datasets get transformed. There are 3 axes now - R, T and U. (fun ensues, but this has physical sense)
	2) Only topography is transformed. This produces a splitting of the datasource into two (not nice)
	3) Only topography is transformed. This increases the number of axes by 1.
	[ versions (2) and (3) have a problem of axis continuity. If the user wants to radial average spectroscopy, too,
	  it will end up on a different axis, and so are less bonded for all future transforms.]
	4) New sets are created. This increases the number of axes by 1. (if the transform deletes display axes,
	 the datasource from option (1) will anyway open in a different window, with the same effect on user)

	The most reasonable solution seems to be #1, especially if applying this transform creates a new
	display window (it should anyway -- the old data was (2+1+1)D, the new is (1+1+1)D. It might
	make sense to plot it on 2D, but this is user's responsiblity. Actually, Novembre should probably
	autocreate a new FileWindow if a filter removes one of the display axes.

	OK, different filter. Let's say we do plane subtract. The number of axes stays the same, no new display
	window. How does it affect different datasets?

	Possible options are:
	1) Only active dataset is changed. This is the "universally accepted" solution.
	2) All topography (i.e. data with the same set of axes) is changed (acceptable, but not if there are
			more topography-like datasets that are not topography (e.g. dI/dU map), since that might have
			a different plane, different 3-points, or have no need for line median subtraction.
	3) All datasets on display axes change. The spectroscopy is a mess.

	So, we need the idea of mutating datasets... The big question is how to relate it to the mutating datasources.
	It should probably be the same mechanism. And then there is the correct undo sequence. One possible
	solution is to put the transform on all datasets, but only enable it for some of them. The user can then
	enable the others himself.

	Then, let's try to describe the transform system in its entirety.

	1) There is the initial datasource. We don't care where is comes from.
	
	2) In case we are in a FileWindow, we also have access to active dataset and active axes parameters.
	But in case the transform is applied elsewhere (e.g. in browser) these parameters have to get default
	values (like no active dataset and the first couple of active axes. This decision rests on the author
	of the transform plugin (TODO : see if the base class could provide some reasonable defaults).

	3) The transform applies to all datasets in the initial datasource, but excluding the ones that are impossible
	to use with this transform (e.g. not enough axes). Additionally some of the datasets are excluded by a mask.
	The default values for the mask are also the plugin's responsiblity (simplest case - all datasets with the same
	set of axes).

	4) The result of the operation is a datasource again. The initial one gets pushed.

	*) There is a possibility to get a control widget out of the datasource. This one is used by the 
	filewindow to get the user the ability to control the transform (TODO : decide if there is only one widget per
	FileWindow for all datasets or many - the distinction can be made by active dataset. Maybe the widget really
	exists only once - to make all changes syncronous - with the activity/inactivity controlled by the WidgetStack)

/////// to be revisited ///////

	Constructors:
		NVBDataSource();
		NVBWrapDataSource(NVBDataSource * source, NVBTransform * transform)
		NVBPatchworkDataSource(NVBDataSource * source);

	Modifiers (that might or might not produce a new datasource):
		NVBDataSource * NVBDataSource::transformData(NVBTransform * transform);
		NVBDataSource * NVBDataSource::transformDataSet(NVBDataSet * dataset, NVBTransform * transform);
		NVBDataSource * NVBDataSource::addAxisMap(NVBAxisMap * map, int axis);
		NVBDataSource * NVBDataSource::removeAxisMap(NVBAxisMap * map);
		NVBDataSource * NVBDataSource::addColorMap(NVBColorMap * map, int dataset);
		NVBDataSource * NVBDataSource::removeColorMap(NVBColorMap * map);

		NVBDataSet * NVBDataSet::transform(NVBTransform * transform);
 */

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
