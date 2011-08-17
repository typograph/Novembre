#ifndef NVBFILEBUNDLE_H
#define NVBFILEBUNDLE_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include "NVBFileGenerator.h"
#include "NVBFileInfo.h"

class NVBFileFactory;
class NVBFile;

class NVBFileBundle: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator)

private:
	 NVBFileFactory * const fileFactory;
	 
public:
  NVBFileBundle( NVBFileFactory * ff ) : NVBFileGenerator(), fileFactory(ff) {;}
  virtual ~NVBFileBundle() {;}

  virtual inline QString moduleName() const { return QString("Novembre bundle files"); }
  virtual inline QString moduleDesc() const { return QString("Novembre file format for bundling together files that normally would be opened separately."); }

  virtual QStringList extFilters() const {
	 static QStringList exts = QStringList() << "*.nbl";
	 return exts;
  }

	virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

  virtual QStringList availableInfoFields() const { return QStringList(); }
  virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;
};

#endif
