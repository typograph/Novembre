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

#include <QtPlugin>
#include <QFile>
#include <QHash>
#include <QString>
#include "NVBVariant.h"
#include "NVBFileGenerator.h"
#include "NVBPages.h"

using namespace NVBErrorCodes;

// struct NanonisHeaderEntry {
//   QString altName;
//   NVBVariant v;
// };

typedef QHash<QString,QStringList> NanonisHeader;

class NanonisFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:
  static NanonisHeader getNanonisHeader(QFile * const file);

  friend class NanonisTopoPage;
#ifdef WITH_SPEC_AGGR
  NVBFileStruct * loadSpecAggregation(QFile & file);
  NVBFileInfo * loadSpecAggregationInfo(QFile & file);
#endif
public:
  NanonisFileGenerator():NVBFileGenerator() {;}
  virtual ~NanonisFileGenerator() {;}

  virtual inline QString moduleName() { return QString("Nanonis SXM files");}
  virtual inline QString moduleDesc() { return QString("Nanonis STM file format"); }

#ifdef WITH_SPEC_AGGR
  virtual inline QString extFilter() { return QString("*.sxm;*.SXM;*.nns;*.NNS"); }; // single semicolon as separator
  virtual inline QString nameFilter() { return QString("Nanonis files (*.sxm *.nns)"); } // space as separator
#else
  virtual inline QString extFilter() { return QString("*.sxm;*.SXM"); }; // single semicolon as separator
  virtual inline QString nameFilter() { return QString("Nanonis files (*.sxm)"); } // space as separator
#endif

  virtual bool canLoadFile(QString filename);
  virtual NVBFileStruct * loadFile(QString filename);
  virtual NVBFileInfo * loadFileInfo(QString filename);

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
  NanonisPage(QFile * const file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection = false);
  virtual ~NanonisPage() {;}
public slots:
  virtual void commit() {;}
};

#endif
