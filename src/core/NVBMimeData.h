//
// Copyright 2006 Timofey <typograph@elec.ru>
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

#include <QMimeData>
#include "NVBDataSource.h"

class NVBDataSourceMimeData : public QMimeData {
	private:
		NVBDataSource * internal;
	public :
		NVBDataSourceMimeData(NVBDataSource * source);
		~NVBDataSourceMimeData();

		virtual QStringList formats() const;
		virtual bool hasFormat(const QString & mimeType) const;

		NVBDataSource * getPageData() { return internal; }

		static QString dataSourceMimeType() { return "novembre/datasource"; }
	protected :
		virtual QVariant retrieveData(const QString & mimeType, QVariant::Type type) const;
	};

#endif
