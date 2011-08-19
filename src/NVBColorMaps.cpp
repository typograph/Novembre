//
// C++ Implementation: NVBColoring
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBColorMaps.h"
#include "NVBLogger.h"

QRgb NVBHSVWheelColorMap::colorize( double z ) const
{
	double hf,sf,vf;

	if (z>1) z = 1;
	else if (z<0) z = 0;

	sf = s.scale(z);
	vf = v.scale(z);

	if( sf <= 0.00196 ) {
		// achromatic (grey)
		return (quint32)(0xFF000000 + 0x00FFFFFF*vf);
	}

	double f;
	unsigned char i, p, q, v, t;

	hf = 6*h.scale(z);      // sector 0 to 5
//    printf("%g %g %g\n",hf,sf,vf);
	i = ((int)floor( hf ))%6;
	f = hf - i;      // factorial part of h

	vf*=0xFF;
	sf*=vf;
	p = (uchar)(vf - sf);  // v*(1-s)
	sf*= f;
	q = (uchar)(vf - sf);  // v*(1-f*s)
	t = p + (uchar)(sf);   // v*(1-s*(1-f))
	v = (uchar)vf;

//  printf("%hhd %02hhX %02hhX %02hhX %02hhX\n",i,p,t,q,v);

	switch( i ) {
		case 0:
			return 0xFF000000 + (v << 16) + (t << 8) + p;
		case 1:
			return 0xFF000000 + (q << 16) + (v << 8) + p;
		case 2:
			return 0xFF000000 + (p << 16) + (v << 8) + t;
		case 3:
			return 0xFF000000 + (p << 16) + (q << 8) + v;
		case 4:
			return 0xFF000000 + (t << 16) + (p << 8) + v;
		case 5:
			return 0xFF000000 + (v << 16) + (p << 8) + q;
		default: {
			// throw nvberr_unexpected_value;
			NVBOutputError("Color conversion error");
			NVBOutputError(QString("%1 (%2,%3)\nwas converted to\n(%4->%5) -- your computer is crazy!").arg(z,6).arg(0).arg(1).arg(hf,3).arg(i));
			return 0xFF000000;
			}
		}
}

QRgb NVBGrayRampColorMap::colorize( double z ) const
{
  quint32 u = 0xFF & (uchar)(0xFF*z);
  return (quint32)(0xFF000000 | (u << 16) | (u << 8) | u);
}

NVBRGBRampColorMap::NVBRGBRampColorMap(double r_min, double r_max, double g_min, double g_max, double b_min, double b_max)
: NVBColorMap()
{
  quint32 rgb_min = 0xFF000000 + (((uchar)(r_min*0xFF)) << 16) + (((uchar)(g_min*0xFF)) << 8) + (uchar)(b_min*0xFF);
  quint32 rgb_max = 0xFF000000 + (((uchar)(r_max*0xFF)) << 16) + (((uchar)(g_max*0xFF)) << 8) + (uchar)(b_max*0xFF);
	rgb = NVBValueScaler<double,quint32>(0,1,rgb_min,rgb_max);
}

void NVBGrayStepColorMap::addStep(double x, double value) {
	QList<double>::iterator i = qLowerBound(steps.begin(),steps.end(),x);
	int index = i-steps.begin()-1;
	steps.insert(i,x);

	NVBValueScaler<double,double> old = scales.takeAt(index);
	scales.insert(index,
								NVBValueScaler<double,double>(
									steps.at(index),x,
									old.scale(steps.at(index)),value)
								);
	scales.insert(index+1,
								NVBValueScaler<double,double>(
									x,steps.at(index+1),
									value,old.scale(steps.at(index+1)))
								);
}

QRgb NVBGrayStepColorMap::colorize(double z) const {
	int index = qLowerBound(steps.begin(),steps.end(),z) - steps.begin() - 1;
	if (index == -1) index = 0;

	quint32 u = 0xFF & (uchar)(0xFF*( scales.at(index).scale(z)));
	return (quint32)(0xFF000000 | (u << 16) | (u << 8) | u);
}
