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

#ifndef CREATEC_H
#define CREATEC_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#include <QtCore/QtPlugin>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QString>
#include "NVBVariant.h"
#include "NVBFileGenerator.h"
#include "NVBFile.h"

typedef QHash<QString,NVBVariant> CreatecHeader;

class CreatecFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:

	QStringList DATchannelNames;
	QList<NVBDimension> DATchannelDims;

	static CreatecHeader getCreatecHeader(QFile & file);

	NVBDataComments commentsFromHeader(const CreatecHeader & header) const;

	NVBDataSource* loadAllChannelsFromDAT(QString filename) const;
	NVBDataSource* loadAllChannelsFromVERT(QStringList filenames) const;
	NVBDataSource* loadAllChannelsFromLAT(QString filename) const;
	NVBDataSource* loadAllChannelsFromTSPEC(QString filename) const;

public:
	CreatecFileGenerator():NVBFileGenerator() {
		DATchannelNames << "Topography" << "Current" << "ADC1" << "ADC2";
		DATchannelDims << NVBDimension("nm")
									 << NVBDimension("A")
									 << NVBDimension("DAC",false)
									 << NVBDimension("DAC",false)
									 ;
		}
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

#endif
