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
#ifndef NANONIS_H
#define NANONIS_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#include <QtCore/QtPlugin>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QString>
#include "../core/NVBVariant.h"
#include "../core/NVBFileGenerator.h"
#include "../core/NVBPages.h"

// using namespace NVBErrorCodes;

// struct NanonisHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString, QStringList> NanonisHeader;

class NanonisFileGenerator: public QObject, public NVBFileGenerator {
		Q_OBJECT
		Q_INTERFACES(NVBFileGenerator);

	private:
		static NanonisHeader getNanonisHeader(QFile & file);

		friend class NanonisTopoPage;

		NVBFile * loadSpecAggregation(const NVBAssociatedFilesInfo & info) const;
		NVBFileInfo * loadSpecAggregationInfo(const NVBAssociatedFilesInfo & info) const;

		NVBFile * load3DS(const NVBAssociatedFilesInfo & info) const;
		NVBFileInfo * load3DSInfo(const NVBAssociatedFilesInfo & info) const;

	public:
		NanonisFileGenerator(): NVBFileGenerator() {;}
		virtual ~NanonisFileGenerator() {;}

		virtual inline QString moduleName() const { return QString("Nanonis SXM files");}
		virtual inline QString moduleDesc() const { return QString("Nanonis STM file format"); }

		virtual QStringList extFilters() const {
			static QStringList exts \
			  = QStringList() \
			    << "*.sxm" \
			    << "*.3ds" \
			    << "*.dat" \
			    ;
			return exts;
			}

		QStringList availableInfoFields() const;

		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

		virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;

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