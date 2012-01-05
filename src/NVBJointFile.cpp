#include "NVBJointFile.h"

NVBJointFile::NVBJointFile(const NVBAssociatedFilesInfo & sources, QList<NVBFile*> components )
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

int NVBJointFile::rowCount ( const QModelIndex &  ) const {
	int sum = 0;
	foreach (NVBFile * f, files)
		sum += f->rowCount();
	return sum;
}

QVariant NVBJointFile::data ( const QModelIndex & index, int role) const {
	int yIndex;
	QModelIndex xIndex;

	calculateIndexes(index, yIndex, xIndex);

	return files.at(yIndex)->data(xIndex,role);
}

Qt::ItemFlags NVBJointFile::flags ( const QModelIndex & index ) const {
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
			r -=k;
			i += 1;
			}
		else {
			file = i;
			page = cf->index(r);
			return;
			}
		}
}
