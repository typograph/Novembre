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

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#include <QtCore/QtPlugin>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QString>
#include "NVBVariant.h"
#include "NVBFileGenerator.h"
#include "NVBPages.h"

// using namespace NVBErrorCodes;

// struct NanonisHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString,QStringList> NanonisHeader;

class NanonisFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:
	static NanonisHeader getNanonisHeader(QFile & file);

  friend class NanonisTopoPage;

public:
  NanonisFileGenerator():NVBFileGenerator() {;}
  virtual ~NanonisFileGenerator() {;}

	virtual inline QString moduleName() const { return QString("Nanonis SXM files");}
	virtual inline QString moduleDesc() const { return QString("Nanonis STM file format"); }

	virtual QStringList extFilters() const {
			static QStringList exts \
					= QStringList() \
					<< "*.sxm" \
					;
			return exts;
			}

	QStringList availableInfoFields() const;

	virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

};

/*
class NanonisFile : public NVBFile {
public:
  NanonisFile(QString filename);
  virtual ~NanonisFile() {;}
};
*/

class NanonisPage : public NVB3DPage {
Q_OBJECT
private:
//   NanonisHeader header;
public:
	NanonisPage(QFile & file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection = false);
  virtual ~NanonisPage() {;}
public slots:
  virtual void commit() {;}
};

#endif
