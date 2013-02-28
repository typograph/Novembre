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
#include "NVBJointFile.h"

NVBJointFile::NVBJointFile(const NVBAssociatedFilesInfo & sources, QList<NVBFile*> components)
	: NVBFile(sources) {
	addFiles(components);
	}

void NVBJointFile::addFile(NVBFile * other) {
	other->use();
	files << other;
	}

void NVBJointFile::addFiles(QList<NVBFile*> others) {
	foreach(NVBFile * f, others)
	addFile(f);
	}

int NVBJointFile::rowCount(const QModelIndex &) const {
	int sum = 0;
	foreach(NVBFile * f, files)
	sum += f->rowCount();
	return sum;
	}

QVariant NVBJointFile::data(const QModelIndex & index, int role) const {
	int yIndex;
	QModelIndex xIndex;

	calculateIndexes(index, yIndex, xIndex);

	return files.at(yIndex)->data(xIndex, role);
	}

Qt::ItemFlags NVBJointFile::flags(const QModelIndex & index) const {
	int yIndex;
	QModelIndex xIndex;

	calculateIndexes(index, yIndex, xIndex);

	return files.at(yIndex)->flags(xIndex);
	}

void NVBJointFile::calculateIndexes(const QModelIndex & original, int & file, QModelIndex & page) const {
	QListIterator<NVBFile*> it(files);
	int r = original.row();
	int i = 0;

	while (it.hasNext()) {
		NVBFile * cf = it.next();
		int k = cf->rowCount();

		if (r >= k) {
			r -= k;
			i += 1;
			}
		else {
			file = i;
			page = cf->index(r);
			return;
			}
		}
	}
