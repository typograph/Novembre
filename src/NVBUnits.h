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

#ifndef NVBUNITS_H
#define NVBUNITS_H

#include <math.h>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QStringList>
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
	static QStringList recognizedUnits;
public:
	/// Constructs an invalid unit
	NVBUnits():base(),mult(1) {;}
	/**
		Construct NVBUnits by deciphering the string into power and unit.
		If scalable is false, the string will not be deciphered, and the unit will be declared absolute.

		\example NVBUnits("kHz") // kilo Herz
		\example NVBUnits("DAC",false) // DAC units
		*/
	NVBUnits( const QString& s, bool scalable = true );
	NVBUnits( const char * s, bool scalable = true );
	/**
		Construct NVBUnits by using the provided string as unit, and the multiplier directly.

		\example NVBUnits("Hz",1000) // kilo Herz
		\example NVBUnits("m",1) // meters
		*/
	NVBUnits( const QString& s, double multiplier):base(s),mult(multiplier) {;}
	~NVBUnits() {;}

	/// NVBUnits is pure when there's no built-in multiplier. E.g. "kHz" are not pure, but "Hz" are.
	bool pure() const { return mult == 0 || mult == 1; }
	/// Makes the unit pure and returns the old factor. /sa NVBUnits::pure()
	double purify() {
		if ( pure() ) return 1;
		dimstr.clear();
		double m = mult; mult = 1; return m;
	}

	/// Outputs the units when multiplied by 10^\p order. 
	QString unitFromOrder(int order) const;

	/** \brief Converts order into SI-style multiplier letter.
	*  E.g. 3 gives "k" and -3 - "m".
	*  Some orders, e.g. 5 cannot be converted. If \p neworder is given, the part that couldn't be converted is returned.
	* \p neworder is always positive.
	*/
	static QChar charFromOrder(int order, int * neworder = 0);
	/// Converts SI multiplier letter into powers of 10. E.g. "k" gives 1000 and "m" gives 0.001.
	static double multFromChar(const QChar & c);

	/** \brief Returns a string representation of the unit
	*  Converts the unit into string, following the SI standard.
	*  In the case that the multiplier does not correspond to a SI letter,
	*  the rest of the multiplier that couldn't be converted is displayed in front
	*  E.g. 10^5 m will convert to "100 km".
	*/
	QString toStr() const;
	/// Returns the unit without multipliers, e.g. "Hz", "m" or "DAC"
	inline QString baseUnit() { return base; }

	/** Unit is scalable when it can have a multiplier. If the constructor 
	*  was called with \p scalable = false, this function will return false.
	*/
	bool isScalable() const { return mult != 0; }
	/**
	* An NVBUnit is invalid if it was constructed without a string.
	* Warning - a unit with empty base is valid.
	*/
	bool isValid() const { return not base.isNull(); }

	/// A unit is comparable with another unit if both have the same base
	bool isComparableWith(const NVBUnits & d) const {
		return base == d.base;
		}

	/// Two units are equal if the base and the multiplier match
	bool operator== (const NVBUnits & d) const {
		return toStr() == d.toStr();
		}

	/** Two units are unequal if they have different bases
	 * Note: this is not the same as not equal, it is the same as not comparable.
	 */
	bool operator!= (const NVBUnits & d) const {
		return base != d.base;
		}

	/// Returns a factor that one should multiply this units to get \p target.
	double scaleTo(const NVBUnits & target) const {
		if (isComparableWith(target)) {
			if (isScalable())
				return target.mult/mult;
			else
				return 1;
			}
		return 0;
		}

	/// Multiply the unit by 10^order
	NVBUnits &  operator+=( int order ) { mult *= exp10(order); dimstr.clear(); return *this; }
	/// Divide the unit by 10^order
	NVBUnits &  operator-=( int order ) { mult /= exp10(order); dimstr.clear(); return *this; }

};

Q_DECLARE_METATYPE(NVBUnits);

class NVBPhysValue {
private:
	NVBUnits dim;
	double value;
	mutable QString valstr;
public:
	NVBPhysValue():value(0) {;}
	NVBPhysValue(const QString& s, bool scalableDimension = true);
	NVBPhysValue(const QString& s, const NVBUnits & d);
	NVBPhysValue(double f, const NVBUnits & d);
	~NVBPhysValue() {;}

	double getValue( ) const { return value; }
	double getValue( const NVBUnits & dim ) const;
	
	NVBUnits getDimension() const { return dim; }

/** Returns a string representation of the value with dimension.
	* The first significant digit will be at least at position \a minSignPos
	* but not later than \a maxSignPos. Positions are counted from log10,
	* meaning that number "1" has first significant digit at 0. The total number
	* of displayed significant digits is given by \a nSDig. Negative values
	* mean that the number will be cut off at the decimal point.
	* Zero means "auto-adjust", i.e. at least one digit will be visible (or I hope so)
	* 
	* Examples:
	* NVBPhysValue v("123456.7 V");
	* v.toString(0); // "123 kV"
	* v.toString(0,5); // "123457 kV"
	* v.toString(0,5,8); // "123456.70 kV"
	* v.toString(0,5,2); // "120000 kV"
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
	bool operator!=( const NVBPhysValue & v ) const { return !operator==(v);}
	bool operator<( const NVBPhysValue & v ) const;
	bool operator<=( const NVBPhysValue & v ) const;
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

	NVBPhysValue abs() const {
		return NVBPhysValue(fabs(value)*dim.mult,dim.base);
		}

};

Q_DECLARE_METATYPE(NVBPhysValue);

class NVBPhysPoint {
	private:
		QPointF p;
		NVBUnits d;
	public:
		NVBPhysPoint() {;}
		NVBPhysPoint(double x, double y, const NVBUnits & dimension);
		NVBPhysPoint(const NVBPhysValue & x, const NVBPhysValue & y);
		NVBPhysPoint(const QPointF & point, const NVBUnits & dimension):p(point),d(dimension) {;}

		~NVBPhysPoint() {;}
		
		inline NVBPhysValue x() const { return NVBPhysValue(p.x(),d); }
		inline NVBPhysValue y() const { return NVBPhysValue(p.y(),d); }
		QPointF point(const NVBUnits & targetDimension) const;
		QPointF vectorTo(const NVBPhysPoint& other) const { return vectorTo(other,d); }
		QPointF vectorTo(const NVBPhysPoint& other, const NVBUnits & targetDimension) const;
		
		NVBPhysValue distance(const NVBPhysPoint& other);

		NVBUnits dimension() const { return d; }
		operator QPointF () { return p; }

		NVBPhysPoint operator+(const QPointF & other) const {
			return NVBPhysPoint(p + other, d);
			}
			
		NVBPhysPoint operator-(const NVBPhysPoint & other) const {
			return NVBPhysPoint(p - other.point(d), d);
			}

		void operator+=(const QPointF & other) {
			p += other;
			}

		void operator-=(const QPointF & other) {
			p -= other;
			}

};

Q_DECLARE_METATYPE(NVBPhysPoint);

#endif
