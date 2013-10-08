//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

typedef QHash<QString, QString> NanonisHeader;

class NanonisFileGenerator: public NVBFileGenerator {
// 		Q_OBJECT
// 		Q_INTERFACES(NVBFileGenerator)

	private:
		void loadChannelsFromSXM(QString filename, NVBFile * sources) const;
		void loadChannelsFromDAT(QString filename, NVBFile * sources) const;
		void loadChannelsFrom3DS(QString filename, NVBFile * sources) const;

	public:
		NanonisFileGenerator(NVBSettings settings): NVBFileGenerator(settings) {;}
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
