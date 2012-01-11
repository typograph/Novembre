#include "NVBUserPageViewModel.h"

NVBUserPageViewModel::NVBUserPageViewModel()
: NVBPageViewModel()
	{

	}

bool NVBUserPageViewModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	Q_UNUSED(row);
	Q_UNUSED(column);
//   if (column != 0) return false;
//   if (parent.isValid()) return false;
	if (!data->hasFormat(NVBDataSourceMimeData::dataSourceMimeType()))
		return false;
	if (!(action & supportedDropActions())) return false;

	insertSourceApprox(((NVBDataSourceMimeData*)data)->getPageData(),	parent.isValid() ? parent.row() : rowCount());

	return true;

	}

int NVBUserPageViewModel::addSource(NVBDataSource *page) {
	return insertSourceApprox(page,0);
	}

int NVBUserPageViewModel::insertSourceApprox(NVBDataSource *page, int index) {
	// Find a good place to insert
	// Check that the topography is not ending up on top of spectroscopy
	switch (page->type()) {
		case NVB::TopoPage :
			for (int i = index; i<rowCount(); i++)
				if (pages.at(i)->type() == NVB::SpecPage)
					index = i+1;
			break;
		case NVB::SpecPage :
			for (int i = index-1; i>=0; i--)
				if (pages.at(i)->type() == NVB::TopoPage)
					index = i;
			break;
		default:
			break;
		}

	NVBPageViewModel::addSource(page,index);
	return index;
	}

/**
 * The model will emit layoutAboutToBeChanged and layoutChanged.
 */
void NVBUserPageViewModel::swapItems(int row1, int row2) {
	if (!(row1 >= 0 && row2 >= 0 && row1 < rowCount() && row2 < rowCount())) {
		NVBOutputError("Swap targets not in model");
		return;
		}
	emit layoutAboutToBeChanged();
	icons.swap(row1,row2);
	pages.swap(row1,row2);
	emit layoutChanged();
	emit itemsSwapped(row1,row2);
	}
