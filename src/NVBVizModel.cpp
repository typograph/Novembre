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
#include "NVBVizModel.h"
#include "NVBPageViewModel.h"

NVBVizModel::NVBVizModel(NVBPageViewModel * model, NVB::ViewType vtype): QAbstractListModel(), viewtype(vtype), pagemodel(model) {
	connect(pagemodel, SIGNAL(itemsSwapped(int, int)),
	        SLOT(swapItems(int, int)));
	connect(pagemodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
	        SLOT(pagesAboutToBeRemoved(const QModelIndex&, int, int)));
	connect(pagemodel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
	        SLOT(pagesRemoved(const QModelIndex&, int, int)));
	connect(pagemodel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
	        SLOT(pagesAboutToBeInserted(const QModelIndex&, int, int)));
	connect(pagemodel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
	        SLOT(pagesInserted(const QModelIndex&, int, int)));
	connect(pagemodel, SIGNAL(dataChanged(const QModelIndex& , const QModelIndex&)),
	        SLOT(pagesChanged(const QModelIndex&, const QModelIndex&)));
//   vizs.append(NVBVizUnion()); // Invalid union

	if (pagemodel->rowCount())
		NVBOutputError("Parent page model not empty!");

//    pagesInserted(QModelIndex(),0,pagemodel->rowCount()-1);

	}

void NVBVizModel::pagesAboutToBeRemoved(const QModelIndex & parent, int start, int end) {
	if (parent.isValid()) return;

	beginRemoveRows(parent, start, end);

	for (int i = start; i <= end; i++) {
		vizs.takeAt(start).clear();
		}

	}

/**
* The model will look in @p future for visualizers corresponding to the newly
* inserted pages. In case there are none, an empty visualizer is used.
*/
void NVBVizModel::pagesInserted(const QModelIndex & parent, int start, int end) {
	if (parent.isValid()) return;

	for (int i = start; i <= end; i++) {
//		NVBDataSource * page = parent.child(i,0).data(PageRole).value<NVBDataSource*>();
		vizs.insert(i, NVBVizUnion());
		}

	endInsertRows();
	}

/**
 * If the page type has not changed, we can still use the previous one, as the view type
 * didn't change.
 */
void NVBVizModel::pagesChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight) {
	if (!topLeft.isValid() || !bottomRight.isValid()) return;

	for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
		if (vizs.at(i).valid && vizs.at(i).ptype != pagemodel->index(i).data(PageTypeRole).value<NVB::PageType>()) {
			vizs[i].clear();
			vizs.replace(i, NVBVizUnion());
			emit dataChanged(index(i), index(i));
			}
		}
	}

/**
 * NVBVizModel defines an additional role \c PageVizItemRole to return
 * the visualizer at @p index. For all other roles, this model returns
 * data from underlying NVBPageViewModel.
 */
QVariant NVBVizModel::data(const QModelIndex & index, int role) const {
	if (!index.isValid()) return QVariant();

	if (index.row() >= vizs.size()) return QVariant();

//  if (role != Qt::DisplayRole) return QVariant();
	switch (role) {
		case PageVizItemRole:
			return QVariant::fromValue(vizs.at(index.row()));;

		default:
			return pagemodel->data(index, role);
		}
	}

Qt::ItemFlags NVBVizModel::flags(const QModelIndex & index) const {
	if (index.isValid())
		return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
	else
		return QAbstractListModel::flags(index);
	}

bool NVBVizModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	if (!index.isValid()) return false;

	switch (role) {
		case PageVizItemRole: {
			setVisualizer(value.value<NVBVizUnion>(), index.row());
			return true;
			}

		default :
			return false;
		}
	}

NVBVizModel::~ NVBVizModel() {
	if (rowCount()) {
		beginRemoveRows(QModelIndex(), 0, rowCount() - 1);

		while (!vizs.isEmpty()) vizs.takeLast().clear();

		endRemoveRows();
		}
	}

void NVBVizModel::setVisualizer(NVBVizUnion visualizer, const QModelIndex & index) {
	setVisualizer(visualizer, index.row());
	}

void NVBVizModel::setVisualizer(NVBVizUnion visualizer, int row) {
	if (row < 0 || row >= rowCount()) return;

	NVBVizUnion old = vizs.at(row);
	vizs.replace(row, visualizer);
	old.clear();
	emit dataChanged(index(row), index(row));
	}

void NVBVizModel::swapItems(int row1, int row2) {
	if (!(row1 >= 0 && row2 >= 0 && row1 < rowCount() && row2 < rowCount())) {
		NVBOutputError("Swap targets not in model");
		return;
		}

	emit layoutAboutToBeChanged();
	vizs.swap(row1, row2);
	emit layoutChanged();
	emit itemsSwapped(row1, row2);
	}

void NVBVizModel::pagesAboutToBeInserted(const QModelIndex & parent, int start, int end) {
	if (parent.isValid()) return;

	beginInsertRows(parent, start, end);
	}

void NVBVizModel::pagesRemoved(const QModelIndex & parent, int start, int end) {
	Q_UNUSED(start);
	Q_UNUSED(end);

	if (parent.isValid()) return;

	endRemoveRows();
	}
