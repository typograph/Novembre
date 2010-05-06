//
// C++ Implementation: NVBMimeData
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBMimeData.h"

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSource * source):QMimeData(),internal(source)
{
  // Think about setHtml and stuff
}

NVBDataSourceMimeData::~ NVBDataSourceMimeData()
{
}

QStringList NVBDataSourceMimeData::formats() const
{
  return QMimeData::formats() << dataSourceMimeType();
}

bool NVBDataSourceMimeData::hasFormat(const QString & mimeType) const
{
  if (mimeType == dataSourceMimeType()) return true;
  return QMimeData::hasFormat(mimeType);
}

QVariant NVBDataSourceMimeData::retrieveData(const QString & mimeType, QVariant::Type type) const
{
  if (mimeType == dataSourceMimeType())
    return QVariant::fromValue(internal);
  return QMimeData::retrieveData(mimeType,type);
}

