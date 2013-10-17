//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBMAP_H
#define NVBMAP_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtCore/QSize>
#include "NVBScaler.h"
#include "NVBDataSource.h"

class QPixmap;
class NVBDataSet;

/**
 * \class NVBAxisMap
 *
 * NVBAxisMap defines a way to associate points in an N-dimentional space
 * with values of certain type. In a sense, this is an NVBDataSet where the
 * values are not of type double, but of some other type, like QColor, QString
 * or NVBPhysValue.
 * 
 * NVBAxisMap can be thought of as a labeling mechanism. It is rarely present
 * in orginal files - most of the mapping content is a result of an interaction
 * with the user. Some simple and common examples include colouring spectroscopy
 * curves and labeling spectroscopy locations. It is also one way to implement
 * a mask.
 */
/// A mapping of arbitrary type from a set of axes.
class NVBAxisMap {
	private:
	public:
		NVBAxisMap() {;}
		virtual ~NVBAxisMap() {;}

		/// Defines the way indexes are mapped into values.
		enum MapType {
		  General = 0, ///< Arbitrary mapping, any distribution.
		  Linear = 1,  ///< Linear mapping, the resulting points are linearly distributed
		  Linear2D = 2 ///< Bilinear mapping, the resulting points are on a regular grid (not necessarily rectangular).
		               ///< A map of this type can be cast into QTransform.
		};

		/// Defines the type of output values.
		enum ValueType {
		  Template = 0, ///< Undefined
		  Physical = 1, ///< NVBPhysValue
		  Point = 2,    ///< NVBPhysPoint
		  Color = 3,    ///< QColor
		  Label = 4     ///< QString
		};

		/// Returns the number of axes this map covers
		virtual int dimension() const = 0;

		/// Returns the type of map values. \sa QMetatype
		virtual int valType() const = 0;
		/// Returns the type of the map
		virtual NVBAxisMap::MapType mapType() const { return General; }
		/// Returns the type of map values for common maps
		virtual NVBAxisMap::ValueType mappingType() const = 0;
		/// Returns the value of the map at a specified point
		virtual NVBVariant value(QVector<axissize_t> indexes) = 0;

		/// Creates a deep copy of the map
		virtual NVBAxisMap * copy() = 0;
	};

class NVBColorMap;

/**
	\class NVBColorInstance

	NVBColorInstance is a way to apply a NVBColorMap to real data. While NVBColorMap
	only produces colours for input values in the [0,1] range, NVBColorInstance
	covers any range, by scaling it to fit the range of the NVBColorMap. Any values
	that scale to a result smaller than 0 are treated as 0, and value that scale
	to numbers greater than 1 as 1.
	
	When supplied with a two-dimentional data array, NVBColorInstance produces
	a QPixmap, colouring every point in the array according to the scaling.
*/
/// A color map with a built-in scaling.
class NVBColorInstance {
	protected:
		const NVBColorMap * source;
		double zmin, zmax;
		NVBValueScaler<double, double> zscaler;
	public:
		NVBColorInstance(const NVBColorMap * map);
		~NVBColorInstance() {;}
		virtual QRgb colorize(double z) const;

		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }

		void autoscale(const double * zs, axissize_t size);
		void autoscale(const double * zs, QSize d_wxh);

		/**
			\fn void setLimits(double zmin, double zmax)

			The mapping limits will be changed to \a zmin & \a zmax
			\a zmin will now map to 0 and \a zmax to 1.

			*/
		void setLimits(double zmin, double zmax);

		/**
			\fn void overrideLimits(double zmin, double zmax)

			Tricks the model to make it think the limits are \a zmin & \a zmax
			This is trickier than setLimits. Example:
			We map -1 to 0 and 1 to 1. Do overrideLimits(0,1).
			The mapping stays the same. Now setLimits(-1,1).
			What maps to 0 now? Answer: -3!
			This could have been reached with setLimits(-3,1)

			How to use that? No idea.
			*/
		void overrideLimits(double zmin, double zmax);

		QPixmap colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const;
		QPixmap colorizeFlipped(const double * zs, bool flipX, bool flipY, QSize d_wxh, QSize i_wxh = QSize()) const;
	};

/**
	\class NVBDataColorInstance

	NVBDataColorInstance is a NVBColorInstance applied to a NVBDataSet.
	It follows the changes in the dataset, and automatically adjusts the scaling
	limits to the data. This behaviour can be turned on and off using
	NVBDataColorInstance::setRescaleOnDataChange.
	
	As a dataset can have more than two axes, NVBDataColorInstance lets one select
	the axes that will be used to generate the image.
*/
/// A color map applied to a NVBDataSet.
class NVBDataColorInstance : public QObject, public NVBColorInstance {
		Q_OBJECT
	private:
		/// The dataset that is used
		const NVBDataSet * data;
		/// The axes that are perpendicular to the image axes
		QVector<axisindex_t> sliceAxes;
		/// The axes of the image
		QVector<axisindex_t> xyAxes;
		/// Recalculates sliceAxes after changes in x or y
		void calculateSliceAxes();
		/// Controls automatic rescaling on data change
		bool rescaleOnDataChange;
	private slots:
		void parentDataReformed();
		void parentDataChanged();
	public:
		/// Constructs an NVBDataColorInstance using the dataset \a data and color from \a map
		NVBDataColorInstance(const NVBDataSet * data, const NVBColorMap * map);
		/// Constructs a copy of another NVBDataColorInstance
		NVBDataColorInstance(const NVBDataColorInstance & other);
		/// Constructs a copy of another NVBDataColorInstance
		NVBDataColorInstance & operator=(const NVBDataColorInstance & other);
		/// Deconstructs the instance. Neither the dataset nor the map are affected
		~NVBDataColorInstance();
		/**
			\fn void setImageAxes(axisindex_t x, axisindex_t y)

			Set two axis for converting data to images. \a x will be the horizontal axis and \a y will be vertical.
			*/

		void setImageAxes(QVector<axisindex_t> xy);
		void setImageAxes(axisindex_t x, axisindex_t y);

		QVector<axisindex_t> getSliceAxes();

		QPixmap colorize(QVector<axissize_t> slice = QVector<axissize_t>(), QSize i_wxh = QSize()) const;

		static QPixmap colorize(const NVBDataSet * dset, QVector<axissize_t> slice = QVector<axissize_t>(), QSize i_wxh = QSize());

	public slots:
		void setXAxis(axisindex_t x);
		void setYAxis(axisindex_t y);

		void setRescaleOnDataChange(bool rescale = true) { rescaleOnDataChange = rescale; }
	};

/**
	\class NVBColorMap

	NVBColorMap is an abstract interface for conversion between data and color.
	NVBColorMap::colorize converts a value of type \a double to 32-bit color
	in RGBA	format. The allowed input values are between 0 and 1. The behaviour
	for inputs outside this range is unspecified.
*/
/// Base class for mapping \a double to \a color.
class NVBColorMap {
	public:
		NVBColorMap() {;}
		virtual ~NVBColorMap() {;}

		/// Convert a double value between 0 and 1 to a RGBA 32-bit colo0r
		virtual QRgb colorize(double z) const = 0;

		/// Create an NVBColorInstance object using this map.
		NVBColorInstance * instantiate() const { return new NVBColorInstance(this); }
		/// Create an NVBDataColorInstance object for colouring a particular dataset.
		NVBDataColorInstance * instantiate(const NVBDataSet * data) const
			{ return new NVBDataColorInstance(data, this); }

		/// Create a deep copy of this map.
		virtual NVBColorMap * copy() const = 0;
	};

Q_DECLARE_METATYPE(const NVBColorMap*)
Q_DECLARE_METATYPE(NVBColorMap*)

#endif
