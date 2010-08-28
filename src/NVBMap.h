#ifndef NVBMAP_H
#define NVBMAP_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include "NVBScaler.h"
#include "NVBDataSource.h"

class QImage;
class NVBDataSet;

class NVBAxisMap {
  private:
  public:
		NVBAxisMap() {;}
		virtual ~NVBAxisMap() {;}

    enum MapType {
      General = 0,
      Linear = 1,
      Linear2D = 2
      };
    enum Type { // RTTI
      Template = 0,
      Physical = 1,
      Point = 2,
      Grid = 3
      };

    virtual int dimension() const = 0;

    virtual int valType() const = 0;
    virtual NVBAxisMap::MapType mapType() const { return General; }
    virtual NVBAxisMap::Type mappingType() const = 0;

		virtual QVariant value(QList<int> indexes) = 0;
};

class NVBColorInstance;

/**
	\class NVBColorMap

	Base class for mapping <double> to <color>.
	The implementation should produce a color in RGBA format
	for a float in [0,1] range.
	*/

class NVBColorMap {
    NVBColorMap() {;}
    virtual ~NVBColorMap() {;}

    virtual QRgb colorize(double z) const = 0;

		NVBColorInstance * instantiate(const NVBDataSet * data) { return new NVBColorInstance(data,this); }
};

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
		
		*/
		void setLimits(double zmin, double zmax);

	/**
		\fn void overrideLimits(double zmin, double zmax)

		Tricks the model to make it think the limits are \a zmin & \a zmax
		*/
		void overrideLimits(double zmin, double zmax);

	/**
		\fn void setImageAxes(axisindex_t x, axisindex_t y)

		Set two axis for converting data to images. \a x will be the horizontal axis and \a y will be vertical.
		*/

		void setImageAxes(axisindex_t x, axisindex_t y) {
			axisH = x;
			axisV = y;
			calculateSliceAxes();
			}

		QVector<axisindex_t> getSliceAxes() {
			return sliceAxes;
			}
			
		void setXAxis(axisindex_t x) {
			axisH = x;
			calculateSliceAxes();
			}
			
		void setYAxis(axisindex_t y) {
			axisV = y;
			calculateSliceAxes();
			}
			
		QImage * colorize(QVector<axissize_t> slice, QSize i_wxh = QSize()) const;
		QImage * colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const;
};

#endif
