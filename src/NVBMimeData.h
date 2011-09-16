//
// C++ Interface: NVBMimeData
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBMIMEDATA_H
#define NVBMIMEDATA_H

#include <QtCore/QMimeData>
#include "NVBDataSource.h"

class NVBDataSourceMimeData : public QMimeData {
private:
  NVBDataSource * internal;
	NVBDataSet * dset;
public :
	NVBDataSourceMimeData(NVBDataSource * source);
	NVBDataSourceMimeData(NVBDataSet * dataset);
	~NVBDataSourceMimeData();

  virtual QStringList formats () const;
  virtual bool hasFormat ( const QString & mimeType ) const;

  NVBDataSource * getDataSource() { return internal; }
  NVBDataSet * getDataSet() { return dset; }

  static QString dataSourceMimeType() { return QString("novembre/datasource"); }
  static QString dataSetMimeType() { return QString("novembre/dataset"); }
protected :
  virtual QVariant retrieveData ( const QString & mimeType, QVariant::Type type ) const;
};

#endif
