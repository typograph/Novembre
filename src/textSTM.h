#ifndef TEXTSTM_H
#define TEXTSTM_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QList>
#include "NVBFileGenerator.h"

class TestSTMFileGenerator : public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

public:

	TestSTMFileGenerator():NVBFileGenerator() {;}
	virtual ~TestSTMFileGenerator() {;}

	virtual inline QString moduleName() const { return QString("Novembre test files"); }
	virtual inline QString moduleDesc() const { return QString("Novembre test file format for functional data generation"); }

	virtual QStringList extFilters() const {
		static QStringList exts = QStringList() << "*.sch";
		return exts;
	}

	virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

	virtual QStringList availableInfoFields() const;
//	virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;
};


#endif // TEXTSTM_H
