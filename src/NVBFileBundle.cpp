#include "NVBFileBundle.h"
#include "NVBFileFactory.h"
#include "NVBFileInfo.h"
#include "NVBJointFile.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

QStringList NVBFileBundle::getFilenamesFromFile(QString filename) {
  QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return QStringList();
		}
	
	QStringList fnames;
	QString path = QFileInfo(file).absolutePath();
	
	while (file.canReadLine()) {
	  QString line = file.readLine(1024);
	  line.chop(1);
	  fnames << path + "/" + line;
	  }
	
	if (!file.atEnd())
	  fnames << path + "/" + file.read(1024);
	
	return fnames;
}

NVBAssociatedFilesInfo NVBFileBundle::associatedFiles(QString filename) const {
   QStringList ifnames;
  
	foreach(QString fname, getFilenamesFromFile(filename))
	  if (QFileInfo(fname).exists()) {
		 NVBAssociatedFilesInfo li = fileFactory->associatedFiles(fname);
		 if (li.count() == 0)
			 NVBOutputError(QString("File %1 did not load").arg(fname));
		 else
		  ifnames << li;
		}
	  else
		 NVBOutputError(QString("File %1 did not exist, but was mentioned").arg(fname));
	
	return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), ifnames, this);
}

NVBFile * NVBFileBundle::loadFile(const NVBAssociatedFilesInfo & info) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	NVBJointFile * f = new NVBJointFile(info);
	if (!f) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	foreach(NVBAssociatedFilesInfo info, fileFactory->associatedFilesFromList(getFilenamesFromFile(info.first()))) {
		 NVBFile * fx = fileFactory->openFile(info);
		 if (!fx)
			 NVBOutputError(QString("File %1 did not load").arg(info.name()));
		 else
			 f->addFile(fx);
		}
//	  else
//		 NVBOutputError(QString("File %1 does not exist, but was mentioned").arg(fname));

	return f;
}

NVBFileInfo * NVBFileBundle::loadFileInfo( const NVBAssociatedFilesInfo & info ) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	NVBFileInfo * fi = new NVBFileInfo(info);
	if (!fi) return 0;

	foreach(NVBAssociatedFilesInfo info, fileFactory->associatedFilesFromList(getFilenamesFromFile(info.first()))) {
		NVBFileInfo * fix = fileFactory->getFileInfo(info);
		if (!fix)
			NVBOutputError(QString("File %1 did not load").arg(info.name()));
		else
			fi->pages.append(fix->pages);
		}

	return fi;
}
