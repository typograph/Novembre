//
// C++ Interface: QDimension
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef QDIMENSION_H
#define QDIMENSION_H

#include "dimension.h"
#include <math.h>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QSizeF>
#include <QPointF>
#include <QRectF>

class QScaler : public scaler<double,double>
{
public:
  QScaler(double offset,double multiplier):scaler<double,double>(offset,multiplier) {;}
  QScaler(double from0, double to0, double multiplier):scaler<double,double>(to0-from0*multiplier,multiplier) {;}
  QScaler(double f0,double f1,double t0, double t1):scaler<double,double>(f0,f1,t0,t1) {;}
  
};

class Q2DScaler {

protected:
  QPointF fromP, toP;
  QSizeF fromS, toS;
  double multW, multH;

  QPointF squeezescale(QPointF pt) const {return QPointF(pt.x()*multW,pt.y()*multH);}
  QSizeF squeezescale(QSizeF sz) const {return QSizeF(sz.width()*multW,sz.height()*multH);}

  void setMult();

public:
  Q2DScaler();
  Q2DScaler(QRectF,QRectF);
  virtual ~Q2DScaler() {;}

  virtual bool inflates();
  virtual bool deflates();
  virtual bool onetoone();

  virtual QRectF rectFrom() const { return QRectF(fromP,fromS);}
  virtual QRectF rectTo() const { return QRectF(toP,toS);}

  virtual Q2DScaler * backScaler() { return new Q2DScaler(QRectF(toP,toS),QRectF(fromP,fromS));}

  virtual QScaler XScaler() {return QScaler(fromP.x(),toP.x(),multW);}
  virtual QScaler YScaler() {return QScaler(fromP.y(),toP.y(),multH);}
  
  virtual QPointF scale(QPointF pt) const { return toP+squeezescale(pt-=fromP);}
  virtual QSizeF scale(QSizeF sz) const { return squeezescale(sz); }
  virtual QRectF scale(QRectF rect) const { return QRectF(scale(rect.topLeft()),scale(rect.size()));}
  
  virtual QPointF * scale(QPointF * pt) const;
  virtual QSizeF * scale(QSizeF * sz) const;
  virtual QRectF * scale(QRectF * rect) const;

  virtual QPoint scale(QPoint pt) const { return scale(QPointF(pt)).toPoint();}
  virtual QSize scale(QSize sz) const { return scale(QSizeF(sz)).toSize();}
  virtual QRect scale(QRect rect) const {  return QRect(scale(rect.topLeft()),scale(rect.size())); }
    
  virtual QPoint * scale(QPoint * pt) const;
  virtual QSize * scale(QSize * sz) const;
  virtual QRect * scale(QRect * rect) const;
    
  virtual void setFrom(QPointF pt) { fromP = pt;}
  virtual void setFrom(QSizeF sz) { fromS = sz; setMult(); }
  virtual void setFrom(QRectF);
  virtual void setFrom(QRect rect) {setFrom(QRectF(rect));}
  virtual void shiftFrom(QPointF pt) { fromP += pt;}
  virtual void scaleFrom(double factor) { fromS *= factor; setMult(); }
  
  virtual void setTo(QPointF pt) { toP = pt; }
  virtual void setTo(QSizeF sz) { toS = sz; setMult(); }
  virtual void setTo(QRectF);
  virtual void setTo(QRect rect) { setTo(QRectF(rect));}
  virtual void shiftTo(QPointF pt) { toP += pt; }
  virtual void scaleTo(double factor, QPointF ref = QPointF());

};

class Q2DSqScaler : public Q2DScaler {

public:
  Q2DSqScaler();
  Q2DSqScaler(QRectF,QRectF);
  Q2DSqScaler(QPointF,double,QPointF,double);
  virtual ~Q2DSqScaler() {;}
  
  virtual Q2DSqScaler * backScaler() { return new Q2DSqScaler(QRectF(toP,toS),QRectF(fromP,fromS));}
  
  virtual void setFrom(QRectF);
  virtual void setTo(QRectF);
};

#endif
