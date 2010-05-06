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

#include <QMimeData>
#include "NVBDataSource.h"

class NVBDataSourceMimeData : public QMimeData {
private:
  NVBDataSource * internal;
public :
  NVBDataSourceMimeData(NVBDataSource * source);
  ~NVBDataSourceMimeData();

  virtual QStringList formats () const;
  virtual bool hasFormat ( const QString & mimeType ) const;

  NVBDataSource * getPageData() { return internal; }

  static QString dataSourceMimeType() { return "novembre/datasource"; }
protected :
  virtual QVariant retrieveData ( const QString & mimeType, QVariant::Type type ) const;
};

#endif
