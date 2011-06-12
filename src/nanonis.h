/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov   *
 *   Timofey.Balashov@pi.uka.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NANONIS_H
#define NANONIS_H

#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include "NVBUnits.h"
#include "NVBFileGenerator.h"

// using namespace NVBErrorCodes;

// struct NanonisHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString,QString> NanonisHeader;

class NanonisFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:
	NanonisHeader getSXMHeader(QFile & file) const;
	NVBDataComments getCommentsFromSXMHeader(NanonisHeader h) const;
	void loadChannelsFromSXM(QString filename, NVBFile * sources) const;
	
	NanonisHeader getDATHeader(QFile & file) const;
	NVBDataComments getCommentsFromDATHeader(NanonisHeader h) const;
	void loadChannelsFromDAT(QString filename, NVBFile * sources) const;

	NanonisHeader get3DSHeader(QFile & file) const;
	NVBDataComments getCommentsFrom3DSHeader(NanonisHeader h) const;
	void loadChannelsFrom3DS(QString filename, NVBFile * sources) const;
	
	
public:
  NanonisFileGenerator():NVBFileGenerator() {;}
  virtual ~NanonisFileGenerator() {;}

	virtual inline QString moduleName() const { return QString("Nanonis SXM files");}
	virtual inline QString moduleDesc() const { return QString("Nanonis STM file format"); }

	virtual QStringList extFilters() const {
			static QStringList exts = QStringList() << "*.sxm" << "*.dat" << "*.3ds";
			return exts;
			}

	QStringList availableInfoFields() const;

	virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

	virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;

};

#endif
