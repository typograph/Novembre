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
#ifndef CREATEC_H
#define CREATEC_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#include <QtPlugin>
#include <QFile>
#include <QHash>
#include <QString>
#include "../core/NVBVariant.h"
#include "../core/NVBFileGenerator.h"
#include "../core/NVBFile.h"

// using namespace NVBErrorCodes;

// struct CreatecHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString, NVBVariant> CreatecHeader;

class CreatecFileGenerator: public QObject, public NVBFileGenerator {
		Q_OBJECT
		Q_INTERFACES(NVBFileGenerator);

	private:
//   static NVBDataSource * loadNextPage(QFile & file);
		static CreatecHeader getCreatecHeader(QFile & file);
//   static QStringList loadCreatecStrings(QFile & file, qint16 nstrings);
//   static QString getPageTypeString(qint32 type);
//   static QString getGUIDString(GUID id);
//   static QString getLineTypeString(qint32 type);
//   static QString getSourceTypeString(qint32 type);
//   static QString getDirectionString(qint32 type);
//   static QString getImageTypeString(qint32 type);

		friend class CreatecDatPage;
		friend class CreatecLatPage;
		friend class CreatecVertPage;
		friend class CreatecTSpecPage;

	public:
		CreatecFileGenerator(): NVBFileGenerator() {;}
		virtual ~CreatecFileGenerator() {;}

		virtual inline QString moduleName() const { return QString("Createc SPS files");}
		virtual inline QString moduleDesc() const { return QString("Createc STM file format used by STMAFM 2.x"); }

		virtual QStringList extFilters() const {
			static QStringList exts = QStringList() << "A??????.??????.dat" << "A??????.*.vert"; // FIXME << "A*.lat" << "A*.tspec";
			return exts;
			}

//  virtual bool canLoadFile(QString filename);
		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

		virtual QStringList availableInfoFields() const;
		virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;
	};

/*
class CreatecFile : public NVBFile {
public:
  CreatecFile(QString filename);
  virtual ~CreatecFile() {;}
};
*/

class CreatecDatPage : public NVB3DPage {
		Q_OBJECT
	private:
		CreatecHeader header;
		CreatecDatPage();
		CreatecDatPage(const CreatecDatPage & other);
		CreatecDatPage(CreatecHeader file_header, int channel, double * bulk_data);
		static QStringList channelNames;
	public:
//  CreatecDatPage(QFile & file, int channel);
		virtual ~CreatecDatPage() {;}
		static QList<NVBDataSource*> loadAllChannels(QString filename);
	public slots:
		virtual void commit() {;}
	};

class CreatecLatPage : public NVBSpecPage {
		Q_OBJECT
	protected:
		CreatecHeader header;
	public:
		CreatecLatPage(QFile & file);
		virtual ~CreatecLatPage() {;}

	public slots:
		virtual void commit() {;}
	};

class CreatecVertPage : public NVBSpecPage {
		Q_OBJECT
	protected:
//	CreatecHeader header;
		CreatecVertPage();
		CreatecVertPage(const CreatecVertPage & other);
		CreatecVertPage(CreatecHeader file_header, int channel, double * bulk_data);
	public:
		virtual ~CreatecVertPage() {;}

		static QList<NVBDataSource*> loadAllChannels(QStringList filenames);

	public slots:
		virtual void commit() {;}
	};

class CreatecTSpecPage : public NVBSpecPage {
		Q_OBJECT
	protected:
		CreatecHeader header;
	public:
		CreatecTSpecPage(QFile & file);
		virtual ~CreatecTSpecPage() {;}
	public slots:
		virtual void commit() {;}
	};

#endif
