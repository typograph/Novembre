//
// C++ Interface: NVBDimension
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
#include "NVBLogger.h"

#ifndef exp10
#define exp10(x) pow(10,x)
#endif

class NVBDimension {
private:
  QString base;
  double mult;
  mutable QString dimstr;
public:
  NVBDimension():base(),mult(1) {;}
  /**
    Construct NVBDimension by deciphering the string into power and unit.
    If scalable is false, the string will not be deciphered, and the unit will be declared absolute.

    example NVBDimension("kHz") // kilo Herz
    example NVBDimension("DAC",false) // DAC units
    */
  NVBDimension( const QString& s, bool scalable = true );
  /**
    Construct NVBDimension by using the provided string as unit and the multiplier.

    example NVBDimension("Hz",1000) // kilo Herz
    example NVBDimension("m",1) // meters
    */
  NVBDimension( const QString& s, double multiplier):base(s),mult(multiplier) {;}
  ~NVBDimension() {;}

  bool pure() const { return mult == 0 || mult == 1; }
  double purify() {
    if ( pure() ) return 1;
    double m = mult; mult = 1; return m;
  }

  QString unitFromOrder(int order) const;

  static QChar charFromOrder(int order, int * neworder = 0);
  static double multFromChar(QChar c);

  QString toStr() const;

  bool isScalable() const { return mult != 0; }

  bool isComparableWith(const NVBDimension & d) const {
    return base == d.base;
    }

  bool operator== (const NVBDimension & d) const {
    return toStr() == d.toStr();
    }

  bool operator!= (const NVBDimension & d) const {
    return base != d.base;
    }

  NVBDimension &  operator+=( int order ) { mult *= exp10(order); return *this; }
  NVBDimension &  operator-=( int order )  { mult /= exp10(order); return *this; }

};

Q_DECLARE_METATYPE(NVBDimension);

class NVBPhysValue {
private:
  NVBDimension dim;
  double value;
  mutable QString valstr;
public:
  NVBPhysValue():value(0) {;}
	NVBPhysValue(const QString& s, bool scalableDimension = true);
  NVBPhysValue(const QString& s, NVBDimension d);
  NVBPhysValue(double f, NVBDimension d);
  ~NVBPhysValue() {;}

  double getValue( ) const { return value; }
  double getValue( NVBDimension dim ) const;
  
  NVBDimension getDimension() const { return dim; }

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
  
//  NVBPhysValue operator+(const NVBPhysValue &value_1, const NVBPhysValue &value_2);

  NVBPhysValue &  operator*=( double mult ) { value *= mult; return *this; }
  NVBPhysValue &  operator/=( double div )  {  value /= div; return *this; }

};

Q_DECLARE_METATYPE(NVBPhysValue);

#endif
