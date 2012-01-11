#ifndef NVBUSERPAGEVIEWMODEL_H
#define NVBUSERPAGEVIEWMODEL_H

#include "NVBPageViewModel.h"

/*!
* A model for showing pages in NVBFileWindow.
* Unlike NVBPageViewModel, this model is more strict about the order
* in which its pages are arranged. Concretely, spectroscopy
* pages will always be inserted on top of topography pages,
* to make sure that the spectroscopy points are visible
* on top of topography images.
*/
class NVBUserPageViewModel : public NVBPageViewModel
{
public:
	NVBUserPageViewModel();

	/*!
	* Try to insert @p page on top. The actual position may be
	* different if a topography page will end up on top of a spectroscopy page.
	*/
	virtual int addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
	/*!
	* Try to insert a dropped page above @p parent or at the end. The actual position may be
	* different if a topography page will end up on top of a spectroscopy page.
	*/
	virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

	/*!
	* Try to insert \a page approximately at position \a index. The actual position may be higher
	* for spectroscopy pages and lower for topography pages. A spectroscopy page
	* will never be inserted below a topography page.
	*/ 
	int insertSourceApprox(NVBDataSource * page, int index, NVBVizUnion viz = NVBVizUnion());
};

#endif // NVBUSERPAGEVIEWMODEL_H
