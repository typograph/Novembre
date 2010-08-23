#ifndef NVBMAP_H
#define NVBMAP_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QColor>

#include "NVBDataSource.h"
#include "NVBScaler.h"

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

class NVBColorMap {
    NVBColorMap() {;}
    virtual ~NVBColorMap() {;}

    virtual QRgb colorize(double z) const = 0;

		NVBColorInstance * instantiate(const NVBDataSet * data);
};

class NVBColorInstance {
	private:
		const NVBColorMap * source;
		double zmin,zmax;
		NVBValueScaler<double,double> zscaler;
		
	public:
		NVBColorInstance(const NVBDataSet * data, const NVBColorMap * map);
		~NVBColorInstance() {;}

		virtual QRgb colorize(double z) const;

		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }

		void setLimits(double _zmin, double _zmax);
		void overrideLimits(double _zmin, double _zmax);
		
		QImage * colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const;

};

#endif
