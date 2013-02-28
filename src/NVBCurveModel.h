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
#ifndef NVBCURVEMODEL_H
#define NVBCURVEMODEL_H

#include <QAbstractListModel>
#include "NVBDataSource.h"

class NVBCurveModel : public QAbstractListModel {
		Q_OBJECT
	private:
		NVBSpecDataSource * provider;
	public:
		NVBCurveModel(NVBSpecDataSource * source);
		virtual ~NVBCurveModel() {;}

		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	public slots:
		void updateModel();
		void resetModel();
	};

#endif

