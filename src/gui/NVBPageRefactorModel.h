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
#ifndef NVBPAGEREFACTORMODEL_H
#define NVBPAGEREFACTORMODEL_H

#include <QAbstractListModel>

class NVBPageViewModel;

class NVBPageRefactorModel : public QAbstractListModel {
	public:
		enum RefactorMode {
		  None = 0, /// The source model is unchanged
		  Empty = 1, /// This model is empty
		  MarkSpectra /// Mark spectroscopic positions on topography
		};

	private:
		NVBPageViewModel * source;
		RefactorMode mode, bkup_mode;

		QList<QIcon> icons;

		void initialize();
		void clear();

	public:
		NVBPageRefactorModel(): QAbstractListModel(), source(0), mode(Empty), bkup_mode(None) {;}
		~NVBPageRefactorModel() {clear();}

		void setModel(NVBPageViewModel * model);
		void setMode(RefactorMode new_mode);

		virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

//   virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		virtual Qt::ItemFlags flags(const QModelIndex & index) const;

		virtual bool removeRow(int row, const QModelIndex & parent = QModelIndex());

		virtual QMimeData * mimeData(const QModelIndexList & indexes) const;
		virtual Qt::DropActions supportedDropActions() const;
		virtual QStringList mimeTypes() const;
		virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);

	};

#endif
