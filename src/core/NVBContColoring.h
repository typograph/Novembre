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


#ifndef NVBCONTCOLORING_H
#define NVBCONTCOLORING_H

#include <float.h>
#include "dimension.h"
#include "NVBContColorModel.h"

namespace NVBColoring {

typedef enum {
  colortype_None = 0,
  colortype_HSVWheel,
  colortype_RGBRamp,
  colortype_GrayRamp,
  colortype_Constant
} TColorInfoType;

typedef struct {
	double h_min;
	double h_max;
	double s_min;
	double s_max;
	double v_min;
	double v_max;
	double z_min;
	double z_max;
	} TSTMHSVWheelColorInfo;

typedef struct {
	quint32 rgb_min;
	quint32 rgb_max;
	double z_min;
	double z_max;
	} TSTMRGBRampColorInfo;

typedef struct {
	double gray_min;
	double gray_max;
	double z_min;
	double z_max;
	} TSTMGrayRampColorInfo;

typedef struct {
	quint32 value;
	} TSTMConstantColorInfo;

typedef union {
	TSTMHSVWheelColorInfo hsvwheel;
	TSTMRGBRampColorInfo  rgbramp;
	TSTMGrayRampColorInfo  grayramp;
	TSTMConstantColorInfo  constant;
	} TColorInfo;

template< class T >
class NVBIntervalList {
	private:
		QList<double> mins;
		QList<double> maxs;
		QList<T*> objects;

		int getIndexAfter(double pos, int skiphead = 0) const {
			int i;

			for (i = skiphead; i < mins.size(); i++)
				if (pos < mins.at(i)) return i;

			return i;
			}

		int getIndexBefore(double pos, int skiptail = 0) const {
			int i;

			for (i = maxs.size() - 1 - skiptail; i >= 0; i--)
				if (pos > maxs.at(i)) return i;

			return i;
			}

		int getIndexAt(double pos) const {
			int ia = getIndexAfter(pos);
			int ib = getIndexBefore(pos);

			if (ia - ib > 1)
				return ia - 1;
			else
				return -1;
			}

	public:
		NVBIntervalList() {;}
		~NVBIntervalList() {
			while (!objects.isEmpty())
				delete objects.takeFirst();
			}

//   inline double vMin() { return mins.at(0); }
//   inline double vMax() { return maxs.at(maxs.size()-1); }

		void addObject(T* obj, double vmin, double vmax) {
			if (vmax < vmin) {
				double t = vmin;
				vmin = vmax;
				vmax = t;
				}

			if (!objects.size()) {
				mins.append(vmin);
				maxs.append(vmax);
				objects.append(obj);
				return;
				}

			int ia = getIndexAfter(vmin);
			int ib = getIndexBefore(vmax);

			if (ib >= 0 && maxs.at(ib) > vmin) maxs.replace(ib, vmin);

			if (ia < mins.size() && mins.at(ia) < vmax) mins.replace(ia, vmax);

			if (ia - ib > 1) {
				if (mins.at(ib + 1) <= vmin) maxs.replace(++ib, vmin);

				if (maxs.at(ia - 1) >= vmax) mins.replace(--ia, vmax);

				for (int i = ib + 1; i < ia; i++) {
					mins.removeAt(i);
					maxs.removeAt(i);
					delete objects.takeAt(i);
					}
				}

			mins.insert(ib + 1, vmin);
			maxs.insert(ib + 1, vmax);
			objects.insert(ib + 1, obj);
			}

		T* getObjectAt(double pos) const {
			int ia = getIndexAfter(pos);
			int ib = getIndexBefore(pos, maxs.size() - ia);

			if (ia - ib > 1)
				return objects.at(ia - 1);
			else
				return NULL;
			}
		T* takeObjectAt(double pos) const {
			int ia = getIndexAfter(pos);
			int ib = getIndexBefore(pos, maxs.size() - ia);

			if (ia - ib > 1)
				return objects.takeAt(ia - 1);
			else
				return NULL;
			}
	};

/**
This class takes several models for the range
*/
class NVBSetContColorModel : public NVBContColorModel {
		Q_OBJECT
	private:
		NVBIntervalList<NVBContColorModel> models;
		double zmin;
		double zmax;
	protected:
		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }
	public:
		NVBSetContColorModel(): NVBContColorModel() {;}
		virtual ~NVBSetContColorModel() {;}

		virtual QRgb colorize(double z) const {
			if (NVBContColorModel * m = models.getObjectAt(z))
				return m->colorize(z);
			else
				return Qt::black;
			}

		virtual void addColorModel(NVBContColorModel* model, double z_min, double z_max) {
			zmin = qMin(zmin, z_min);
			zmax = qMax(zmax, z_max);
			models.addObject(model, z_min, z_max);
			connect(model, SIGNAL(adjusted()), SIGNAL(adjusted()));
			emit adjusted();
			}
	};

/**
  A color model, where the colors are on a spiral on the HSV wheel
*/
class NVBHSVWheelContColorModel : public NVBContColorModel {
		Q_OBJECT
	private:
		scaler<double, double> *h, *s, *v;

		TSTMHSVWheelColorInfo cInfo;
		void init();
	protected:
		virtual inline double zMin() const { return cInfo.z_min; }
		virtual inline double zMax() const { return cInfo.z_max; }
	public:
		NVBHSVWheelContColorModel();
		NVBHSVWheelContColorModel(double _h_min, double _h_max, double _s_min, double _s_max, double _v_min, double _v_max, double _z_min, double _z_max, short turns = 0);
		NVBHSVWheelContColorModel(TSTMHSVWheelColorInfo cInfo);
		virtual ~NVBHSVWheelContColorModel();

		virtual QRgb colorize(double z) const;
//    virtual QImage * colorize(double * zs, QSize wxh);

		double getHMin() { return cInfo.h_min; };
		double getHMax() { return cInfo.h_max; };
		double getSMin() { return cInfo.s_min; };
		double getSMax() { return cInfo.s_max; };
		double getVMin() { return cInfo.v_min; };
		double getVMax() { return cInfo.v_max; };

	private slots:
		void updatescalers();

	public slots:
		void setHMin(double new_value)
			{ cInfo.h_min = new_value; emit adjusted(); };
		void setHMax(double new_value)
			{ cInfo.h_max = new_value; emit adjusted(); };
		void setSMin(double new_value)
			{ cInfo.s_min = new_value; emit adjusted(); };
		void setSMax(double new_value)
			{ cInfo.s_max = new_value; emit adjusted(); };
		void setVMin(double new_value)
			{ cInfo.v_min = new_value; emit adjusted(); };
		void setVMax(double new_value)
			{ cInfo.v_max = new_value; emit adjusted(); };
	};

/**
This class produces constant coloring
*/
class NVBConstantContColorModel : public NVBContColorModel {
		Q_OBJECT
	private:
		quint32 color;
	protected:
		virtual inline double zMin() const { return DBL_MIN; }
		virtual inline double zMax() const { return DBL_MAX; }
	public:
		NVBConstantContColorModel()
			{ color = 0xFF000000; };
		NVBConstantContColorModel(quint32 _color)
			{ color = _color; };
		NVBConstantContColorModel(TSTMConstantColorInfo cInfo)
			{ color = cInfo.value; };
		virtual ~NVBConstantContColorModel() {};

		virtual QRgb colorize(double) const { return color; };
		virtual QImage * colorize(double *, QSize d_wxh, QSize i_wxh = QSize()) {
			if (!i_wxh.isValid()) i_wxh = d_wxh;

			QImage * t = new QImage(i_wxh, QImage::Format_RGB32);
			t->fill(color);
			return t;
			}
	};

/**
This class makes a simple gray gradient from start to end
*/
class NVBGrayRampContColorModel : public NVBContColorModel {
		Q_OBJECT
	private:
		scaler<double, double> *g;
		void init();
		TSTMGrayRampColorInfo cInfo;
	protected:
		virtual inline double zMin() const { return cInfo.z_min; }
		virtual inline double zMax() const { return cInfo.z_max; }
	public:
		NVBGrayRampContColorModel();
		NVBGrayRampContColorModel(double _g_min, double _g_max, double _z_min, double _z_max);
		NVBGrayRampContColorModel(TSTMGrayRampColorInfo cInfo);
		virtual ~NVBGrayRampContColorModel();

		virtual QRgb colorize(double z) const;
//    virtual QImage * colorize(double * zs, QSize wxh);

	private slots:
		void updatescalers();
	};

/**
This class makes a simple gradient from ARGB_start to ARGB_end
*/
class NVBRGBRampContColorModel : public NVBContColorModel {
		Q_OBJECT
	private:
		scaler<double, quint32> *rgb;
		void init();
		TSTMRGBRampColorInfo cInfo;
	protected:
		virtual inline double zMin() const { return cInfo.z_min; }
		virtual inline double zMax() const { return cInfo.z_max; }
	public:
		NVBRGBRampContColorModel();
		NVBRGBRampContColorModel(double _r_min, double _r_max, double _g_min, double _g_max, double _b_min, double _b_max, double _z_min, double _z_max);
		NVBRGBRampContColorModel(quint32 _rgb_min, quint32 _rgb_max, double _z_min, double _z_max);
		NVBRGBRampContColorModel(TSTMRGBRampColorInfo cInfo);
		virtual ~NVBRGBRampContColorModel();

		virtual QRgb colorize(double z) const;
//    virtual QImage * colorize(double * zs, QSize wxh);

	private slots:
		void updatescalers();
	};

class NVBRescaleColorModel : public NVBContColorModel {
		Q_OBJECT
	protected:
		const NVBContColorModel * source;
		double zmin, zmax;
		scaler<double, double> zscaler;
	protected slots:
		virtual void parentAdjusted();
	public:
		NVBRescaleColorModel(const NVBContColorModel * model);
		NVBRescaleColorModel(double vmin, double vmax);
		virtual ~NVBRescaleColorModel() {;}

		virtual QRgb colorize(double z) const;

		virtual inline double zMin() const { return zmin; }
		virtual inline double zMax() const { return zmax; }
	public slots:
		void setLimits(double _zmin, double _zmax);
		void overrideLimits(double _zmin, double _zmax);
		void setModel(const NVBContColorModel * model);
	};

/**
This class makes a function gradient from ARGB_start to ARGB_end
*/
class NVBRGBFnctContColorModel : public NVBContColorModel {
		Q_OBJECT
		NVBRGBFnctContColorModel() {};
		~NVBRGBFnctContColorModel() {};
	};

/**
This class makes a function gradient set from ARGB_start to ARGB_end
*/
class NVBRGBFnctSetContColorModel : public NVBContColorModel {
		Q_OBJECT
		NVBRGBFnctSetContColorModel() {};
		~NVBRGBFnctSetContColorModel() {};
	};

/**
This class uses a supplied color model to plot the derivative of Z
*/
class NVBdyContColorModel : public NVBContColorModel {
		Q_OBJECT
		NVBdyContColorModel() {};
		~NVBdyContColorModel() {};
	};

/**
This class uses a supplied color model to plot the gradient modulo for Z
*/
class NVBGrainContColorModel : public NVBContColorModel {
		Q_OBJECT
		NVBGrainContColorModel() {};
		~NVBGrainContColorModel() {};
	};

/**
This class uses a look-up table
*/
class NVBTableContColorModel : public NVBContColorModel {
		Q_OBJECT
		NVBTableContColorModel() {};
		~NVBTableContColorModel() {};
	};

}

#endif
