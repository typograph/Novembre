#include "NVBJointFile.h"

NVBJointFile::NVBJointFile(NVBAssociatedFilesInfo & sources, QList<NVBFile*> components = QList<NVBFile*>)
	: NVBFile(source) {
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

int NVBJointFile::rowCount ( const QModelIndex & parent = QModelIndex() ) const {
	int sum = 0;
	foreach (NVBFile * f, files)
		sum += f->rowCount();
}

QVariant NVBJointFile::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
	int yIndex;
	QModelIndex xIndex;

	calculateIndexes(index, yIndex, xIndex);

	return files.at(xIndex)->data(yIndex,role);
}

Qt::ItemFlags NVBJointFile::flags ( const QModelIndex & index ) const {
	int yIndex;
	QModelIndex xIndex;

	calculateIndexes(index, yIndex, xIndex);

	return files.at(xIndex)->flags(yIndex);
}

void NVBJointFile::calculateIndexes(const QModelIndex & original, int & file, QModelIndex & page) {
	QListIterator<NVBFile*> it(files);
	int r = original.row();
	int i = 0;
	while (it.hasNext()) {
		NVBFile * cf = it.next();
		int k = cf->rowCount();
		if (r >= k)
			r -=k;
			i += 1;
		else {
			file = i;
			page = cf->index(r);
			return;
			}
		}
}
