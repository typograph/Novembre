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

class NVBAxisMap {
  private:
  public:
		NVBAxisMap() {;}
		virtual ~NVBAxisMap() {;}

	/// Defines the way indexes are mapped into values.
	enum MapType {
		General = 0, /// Arbitrary mapping, any distribution.
		Linear = 1,  /// Linear mapping, the resulting points are linearly distributed
		Linear2D = 2 /// Bilinear mapping, the resulting points are on a regular grid (not necessarily rectangular).
		             /// A map of this type can be cast into QTransform.
		};

	/// Defines the type of output values.
	enum ValueType {
		Template = 0, /// Underfined
		Physical = 1, /// NVBPhysValue
		Point = 2,    /// NVBPhysPoint
		Color = 3     /// QColor
		};

    virtual int dimension() const = 0;

    virtual int valType() const = 0;
    virtual NVBAxisMap::MapType mapType() const { return General; }
    virtual NVBAxisMap::ValueType mappingType() const = 0;

		virtual NVBVariant value(QVector<axissize_t> indexes) = 0;
};

class NVBColorMap;

/**
	\class NVBColorInstance

	A color map with a built-in scaling.
	A color instance is attached to a dataset and might follow its changes.
	A color instance can produce full images using a given color map
*/

class NVBColorInstance {
	private:
		const NVBColorMap * source;
		const NVBDataSet * data;
		double zmin,zmax;
		NVBValueScaler<double,double> zscaler;
		QVector<axisindex_t> sliceAxes;
		axisindex_t axisH, axisV;
		void calculateSliceAxes();
	public:
		NVBColorInstance(const NVBDataSet * data, const NVBColorMap * map);
		~NVBColorInstance() {;}
		virtual QRgb colorize(double z) const;

		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }

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

	/**
		\fn void setImageAxes(axisindex_t x, axisindex_t y)

		Set two axis for converting data to images. \a x will be the horizontal axis and \a y will be vertical.
		*/

		void setImageAxes(axisindex_t x, axisindex_t y);
		QVector<axisindex_t> getSliceAxes();
		void setXAxis(axisindex_t x);
		void setYAxis(axisindex_t y);
		
		QPixmap colorize(QVector<axissize_t> slice = QVector<axissize_t>(), QSize i_wxh = QSize()) const;
		QPixmap colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const;
};

/**
	\class NVBColorMap

	Base class for mapping <double> to <color>.
	The implementation should produce a color in RGBA format
	for a float in [0,1] range.
	*/

class NVBColorMap {
	public:
    NVBColorMap() {;}
    virtual ~NVBColorMap() {;}

    virtual QRgb colorize(double z) const = 0;

		NVBColorInstance * instantiate(const NVBDataSet * data) const { return new NVBColorInstance(data,this); }
};


#endif
