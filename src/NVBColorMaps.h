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
};

/**
This class makes a simple gray gradient from start to end
*/
class NVBGrayRampColorMap : public NVBColorMap{
	public:
    NVBGrayRampColorMap()	{;}
			
    virtual ~NVBGrayRampColorMap() {;}
    
    virtual QRgb colorize(double z) const;
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
};

/**
This class uses a look-up table
*/
class NVBTableColorMap : public NVBColorMap {
    NVBTableColorMap() {};
    ~NVBTableColorMap() {};
};


#endif
