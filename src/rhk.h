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

#ifndef RHK_H
#define RHK_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#define RHK_TOPOPAGE 0
#define RHK_SPECPAGE 1
#define RHK_UNKPAGE 2
#define RHK_ANNOTATEDSPECPAGE 3

#include <QtPlugin>
#include <QFile>
#include "dimension.h"
#include "NVBFileGenerator.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
// #include "NVBLogger.h"
#include "NVBPages.h"

/*GUID*/
typedef struct {
  quint32 Data1;
  quint16 Data2;
  quint16 Data3;
  quint64 Data4;
} RHK_GUID;

/*TRHKHEADER*/
struct TRHKHeader{
  qint16 parameter_size;
  quint16 version[18];
  qint16 string_count;
  qint32 type;
  qint32 page_type;
  qint32 data_sub_source;
  qint32 line_type;
  qint32 x_coordinate;
  qint32 y_coordinate;
  qint32 x_size;
  qint32 y_size;
  qint32 source_type;
  qint32 image_type;
  qint32 scan;
  qint32 group_ID;
  quint32 page_data_size;
  qint32 z_min;
  qint32 z_max;
  float x_scale;
  float y_scale;
  float z_scale;
  float xy_scale;
  float x_offset;
  float y_offset;
  float z_offset;
  float period;
  float bias;
  float current;
  float angle;
	RHK_GUID page_ID;
  qint32 colorinfo_count;
  qint32 grid_xsize;
  qint32 grid_ysize;
};


/*TRHKCOLORTRANSFORM*/
struct  TRHKColorTransform{
  float gamma;
  float alpha;
  float x_start;
  float x_stop;
  float y_start;
  float y_stop;
  qint32 mapping_mode;
  // FIXME how big is RHK's bool?
  bool invert;  
};

/*TCOLORINFO*/
typedef struct  {
  qint16 parameter_count;
  float start_h;
  float start_s;
  float start_b;
  float end_h;
  float end_s;
  float end_b;
  qint32 color_direction;
  qint32 color_entries;
  float start_slidepos;
  float end_slidepos;
  TRHKColorTransform transform;
} TRHKColorInfo;

using namespace NVBErrorCodes;

class NVBDataSource;

class RHKFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:
  static NVBDataSource * loadNextPage(QFile & file);
  static TRHKHeader getRHKHeader(QFile & file);
  static QStringList loadRHKStrings(QFile & file, qint16 nstrings);
  static QString getPageTypeString(qint32 type);
	static QString getGUIDString(RHK_GUID id);
  static QString getLineTypeString(qint32 type);
  static QString getSourceTypeString(qint32 type);
  static QString getDirectionString(qint32 type);
  static QString getImageTypeString(qint32 type);

  friend class RHKTopoPage;
  friend class RHKSpecPage;

public:
  RHKFileGenerator():NVBFileGenerator() {;}
  virtual ~RHKFileGenerator() {;}

  virtual inline QString moduleName() const { return QString("RHK XPMPro files");}
  virtual inline QString moduleDesc() const { return QString("RHK Technology STM file format. Works for [SM2], SM3 and [SM4] files"); }

  virtual inline QStringList extFilters() const {
      static QStringList exts = QStringList() << "*.SM3" ; // FIXME << "*.SM2" << "*.SM4";
      return exts;
  }

//  virtual bool canLoadFile(QString filename);
	virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

  virtual QStringList availableInfoFields() const;

	//- Using super's method, since RHK only uses one file per project.
	// virtual inline NVBAssociatedFilesInfo associatedFiles(QString filename) const;
};

class RHKTopoPage : public NVB3DPage {
Q_OBJECT
private:
  TRHKHeader header;
  QStringList strings;
public:
  RHKTopoPage(QFile & file);
  virtual ~RHKTopoPage() {;}
public slots:
  virtual void commit() {;}
};

class RHKSpecPage : public NVBSpecPage {
Q_OBJECT
protected:
  TRHKHeader header;
  QStringList strings;
  double * ys;
  double * xs;
public:
  RHKSpecPage(QFile & file);
  virtual ~RHKSpecPage();
public slots:
  virtual void commit() {;}
};


#endif
