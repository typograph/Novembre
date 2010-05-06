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

#include <QtPlugin>
#include <QFile>
#include <QHash>
#include <QString>
#include "NVBVariant.h"
#include "NVBFileGenerator.h"
#include "NVBPages.h"

using namespace NVBErrorCodes;

// struct CreatecHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString,NVBVariant> CreatecHeader;

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
  CreatecFileGenerator():NVBFileGenerator() {;}
  virtual ~CreatecFileGenerator() {;}

  virtual inline QString moduleName() const { return QString("Createc SPS files");}
  virtual inline QString moduleDesc() const { return QString("Createc STM file format used by STMAFM 2.0"); }

  virtual QStringList extFilters() const {
      static QStringList exts = QStringList() << "*.[dD][aA][tT]" ; // FIXME << "*.[lL][aA][tT]" << "*.[vV][eE][rR][tT]" << "*.[tT][sS][pP][eE][cC]";
      return exts;
  }

//  virtual bool canLoadFile(QString filename);
  virtual NVBFileStruct * loadFile(QFile & file) const;
  virtual NVBFileInfo * loadFileInfo(QFile & file) const;

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
  CreatecDatPage() { throw; }
  CreatecDatPage(const CreatecDatPage & other) { throw; }
  CreatecDatPage( CreatecHeader file_header, int channel, double * bulk_data);
  static QStringList channelNames;
public:
//  CreatecDatPage(QFile & file, int channel);
  virtual ~CreatecDatPage() {;}
  static QList<NVBDataSource*> loadAllChannels(QFile & file);
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
  CreatecHeader header;
  CreatecVertPage() { throw; }
  CreatecVertPage(const CreatecVertPage & other) { throw; }
  CreatecVertPage( CreatecHeader file_header, int channel, double * bulk_data );
public:
  virtual ~CreatecVertPage() {;}

  static QList<NVBDataSource*> loadAllChannels(QFile & file);

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
