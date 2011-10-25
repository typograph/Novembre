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
#include "NVBMap.h"
#include <QtGui/QPixmap>
#include <QtCore/QStringList>

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSource * source)
 : QMimeData()
 , internal(source)
 , dset(0)
{
	useDataSource(internal);
  // Think about setHtml and stuff
}

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSet * dataset)
 : QMimeData()
 , internal(dataset->dataSource())
 , dset(dataset)
{
	useDataSource(internal);
	// Think about setHtml and stuff
}

NVBDataSourceMimeData::~ NVBDataSourceMimeData()
{
	releaseDataSource(internal);
}

QStringList NVBDataSourceMimeData::formats() const
{
	QStringList result = QMimeData::formats();
	if (dset) {
		result << dataSetMimeType() << "application/x-qt-image";// << "text/plain";
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
		if (mimeType == "application/x-qt-image")
			return QVariant::fromValue(NVBDataColorInstance::colorize(dset));
		}
  return QMimeData::retrieveData(mimeType,type);
}

