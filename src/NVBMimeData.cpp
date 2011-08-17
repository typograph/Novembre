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
#include <QtCore/QStringList>

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSource * source, NVBDataSet * dataset)
 : QMimeData()
 , internal(source)
 , dset(dataset)
{
  // Think about setHtml and stuff
}

NVBDataSourceMimeData::~ NVBDataSourceMimeData()
{
}

QStringList NVBDataSourceMimeData::formats() const
{
	QStringList result = QMimeData::formats();
	if (dset) {
		result << dataSetMimeType() << "application/x-qt-image" << "text/plain";
		}
	result << dataSourceMimeType();
  return result;
}

bool NVBDataSourceMimeData::hasFormat(const QString & mimeType) const
{
  return QMimeData::hasFormat(mimeType);
}

QVariant NVBDataSourceMimeData::retrieveData(const QString & mimeType, QVariant::Type type) const
{
  if (mimeType == dataSourceMimeType())
    return QVariant::fromValue(internal);
	if (dset) {
		if (mimeType == dataSetMimeType())
			return QVariant::fromValue(dset);
		if (mimeType == "text/plain")
			return dset->name();
//		if (mimeType == "application/x-qt-image")
//			return 
		}
  return QMimeData::retrieveData(mimeType,type);
}

