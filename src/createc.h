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

#ifndef CREATEC_H
#define CREATEC_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include "NVBUnits.h"
#include "NVBVariant.h"
#include "NVBFileGenerator.h"

typedef QHash<QString, NVBVariant> CreatecHeader;

class CreatecFileGenerator: public QObject, public NVBFileGenerator {
		Q_OBJECT
		Q_INTERFACES(NVBFileGenerator);

	private:

		static CreatecHeader getCreatecHeader(QFile & file);

		NVBDataComments commentsFromHeader(const CreatecHeader & header) const;

		void loadAllChannelsFromDAT(QString filename, NVBFile * sources) const;
		void loadAllChannelsFromVERT(QStringList filenames, NVBFile * sources) const;
		void loadAllChannelsFromLAT(QString filename, NVBFile * sources) const;
		void loadAllChannelsFromTSPEC(QString filename, NVBFile * sources) const;

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

#endif
