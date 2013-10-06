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

/**
 * \class NVBAxisMapping
 * 
 * \brief NVBAxisMap combined with axes for a specific NVBDataSource
 * 
 * NVBAxisMapping is a union containing an NVBAxisMap and the related axes.
 * The axes are kept as a list of axisindex_t values, denoting the indices
 * of the axes in the original datasource.
 * 
 * The reason the information about axes is not kept in the NVBAxisMap
 * is that the indices may change after working with the data, while the map
 * might not change. Thus, further incarnations of the data may use the same
 * map, but a different mapping. The means to do so are given by this class.
 * 
 */
struct NVBAxisMapping {
	/// The original map
	NVBAxisMap * map;
	/// The datasource indexes of the axes, that the map spans
	QVector<axisindex_t> axes;
	/// Constructs an invalid mapping
	NVBAxisMapping() : map(0) {;}
	/// Constructs a mapping with given map and axes
	NVBAxisMapping(NVBAxisMap * m, QVector<axisindex_t> a) : map(m), axes(a) {;}
	/// Compares mappings for equality - true if both using the same map
	bool operator==(const NVBAxisMapping & other) { return map == other.map; }
	/// Compares a mapping with a map - true if using the same map
	bool operator==(const NVBAxisMap * other_map) { return map == other_map; }
	};

/**
 * \class NVBAxis
 * 
 * \brief One of the data dimensions.
 * 
 * An NVBAxis is one data dimension. It describes a part of the shape of
 * the data and contains additional information such as the associated
 * real-world values.
 * 
 * An axis has an associated name() and a length() (number of points).
 * By default, an axis does not have any other properties. On the other
 * hand, an axis often corresponds to a physical quantity in an experiment.
 * So normally, a point on the axis can be translated to physical values
 * via physMap(). All the mappings from axis points to other types of values\
 * are available as a maps() list.
 * 
 * An axis is connected to the parent dataSource(), and knows its index
 * inside the datasource (available as parentIndex())
 * 
 */
class NVBAxis {
		friend class NVBConstructableDataSource;
	protected:
		/// The datasource containing this axis
		NVBDataSource * p;
		/// Axis name
		QString n;
		/// Axis length
		axissize_t l;
		/// Relevant mappings
		QVector< NVBAxisMapping> ms;
		/// NVBPhysValue map - for easier access to units
		NVBAxisPhysMap * pm;

		/// Adds a mapping to this axis
		void addMapping( NVBAxisMapping mapping ) ;

	public:
		/// Constructs an invalid axis
		NVBAxis(): p(0) {;}
		/// Constructs an axis for a given datasource, with given name and length
		NVBAxis(NVBDataSource * parent, QString name, axissize_t length) : p(parent), n(name), l(length), pm(0) {;}
		// NVBAxis doesn't own anything, neither the parent, nor the maps.
		/// Deconstructs the axis, preserving the maps
		~NVBAxis() {;}

		/// The name of the axis
		inline QString name() const { return n; }
		/// The length of the axis (number of points)
		inline axissize_t length() const { return l; }
		/// Mappings containing this axis
		inline QVector<NVBAxisMapping> maps() const { return ms; }

		/// Mapping of this axis to physical values
		inline NVBAxisPhysMap * physMap() const { return pm; }

		/// The datasource this axis belongs to
		inline NVBDataSource * dataSource() const { return p; }

		/// Compares two axes for equality (same parent datasource, same name)
		bool operator==(const NVBAxis & other) const {
			return p == other.p && n == other.n;
			}

		/// Compares two axes for inequality (different parent datasources or different names)
		inline bool operator!=(const NVBAxis & other) const {
			return !operator==(other);
			}

		/// An invalid axis belongs to no datasource
		inline bool isValid() const { return p != 0; }
		/// Index of this axis in the parent datasource
		axisindex_t parentIndex() const;
	};

// Q_DECLARE_METATYPE(NVBAxis)

#endif
