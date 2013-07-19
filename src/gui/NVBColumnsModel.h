//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBCOLUMNSMODEL_H
#define NVBCOLUMNSMODEL_H

#include "NVBTokens.h"
#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>

struct NVBColumnDescriptor {
	QString name;
//   QList<NVBCntToken> contents;
	NVBTokens::NVBTokenList contents;
	NVBColumnDescriptor(QString n = QString(), QString c = QString()): name(n), contents(NVBTokens::NVBTokenList(c)) {;}
	NVBColumnDescriptor(QString n, NVBTokens::NVBTokenList c ): name(n), contents(c) {;}
	};


class NVBDirModelColumns : public QAbstractTableModel {

	private:
		QStringList names;
		QList<NVBTokens::NVBTokenList> keys;

	public:
		NVBDirModelColumns();
		virtual ~NVBDirModelColumns() {;}

		virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
		virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
		virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const { return parent.isValid() ? 0 : 2; }
		virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

		QList<NVBColumnDescriptor> clmnDataList();

		QString name(int row) const;
		NVBTokens::NVBTokenList key(int row) const;
		QString sourceKey(int row) const;
		QString verboseKey(int row) const;
//   void addColumn(QString name, QString key);
		void addColumn(QString name, NVBTokens::NVBTokenList key);
		void updateColumn(int index, NVBColumnDescriptor column);
		void removeColumn(int row);

		QStringList columnNames() { return names; }

	};


#endif
