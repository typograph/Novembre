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
	if (z > 1) return 0xFFFFFFFF;
	if (z < 0) return 0xFF000000;
	quint32 u = 0xFF & (uchar)(0xFF*z);
  return (quint32)(0xFF000000 | (u << 16) | (u << 8) | u);
}

NVBRGBRampColorMap::NVBRGBRampColorMap(double r_min, double r_max, double g_min, double g_max, double b_min, double b_max)
: NVBColorMap()
{
	r = NVBValueScaler<double,quint8>(0,1,(uchar)(r_min*0xFF),(uchar)(r_max*0xFF));
	g = NVBValueScaler<double,quint8>(0,1,(uchar)(g_min*0xFF),(uchar)(g_max*0xFF));
	b = NVBValueScaler<double,quint8>(0,1,(uchar)(b_min*0xFF),(uchar)(b_max*0xFF));
}

NVBRGBRampColorMap::NVBRGBRampColorMap(quint32 rgb_min, quint32 rgb_max)
: NVBColorMap()
, r(NVBValueScaler<double,quint8>(0,1,(rgb_min & 0xFF0000) >> 16,(rgb_max & 0xFF0000) >> 16))
, g(NVBValueScaler<double,quint8>(0,1,(rgb_min & 0xFF00) >> 8,(rgb_max & 0xFF00) >> 8))
, b(NVBValueScaler<double,quint8>(0,1,(rgb_min & 0xFF),(rgb_max & 0xFF)))
{;}

void NVBRGBRampColorMap::setStart(quint32 start) {
	r.change_output(r.scale(0),r.scale(1),(start & 0xFF0000) >> 16,r.scale(1));
	g.change_output(g.scale(0),g.scale(1),(start & 0xFF00) >> 8,g.scale(1));
	b.change_output(b.scale(0),b.scale(1),(start & 0xFF),b.scale(1));
}

void NVBRGBRampColorMap::setEnd(quint32 end) {
	r.change_output(r.scale(0),r.scale(1),r.scale(0),(end & 0xFF0000) >> 16);
	g.change_output(g.scale(0),g.scale(1),g.scale(0),(end & 0xFF00) >> 8);
	b.change_output(b.scale(0),b.scale(1),b.scale(0),(end & 0xFF));
}

QRgb NVBRGBRampColorMap::colorize(double z) const {
	if (z < 0) z = 0;
	if (z > 1) z = 1;
	return 0xFF000000 | r.scale(z) << 16 | g.scale(z) << 8 | b.scale(z);
}

void NVBGrayStepColorMap::addStep(double x, double value) {
	if (x < 0) return; // Invalid step position
	if (x > 1) return; // Invalid step position
	QList<double>::iterator i = qLowerBound(steps.begin(),steps.end(),x);
	int index = i-steps.begin()-1;

	if (*i == x) {
		if (index >= 0)
			scales[index].reinit(
						steps.at(index),steps.at(index+1),
						scales.at(index).scale(steps.at(index)), value);
		if (index + 1 < scales.count())
			scales[index+1].reinit(
						steps.at(index+1), steps.at(index+2),
						value, scales.at(index+1).scale(steps.at(index+2)));
		}
	else {
		steps.insert(i,x);

		NVBValueScaler<double,double> old = scales.takeAt(index);
		scales.insert(index,
									NVBValueScaler<double,double>(
										steps.at(index),x,
										old.scale(steps.at(index)),value)
									);
		scales.insert(index+1,
									NVBValueScaler<double,double>(
										x,steps.at(index+2),
										value,old.scale(steps.at(index+2)))
									);
		}
}

QRgb NVBGrayStepColorMap::colorize(double z) const {
	if (z < steps.first()) return 0xFF000000;
	if (z > steps.last()) return 0xFFFFFFFF;
	int index = qLowerBound(steps.begin(),steps.end(),z) - steps.begin() - 1;
	if (index == -1) index = 0;

	quint32 u = 0xFF & (uchar)(0xFF*( scales.at(index).scale(z)));
	return (quint32)(0xFF000000 | (u << 16) | (u << 8) | u);
}
