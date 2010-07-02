#ifndef NVBJOINTFILE_H
#define NVBJOINTFILE_H

#include <NVBFile.h>

class NVBJointFile : public NVBFile {
private:
	QList<NVBFile*> files;
	void NVBJointFile::calculateIndexes(const QModelIndex & original, int & file, QModelIndex & page);

public:
	NVBJointFile(NVBAssociatedFilesInfo & sources, QList<NVBFile*> components = QList<NVBFile*>);

	void addFile(NVBFile * other);
	void addFiles(QList<NVBFile*> others);

	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ) { return false; }

	virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

};

#endif // NVBJOINTFILE_H
