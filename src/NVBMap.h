#ifndef NVBMAP_H
#define NVBMAP_H

#include <QtCore/QList>
#include <QtCore/QVariant>

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
		NVBRescaleColorModel( const NVBContColorModel * model);
		NVBRescaleColorModel( double vmin, double vmax );
		~NVBColorInstance() {;}

		virtual QRgb colorize(double z) const;

		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }

		void setLimits(double _zmin, double _zmax);
		void overrideLimits(double _zmin, double _zmax);
		void setModel(const NVBContColorModel * model);

		
		QImage * colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const {
		if (!zs) return 0;

		if (!i_wxh.isValid()) i_wxh = d_wxh;

		QImage * result = new QImage(i_wxh,QImage::Format_ARGB32);
		if (!result) return 0;
		result->fill(0x00FFFFFF);

		if (i_wxh != d_wxh) {
			scaler<int,int> w(0,i_wxh.width(),0,d_wxh.width());
			scaler<int,int> h(0,i_wxh.height(),0,d_wxh.height());
			for (int i = 0; i<i_wxh.width(); i++)
				for (int j = 0; j<i_wxh.height(); j++) {
						result->setPixel(i,j,colorize(zs[w.scaleInt(i)+ d_wxh.width()*h.scaleInt(j)]));
					}
			}
		else {
			for (int i = 0; i<i_wxh.width(); i++)
				for (int j = 0; j<i_wxh.height(); j++) {
						result->setPixel(i,j,colorize(zs[i+i_wxh.width()*j]));
					}
			}

		return result;
		}
};

#endif
