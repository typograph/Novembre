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
#ifndef NVBJOINTFILE_H
#define NVBJOINTFILE_H

#include "NVBFile.h"
#include <QtCore/QList>

class QVariant;
class QModelIndex;
class NVBAssociatedFilesInfo;

class NVBJointFile : public NVBFile {
	private:
		QList<NVBFile*> files;
		void calculateIndexes(const QModelIndex & original, int & file, QModelIndex & page) const;

	public:
		NVBJointFile();
		NVBJointFile(const NVBAssociatedFilesInfo & sources, QList<NVBFile*> components = QList<NVBFile*>());

		void addFile(NVBFile * other);
		void addFiles(QList<NVBFile*> others);

		virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		virtual bool setData(const QModelIndex & , const QVariant & , int = Qt::EditRole) { return false; }

		virtual Qt::ItemFlags flags(const QModelIndex & index) const;

	};

#endif // NVBJOINTFILE_H
