//
// C++ Interface: NVBUnits
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBDIMENSION_H
#define NVBDIMENSION_H

#include <math.h>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QPointF>
#include "NVBLogger.h"

#ifndef exp10
#define exp10(x) pow(10,x)
#endif

class NVBUnits {
friend class NVBPhysValue;
private:
  QString base;
  double mult;
  mutable QString dimstr;
public:
  NVBUnits():base(),mult(1) {;}
  /**
    Construct NVBUnits by deciphering the string into power and unit.
    If scalable is false, the string will not be deciphered, and the unit will be declared absolute.

    \example NVBUnits("kHz") // kilo Herz
    \example NVBUnits("DAC",false) // DAC units
    */
  NVBUnits( const QString& s, bool scalable = true );
  /**
    Construct NVBUnits by using the provided string as unit and the multiplier.

    \example NVBUnits("Hz",1000) // kilo Herz
    \example NVBUnits("m",1) // meters
    */
  NVBUnits( const QString& s, double multiplier):base(s),mult(multiplier) {;}
  ~NVBUnits() {;}

  bool pure() const { return mult == 0 || mult == 1; }
  double purify() {
    if ( pure() ) return 1;
    double m = mult; mult = 1; return m;
  }

  QString unitFromOrder(int order) const;

  static QChar charFromOrder(int order, int * neworder = 0);
  static double multFromChar(QChar c);

  QString toStr() const;
	inline QString baseUnit() { return base; }

  bool isScalable() const { return mult != 0; }

  bool isComparableWith(const NVBUnits & d) const {
    return base == d.base;
    }

  bool operator== (const NVBUnits & d) const {
    return toStr() == d.toStr();
    }

  bool operator!= (const NVBUnits & d) const {
    return base != d.base;
    }

  NVBUnits &  operator+=( int order ) { mult *= exp10(order); return *this; }
  NVBUnits &  operator-=( int order )  { mult /= exp10(order); return *this; }

};

Q_DECLARE_METATYPE(NVBUnits);

class NVBPhysValue {
private:
  NVBUnits dim;
  double value;
  mutable QString valstr;
public:
  NVBPhysValue():value(0) {;}
  NVBPhysValue(const QString& s);
  NVBPhysValue(const QString& s, NVBUnits d);
  NVBPhysValue(double f, NVBUnits d);
  ~NVBPhysValue() {;}

  double getValue( ) const { return value; }
  double getValue( NVBUnits dim ) const;
  
  NVBUnits getDimension() const { return dim; }

/** Returns a string representation of the value with dimension.
    The first significant digits will be at least at position \a minSignPos
    but not earlier than \a maxSignPos. Positions are counted from log10,
    meaning that number "1" has first significant digit at 0. The total number
    of significant digits is given by \a nSDig. Negative values mean that the number
    will be cut off at the decimal point. Zero means "auto-adjust", i.e.
    at least one digit will be visible (or I hope so)
  */ 
  QString toString(int minSignPos, int maxSignPos = 2, int nSDig = -1) const;
/** Overloaded version working through cache
  */  
  QString toString() const;
/**
  Returns a string with the unit multiplied by 1e+order. To get e.g. kilograms
  put in 3.
  */
  QString toStringWithOrder(int order) const ;
/**
  Prints all available digits with the base unit (g, Hz or V).
  */
  QString toFullString() const { return QString("%1 %2").arg(value).arg(dim.unitFromOrder(0)); }
/**
  Prints the value with SI units. Not implemented.
  */
  QString toSIString() const ;

  static int getPosMult(double value, int minSignPos = 1, int maxSignPos = 3);

  bool operator==( const NVBPhysValue & v ) const {
    return value == v.getValue() && getDimension() == v.getDimension();
    }
  bool operator!=( const NVBPhysValue & v ) const { return operator==(v);}
  bool operator<( const NVBPhysValue & v ) const {
    return getDimension() == v.getDimension() && getValue() < v.getValue();
    }
  bool operator<=( const NVBPhysValue & v ) const { return operator==(v) || operator <(v); }
  bool operator>( const NVBPhysValue & v ) const { return !operator <=(v); }
  bool operator>=( const NVBPhysValue & v ) const { return !operator <(v);}
  
  NVBPhysValue operator+(const NVBPhysValue &other) const {
    if (dim.isComparableWith(other.dim)) {
      return NVBPhysValue(value*dim.mult + other.value*other.dim.mult,dim.base);
			}
    NVBOutputError("Attempting to add values with different dimensions");
    return NVBPhysValue();
    }

  NVBPhysValue operator-(const NVBPhysValue &other) const {
    if (dim.isComparableWith(other.dim)) {
      return NVBPhysValue(value*dim.mult - other.value*other.dim.mult,dim.base);
			}
    NVBOutputError("Attempting to subtract values with different dimensions");
    return NVBPhysValue();
    }

  NVBPhysValue &  operator*=( double mult ) { value *= mult; return *this; }
  NVBPhysValue &  operator/=( double div )  {  value /= div; return *this; }

};

Q_DECLARE_METATYPE(NVBPhysValue);

class NVBPhysPoint {
  private:
    QPointF p;
    NVBUnits d;
  public:
		NVBPhysPoint() {;}
		NVBPhysPoint(double x, double y, NVBUnits dimension);
    NVBPhysPoint(NVBPhysValue,NVBPhysValue);
    NVBPhysPoint(QPointF point, NVBUnits dimension):p(point),d(dimension) {;}

    NVBPhysPoint(const NVBPhysPoint & other):p(other.p),d(other.d) {;}

    ~NVBPhysPoint() {;}
    
    inline NVBPhysValue x() const { return NVBPhysValue(p.x(),d); }
    inline NVBPhysValue y() const { return NVBPhysValue(p.y(),d); }
    QPointF point(NVBUnits targetDimension) const;
		QPointF vectorTo(const NVBPhysPoint& other, NVBUnits targetDimension = NVBUnits()) const;
		
		NVBPhysValue distance(const NVBPhysPoint& other);

    NVBUnits dimension() const { return d; }
    operator QPointF () { return p; }
 
		NVBPhysPoint operator+(const QPointF & other) const {
			return NVBPhysPoint(p + other, d);
			}
			
    void operator+=(const QPointF & other) {
			p += other;
			}
			
    NVBPhysPoint operator-(const NVBPhysPoint & other) const {
			return NVBPhysPoint(p - other.point(d), d);
			}
};

Q_DECLARE_METATYPE(NVBPhysPoint);

#endif
