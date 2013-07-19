//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBMimeData.h"
#include "NVBMap.h"
#include <QtGui/QPixmap>
#include <QtCore/QStringList>

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSource * source)
	: QMimeData()
	, internal(source)
	, dset(0) {
	useDataSource(internal);
	// Think about setHtml and stuff
	}

NVBDataSourceMimeData::NVBDataSourceMimeData(NVBDataSet * dataset)
	: QMimeData()
	, internal(dataset->dataSource())
	, dset(dataset) {
	useDataSource(internal);
	// Think about setHtml and stuff
	}

NVBDataSourceMimeData::~ NVBDataSourceMimeData() {
	releaseDataSource(internal);
	}

QStringList NVBDataSourceMimeData::formats() const {
	QStringList result = QMimeData::formats();

	if (dset) {
		result << dataSetMimeType() << "application/x-qt-image";// << "text/plain";
		}

	result << dataSourceMimeType();
	return result;
	}

bool NVBDataSourceMimeData::hasFormat(const QString & mimeType) const {
	return QMimeData::hasFormat(mimeType);
	}

QVariant NVBDataSourceMimeData::retrieveData(const QString & mimeType, QVariant::Type type) const {
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

	return QMimeData::retrieveData(mimeType, type);
	}

