#ifndef NVBUSERPAGEVIEWMODEL_H
#define NVBUSERPAGEVIEWMODEL_H

#include "NVBPageViewModel.h"

class NVBUserPageViewModel : public NVBPageViewModel
{
public:
	NVBUserPageViewModel();

	virtual int addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
	virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

	int insertSourceApprox(NVBDataSource * page, int index, NVBVizUnion viz = NVBVizUnion());
};

#endif // NVBUSERPAGEVIEWMODEL_H
