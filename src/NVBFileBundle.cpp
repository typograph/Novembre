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
#include "NVBFileBundle.h"
#include "NVBFileFactory.h"
#include "NVBFileInfo.h"
#include "NVBJointFile.h"
#include "NVBLogger.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

QStringList NVBFileBundle::getFilenamesFromFile(QString filename) {
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		NVBOutputFileError(&file);
		return QStringList();
		}

	QTextStream fs(&file);

	QStringList fnames;
	QString path = QFileInfo(file).absolutePath();

	QString line;

	while (!(line = fs.readLine(1024)).isNull())
		fnames << path + "/" + line;

//	if (!file.atEnd())
//	  fnames << path + "/" + file.read(1024);

	return fnames;
	}

NVBAssociatedFilesInfo NVBFileBundle::associatedFiles(QString filename) const {
	QStringList ifnames;

	foreach(QString fname, getFilenamesFromFile(filename)) {
		if (QFileInfo(fname).exists()) {
			NVBAssociatedFilesInfo li = fileFactory->associatedFiles(fname);

			if (li.count() == 0)
				NVBOutputError(QString("File %1 did not load").arg(fname));
			else
				ifnames << li;
			}
		else
			NVBOutputError(QString("File %1 did not exist, but was mentioned").arg(fname));
		}

	// We have to add the name of the bundle file here, to mark that it has been loaded.
	if (ifnames.count()) ifnames.prepend(filename);

	return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), ifnames, this);
	}

NVBFile * NVBFileBundle::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() < 2) {
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

NVBFileInfo * NVBFileBundle::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() {
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
