//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBAXIS_H
#define NVBAXIS_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include "NVBDataGlobals.h"
class NVBDataSource;
class NVBAxisMap;
class NVBAxisPhysMap;

struct NVBAxisMapping {
	NVBAxisMap * map;
	QVector<axisindex_t> axes;
	NVBAxisMapping() : map(0) {;}
	NVBAxisMapping(NVBAxisMap * m, QVector<axisindex_t> a) : map(m), axes(a) {;}
	bool operator==(const NVBAxisMapping & other) { return map == other.map; }
	bool operator==(const NVBAxisMap * other_map) { return map == other_map; }
	};

class NVBAxis {
		friend class NVBConstructableDataSource;
	protected:
		/// Parent data source (to be able to implement functions)
		NVBDataSource * p;
		/// Axis name
		QString n;
		/// Axis length
		axissize_t l;
		/// Relevant mappings
		QVector< NVBAxisMapping> ms;
		/// NVBPhysValue map - for easier access to units
		NVBAxisPhysMap * pm;

		void addMapping( NVBAxisMapping mapping ) ;

	public:
		NVBAxis(): p(0) {;}
		NVBAxis(NVBDataSource * parent, QString name, axissize_t length) : p(parent), n(name), l(length), pm(0) {;}
		// NVBAxis doesn't own anything, neither the parent, nor the maps.
		~NVBAxis() {;}

		inline QString name() const { return n; }
		inline axissize_t length() const { return l; }
		inline QVector<NVBAxisMapping> maps() const { return ms; }

		inline NVBAxisPhysMap * physMap() const { return pm; }

		inline NVBDataSource * dataSource() const { return p; }

		bool operator==(const NVBAxis & other) const {
			return p == other.p && n == other.n;
			}

		inline bool operator!=(const NVBAxis & other) const {
			return !operator==(other);
			}

		inline bool isValid() const { return p != 0; }
		axisindex_t parentIndex() const;
	};

// Q_DECLARE_METATYPE(NVBAxis)

#endif
