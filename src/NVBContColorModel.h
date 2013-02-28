//
// C++ Interface: NVBContColorModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBCONTCOLORMODEL_H
#define NVBCONTCOLORMODEL_H

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include "dimension.h"

namespace NVBColoring {

/**
*/
class NVBContColorModel : public QObject{
Q_OBJECT
protected:
public:
    NVBContColorModel() {;}
    virtual ~NVBContColorModel() {;}

    virtual double zMin() const =0;
    virtual double zMax() const =0;

    virtual QRgb colorize(double z) const = 0;
    virtual QImage * colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const {
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
            double z = zs[w.scaleInt(i)+ d_wxh.width()*h.scaleInt(j)];
            if (FINITE(z))
              result->setPixel(i,j,colorize(z));
            }
        }
      else {
        for (int i = 0; i<i_wxh.width(); i++)
          for (int j = 0; j<i_wxh.height(); j++) {
            double z = zs[i+i_wxh.width()*j];
            if (FINITE(z))
              result->setPixel(i,j,colorize(z));
            }
        }
        
      return result;
      }

signals:
    void adjusted();
};

}
#endif
