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
#include "NVBContColoring.h"
#include "NVBLogger.h"

namespace NVBColoring {

void NVBHSVWheelContColorModel::init()
{
  memset(&cInfo,0,sizeof(cInfo));
  
  h = NULL;
  s = NULL;
  v = NULL;

  connect(this,SIGNAL(adjusted()),this,SLOT(updatescalers()));
}

NVBHSVWheelContColorModel::NVBHSVWheelContColorModel()
{
  init();
}

NVBHSVWheelContColorModel::NVBHSVWheelContColorModel( double _h_min, double _h_max, double _s_min, double _s_max, double _v_min, double _v_max, double _z_min, double _z_max, short turns )
{
  init();
  cInfo.h_min = _h_min;
  cInfo.h_max = _h_max + turns;
  cInfo.s_min = _s_min;
  cInfo.s_max = _s_max;
  cInfo.v_min = _v_min;
  cInfo.v_max = _v_max;
  cInfo.z_min = _z_min;
  cInfo.z_max = _z_max;  

  emit adjusted();
}

NVBHSVWheelContColorModel::NVBHSVWheelContColorModel(TSTMHSVWheelColorInfo _cInfo)
{
  init();
  cInfo = _cInfo;
  emit adjusted();
}

NVBHSVWheelContColorModel::~NVBHSVWheelContColorModel()
{
  if (h) delete h;
  if (s) delete s;
  if (v) delete v;
}

QRgb NVBHSVWheelContColorModel::colorize( double z ) const
{
  if (h && s && v) {
    double hf,sf,vf;
    
    if (z>cInfo.z_max) z = cInfo.z_max;
    else if (z<cInfo.z_min) z = cInfo.z_min;
    
    sf = s->scale(z);
    vf = v->scale(z);
  
    if( sf <= 0.00196 ) {
      // achromatic (grey)
      return (quint32)(0xFF000000 + 0x00FFFFFF*vf);
    }
  
    double f;
    unsigned char i, p, q, v, t;
  
    hf = 6*h->scale(z);      // sector 0 to 5
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
        NVBOutputError( "NVBHSVWheelContColorModel::colorize", "Color conversion error");
        NVBOutputError( "NVBHSVWheelContColorModel::colorize", "%6g (%d,%d)\nwas converted to\n(%3g->%hhu) -- your computer is crazy!",z,cInfo.z_min,cInfo.z_max,hf,i);
        return 0xFF000000;
        }
      }
    }
  else
    return 0xFF000000;
  
}

void NVBHSVWheelContColorModel::updatescalers( )
{
  if (h) { delete h; }
  h = new scaler<double,double>(cInfo.z_min,cInfo.z_max,cInfo.h_min,cInfo.h_max);
  if (s) { delete s; }
  s = new scaler<double,double>(cInfo.z_min,cInfo.z_max,cInfo.s_min,cInfo.s_max);
  if (v) { delete v; }
  v = new scaler<double,double>(cInfo.z_min,cInfo.z_max,cInfo.v_min,cInfo.v_max);
}

void NVBGrayRampContColorModel::init()
{
  memset(&cInfo,0,sizeof(cInfo));
  
  g = NULL;
  
  connect(this,SIGNAL(adjusted()),this,SLOT(updatescalers()));
}

NVBGrayRampContColorModel::NVBGrayRampContColorModel()
{
  init();
}

NVBGrayRampContColorModel::NVBGrayRampContColorModel(double _g_min, double _g_max, double _z_min, double _z_max)
{
  init();
  cInfo.gray_min = _g_min;
  cInfo.gray_max = _g_max;
  cInfo.z_min = _z_min;
  cInfo.z_max = _z_max;  

  emit adjusted();
}

NVBGrayRampContColorModel::NVBGrayRampContColorModel(TSTMGrayRampColorInfo _cInfo)
{
  init();
  cInfo = _cInfo;
  emit adjusted();
}

NVBGrayRampContColorModel::~NVBGrayRampContColorModel()
{
  if (g) delete g;
}

void NVBGrayRampContColorModel::updatescalers( )
{
  if (g) { delete g; }
  g = new scaler<double,double>(cInfo.z_min,cInfo.z_max,cInfo.gray_min,cInfo.gray_max);
}

QRgb NVBGrayRampContColorModel::colorize( double z ) const
{
  double v;

  if (z < cInfo.z_min)
    v = cInfo.gray_min;
  else if (z > cInfo.z_max)
    v = cInfo.gray_max;
  else if (g)
    v = g->scale(z);
  else v = 0;

  quint32 u = 0xFF & (uchar)(0xFF*v);
  return (quint32)(0xFF000000 | (u << 16) | (u << 8) | u);
}

void NVBRGBRampContColorModel::init()
{
  memset(&cInfo,0,sizeof(cInfo));
  
  rgb = NULL;
  
  connect(this,SIGNAL(adjusted()),this,SLOT(updatescalers()));
}

NVBRGBRampContColorModel::NVBRGBRampContColorModel()
{
  init();
}

NVBRGBRampContColorModel::NVBRGBRampContColorModel(double _r_min, double _r_max, double _g_min, double _g_max, double _b_min, double _b_max, double _z_min, double _z_max)
{
  init(); // TODO float vs. int thing -> leads to add. precision
  cInfo.rgb_min = 0xFF000000 + (((uchar)(_r_min*0xFF)) << 16) + (((uchar)(_g_min*0xFF)) << 8) + (uchar)(_b_min*0xFF);
  cInfo.rgb_max = 0xFF000000 + (((uchar)(_r_max*0xFF)) << 16) + (((uchar)(_g_max*0xFF)) << 8) + (uchar)(_b_max*0xFF);
  cInfo.z_min = _z_min;
  cInfo.z_max = _z_max;  

  emit adjusted();
}

NVBRGBRampContColorModel::NVBRGBRampContColorModel(quint32 _rgb_min, quint32 _rgb_max, double _z_min, double _z_max)
{
  init();
  cInfo.rgb_min = _rgb_min;
  cInfo.rgb_max = _rgb_max;
  cInfo.z_min = _z_min;
  cInfo.z_max = _z_max;

  emit adjusted();
}

NVBRGBRampContColorModel::NVBRGBRampContColorModel(TSTMRGBRampColorInfo _cInfo)
{
  init();
  cInfo = _cInfo;
  emit adjusted();
}

NVBRGBRampContColorModel::~NVBRGBRampContColorModel()
{
  if (rgb) delete rgb;
}

void NVBRGBRampContColorModel::updatescalers( )
{
  if (rgb) { delete rgb; }
  rgb = new scaler<double,quint32>(cInfo.z_min,cInfo.z_max,cInfo.rgb_min,cInfo.rgb_max);
}

QRgb NVBRGBRampContColorModel::colorize( double z ) const
{
  if (rgb) {
    return rgb->scale(z);
    }
  else
    return 0xFF000000;
}
/*
QImage * NVBRGBRampContColorModel::colorize( double * zs, QSize wxh )
{
  QImage * result = new QImage(wxh,QImage::Format_ARGB32);
  if (!result) throw nvberr_not_enough_memory;
  for (int i = 0; i<wxh.width(); i++)
    for (int j = 0; j<wxh.height(); j++) {
        result->setPixel(i,j,colorize(zs[i*wxh.width()+j]));
      }
  return result;
}
*/


NVBRescaleColorModel::NVBRescaleColorModel(const NVBContColorModel * model):NVBContColorModel(),source(0)
{
  if (!model) throw;
  zmin = model->zMin();
  zmax = model->zMax();
  setModel(model);
}

NVBRescaleColorModel::NVBRescaleColorModel(double vmin, double vmax)
  : NVBContColorModel(),source(0),zmin(vmin),zmax(vmax)
{
}

void NVBRescaleColorModel::setLimits(double vmin, double vmax)
{
  zscaler.change_input(zmin,zmax,vmin,vmax);

  zmin = vmin;
  zmax = vmax;

  emit adjusted();
}

void NVBRescaleColorModel::overrideLimits(double vmin, double vmax)
{
  zmin = vmin;
  zmax = vmax;
}

void NVBRescaleColorModel::setModel(const NVBContColorModel * model)
{
  if (source)
    disconnect(source,0,this,0);

  source = model;

  if (source) {
    connect(source,SIGNAL(adjusted()),SLOT(parentAdjusted()));
//     setLimits(model->zMin(),model->zMax());
    parentAdjusted();
    }
//   else
//     zmin = zmax = 0;
}

QRgb NVBRescaleColorModel::colorize(double z) const
{
  if (source)
    return source->colorize(zscaler.scale(z));
  else
    return Qt::black;
}

void NVBRescaleColorModel::parentAdjusted()
{
  zscaler = scaler<double,double>(zmin,zmax,source->zMin(),source->zMax());
  emit adjusted();
}

}
