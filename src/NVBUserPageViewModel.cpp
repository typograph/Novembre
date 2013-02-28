//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "NVBUserPageViewModel.h"

NVBUserPageViewModel::NVBUserPageViewModel()
	: NVBPageViewModel() {

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
	return insertSourceApprox(page, 0);
	}

int NVBUserPageViewModel::insertSourceApprox(NVBDataSource *page, int index) {
	// Find a good place to insert
	// Check that the topography is not ending up on top of spectroscopy
	switch (page->type()) {
		case NVB::TopoPage :
			for (int i = index; i < rowCount(); i++)
				if (pages.at(i)->type() == NVB::SpecPage)
					index = i + 1;

			break;

		case NVB::SpecPage :
			for (int i = index - 1; i >= 0; i--)
				if (pages.at(i)->type() == NVB::TopoPage)
					index = i;

			break;

		default:
			break;
		}

	NVBPageViewModel::addSource(page, index);
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
	icons.swap(row1, row2);
	pages.swap(row1, row2);
	emit layoutChanged();
	emit itemsSwapped(row1, row2);
	}
