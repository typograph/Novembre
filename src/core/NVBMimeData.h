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
