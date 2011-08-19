//
// C++ Interface: NVBColoring
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBCOLORMAPS_H
#define NVBCOLORMAPS_H

#include "NVBScaler.h"
#include "NVBMap.h"
#include "QtCore/qglobal.h"

/**
	A color map, where the colors are on a spiral on the HSV wheel
*/
class NVBHSVWheelColorMap : public NVBColorMap{
	private:
		NVBValueScaler<double,double> h,s,v;
	public:
		NVBHSVWheelColorMap(double h_min, double h_max, double s_min, double s_max, double v_min, double v_max, short turns = 0)
			: h(NVBValueScaler<double,double>(0,1,h_min,h_max+turns))
			, s(NVBValueScaler<double,double>(0,1,s_min,s_max))
			, v(NVBValueScaler<double,double>(0,1,v_min,v_max))
			{;}
			
		virtual ~NVBHSVWheelColorMap() {;}

		virtual QRgb colorize(double z) const;
		
		virtual NVBHSVWheelColorMap * copy() { return new NVBHSVWheelColorMap(h.scale(0),h.scale(1),s.scale(0),s.scale(1),v.scale(0),v.scale(1)); }
};

/**
This class makes a simple gray gradient from 0 to 1
*/
class NVBGrayRampColorMap : public NVBColorMap{
	public:
		NVBGrayRampColorMap()	{;}
			
		virtual ~NVBGrayRampColorMap() {;}
		
		virtual QRgb colorize(double z) const;

		virtual NVBGrayRampColorMap * copy() { return new NVBGrayRampColorMap(); }

};

/**
This class makes a gray gradient with multiple steps
*/
class NVBGrayStepColorMap : public NVBColorMap{
	private:
		QList<double> steps;
		QList< NVBValueScaler<double,double> > scales;

		NVBGrayStepColorMap(const QList<double> & xs, const QList< NVBValueScaler<double,double> > & scalers)
			: steps(xs)
			, scales(scalers)
			{;}
	public:
		//
		NVBGrayStepColorMap(double start, double end)	{
			steps.append(0);
			steps.append(1);
			scales.append(NVBValueScaler<double,double>(0,1,start,end));
			}

		NVBGrayStepColorMap(const QList<double> & xs, const QList<double> & values)
			: steps(xs)
			{
			for (int i=1; i<values.count(); i++)
				scales.append(NVBValueScaler<double,double>(xs.at(i-1),xs.at(i),values.at(i-1),values.at(i)));
			}

		virtual ~NVBGrayStepColorMap() {;}

		void addStep(double x, double value);

		virtual QRgb colorize(double z) const;

		virtual NVBGrayStepColorMap * copy() { return new NVBGrayStepColorMap(steps,scales); }

};

/**
This class makes a simple gradient from ARGB_start to ARGB_end
*/
class NVBRGBRampColorMap : public NVBColorMap {
	private:
		NVBValueScaler<double,quint32> rgb;
	public:
		NVBRGBRampColorMap(double r_min, double r_max, double g_min, double g_max, double b_min, double b_max);
		NVBRGBRampColorMap(quint32 rgb_min, quint32 rgb_max)
			: NVBColorMap()
			, rgb(NVBValueScaler<double,quint32>(0,1,rgb_min,rgb_max))
			{;}
		virtual ~NVBRGBRampColorMap() {;}

		virtual QRgb colorize(double z) const { return rgb.scale(z); }
		
		virtual NVBRGBRampColorMap * copy() { return new NVBRGBRampColorMap(rgb.scale(0),rgb.scale(1));}
};

/**
	This class mixes three channels into rgb
	*/
class NVBRGBMixColorMap : public NVBColorMap {
	private:
		NVBColorMap *r, *g, *b;
	public:
		NVBRGBMixColorMap(NVBColorMap *red, NVBColorMap *green, NVBColorMap *blue)
			: r(red)
			, g(green)
			, b(blue)
		{
			if (!r) {
				NVBOutputError("No red");
				r = new NVBGrayRampColorMap();
			}
			if (!g) {
				NVBOutputError("No green");
				g = new NVBGrayRampColorMap();
			}
			if (!b) {
				NVBOutputError("No blue");
				b = new NVBGrayRampColorMap();
			}
		}
		virtual ~NVBRGBMixColorMap() {
			delete r;
			delete g;
			delete b;
			}

		virtual QRgb colorize(double z) const {
			return (r->colorize(z) & 0xFF) + (g->colorize(z) & 0xFF00) + (b->colorize(z) & 0xFF0000) + 0xFF000000;
		}

		virtual NVBRGBMixColorMap * copy() { return new NVBRGBMixColorMap(r->copy(),g->copy(),b->copy()); }
};

/**
This class uses a look-up table
*/
class NVBTableColorMap : public NVBColorMap {
		NVBTableColorMap() {};
		~NVBTableColorMap() {};
};


#endif
