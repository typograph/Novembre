//
// C++ Implementation: NVBDimension
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdlib.h>
#include <QtCore/QRegExp>
#include "NVBDimension.h"

QChar NVBDimension::charFromOrder(int order, int * neworder)
{
  int no;
  if (!neworder) neworder = &no;
  *neworder = 0;
  switch (order) {
    case 24 : return 'Y';
    case 23 : *neworder += 1;
    case 22 : *neworder += 1;
    case 21 : return 'Z';
    case 20 : *neworder += 1;
    case 19 : *neworder += 1;
    case 18 : return 'E';
    case 17 : *neworder += 1;
    case 16 : *neworder += 1;
    case 15 : return 'P';
    case 14 : *neworder += 1;
    case 13 : *neworder += 1;
    case 12 : return 'T';
    case 11 : *neworder += 1;
    case 10 : *neworder += 1;
    case 9 : return 'G';
    case 8 : *neworder += 1;
    case 7 : *neworder += 1;
    case 6 : return 'M';
    case 5 : *neworder += 1;
    case 4 : *neworder += 1;
    case 3 : return 'k';
    case 2 : return 'h';
    case 1 : *neworder += 1;
    case 0 : return QChar();
    case -1 : return 'd';
    case -2 : return 'c';
    case -3 : return 'm';
    case -4 : *neworder += 1;
    case -5 : *neworder += 1;
    case -6 : return 'u';
    case -7 : *neworder += 1;
    case -8 : *neworder += 1;
    case -9 : return 'n';
    case -10 : *neworder += 1;
    case -11 : *neworder += 1;
    case -12 : return 'p';
    case -13 : *neworder += 1;
    case -14 : *neworder += 1;
    case -15 : return 'f';
    case -16 : *neworder += 1;
    case -17 : *neworder += 1;
    case -18 : return 'a';
    case -19 : *neworder += 1;
    case -20 : *neworder += 1;
    case -21 : return 'z';
    case -22 : *neworder += 1;
    case -23 : *neworder += 1;
    case -24 : return 'y';
    default : return '?';
    }
}

double NVBDimension::multFromChar(QChar c)
{
    switch(c.toAscii()) { // Greek
      case 'Y'    : return 1e+24; // yotta-
      case 'Z'    : return 1e+21; // zetta-
      case 'E'    : return 1e+18; // exa-
      case 'P'    : return 1e+15;
      case 'T'    : return 1e+12; // tera-
      case 'G'    : return 1e+09; // giga-
      case 'M'    : return 1e+06; // mega-
      case 'k'    : return 1e+03; // kilo-
      case 'h'    : return 1e+02; // hecto-
      case 'd'    : return 1e-01; // deci- ( deca ignored )
      case 'c'    : return 1e-02; // centi-
      case 'm'    : return 1e-03; // milli-
      case 'u'    : return 1e-06;  // mikro- v1
      case 'n'    : return 1e-09; // nano-
      case 'p'    : return 1e-12; // pico-
      case 'f'    : return 1e-15;  // femto-
      case 'a'    : return 1e-18; // atto-
      case 'z'    : return 1e-21; // zepto-
      case 'y'    : return 1e-24; // yocto-
      }
  switch (c.unicode()) {
      case 0      : return 1;
      case 0x03BC : return 1e-06; // mikro- v2
			default     : return 0;
      }
}

NVBDimension::NVBDimension(const QString& s, bool scalable):mult(1)
{
	base = s.trimmed().simplified();
  if (scalable) {
		if (base == "hz") return;
    if (base.length() > 1 && base.at(1).isLetter()) {
      mult = multFromChar(base.at(0));
			if (mult != 0)
				base = base.mid(1);
			else
				mult = 1;
      }
    }
  else {
		mult = 0;
    dimstr = base;
  }
}

QString NVBDimension::unitFromOrder(int order) const
{
  if (mult == 0) return base;
  order += (int) floor(log10(fabs(mult)));
  return order ? QString("%1%2").arg(charFromOrder(order)).arg(base) : base;
}

QString NVBDimension::toStr() const
{
  if (!dimstr.isNull()) return dimstr;

  if (base.isNull()) return QString();

  int neworder;
  QChar c = charFromOrder((int)round(log10(mult)),&neworder);
  dimstr = "%1%2%3";
  if (neworder!=0)
    dimstr = dimstr.arg(QString::number((int)exp10(neworder))+" ");
  else
    dimstr = dimstr.arg(QString());
  if (c.isNull())
    dimstr = dimstr.arg(QString());
  else
    dimstr = dimstr.arg(c);
  return dimstr.arg(base);
}

NVBPhysValue::NVBPhysValue(const QString & s, bool scalableDimension)
{
	QString v = s.trimmed();
  int i;
	if ( (i = v.indexOf(QRegExp("[^.,0-9-eE+]"))) > 0) {
    bool ok;
    value = v.left(i).toDouble(&ok);
		dim = NVBDimension(v.mid(i),scalableDimension);
    if (ok)
      value *= dim.purify();
    else
			NVBOutputError(QString("Converting string \"%1\" to double failed").arg(v.left(i)));
    }
  else
		NVBOutputError(QString("Converting string \"%1\" to number and dimension failed").arg(s));
}

NVBPhysValue::NVBPhysValue(const QString & s, NVBDimension d):dim(d)
{
  bool ok;
  value = s.toDouble(&ok);
  if (ok)
    value *= dim.purify(); // If not OK, value will be 0;
  else
		NVBOutputError(QString("Converting string \"%1\" to double failed").arg(s));
}

NVBPhysValue::NVBPhysValue(double f, NVBDimension d):dim(d),value(f)
{
  value *= dim.purify();
}

QString NVBPhysValue::toSIString() const
{
  return toFullString();
}

QString NVBPhysValue::toString( ) const
{
	if (valstr.isNull()) valstr = toString(0,2,3);
  return valstr;
}

QString NVBPhysValue::toString(int min1Pos, int max1Pos, int nSDig) const
{
  if (value == 0)
    return QString("0 %1").arg(dim.unitFromOrder(0));

  if (!dim.isScalable())
    return QString::number(value,'g',nSDig);

  if (max1Pos - min1Pos < 2) max1Pos = min1Pos+2;

  int h = (int) floor(log10(fabs(value)));
  int m = getPosMult(value,min1Pos,max1Pos);

  int p; // QString precision

  if (nSDig == 0) { // autoadjust
    if ( min1Pos < 0 )
      p = abs(h + m) + 2;
    else
      p = 0;
    }
  else {
    p =nSDig - h - m - 1;
    if (p < 0) p = 0;
    }

  return QString("%1 %2").arg(value*exp10(m),0,'f',p).arg(dim.unitFromOrder(-m));
}

QString NVBPhysValue::toStringWithOrder(int order) const
{
  if (!dim.isScalable())
    return QString::number(value);

  while (order%3) order++;
  return QString("%1 %2").arg(value*exp10(-order),0,'f').arg(dim.unitFromOrder(order));
}

int NVBPhysValue::getPosMult(double nvalue, int minSignPos, int maxSignPos)
{
  if (nvalue == 0) return 0;
  if (maxSignPos - minSignPos < 2) maxSignPos = minSignPos+2;

  int h = (int) floor(log10(fabs(nvalue)));
  int m; // multiplier
//  if (h >= 0) h+=1;
  if (h > maxSignPos) {
    m = maxSignPos - h;
    while (m%3) m--;
    }
  else if (h < minSignPos) {
    m = minSignPos - h;
    while (m%3) m++;
    }
  else {
    m = 0;
    }

  return m;
}

bool NVBPhysValue::operator <(const NVBPhysValue &v) const
{
	if (dim.isComparableWith(v.getDimension())) {
	 return getValue() < v.getValue(dim);
}
	else
		return dim.toStr() < v.getDimension().toStr();
}

double NVBPhysValue::getValue( NVBDimension _dim ) const
{
	if (dim.isComparableWith(_dim))
		return value / _dim.purify();
	else
		return 0;
}
