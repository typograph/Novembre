//
// C++ Interface: NVBVariant
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBVARIANT_H
#define NVBVARIANT_H

#include <QVariant>
#include "NVBDimension.h"

class NVBVariant;
Q_DECLARE_METATYPE(NVBVariant);

typedef QList<NVBVariant> NVBVariantList;
Q_DECLARE_METATYPE(NVBVariantList);

class NVBVariant: public QVariant {
private:
public:
  NVBVariant ():QVariant() {;}
/*  NVBVariant ( Type type );
  NVBVariant ( int typeOrUserType, const void * copy ) ;
  NVBVariant ( const NVBVariant & p ) ;
  NVBVariant ( QDataStream & s ) ;
  NVBVariant ( int val ) ;
  NVBVariant ( uint val ) ;
  NVBVariant ( qlonglong val ) ;
  NVBVariant ( qulonglong val ) ;
  NVBVariant ( bool val ) ;
  NVBVariant ( double val ) ;
  NVBVariant ( const char * val ) ;
  NVBVariant ( const QByteArray & val ) ;
  NVBVariant ( const QBitArray & val ) ;
  NVBVariant ( const QString & val ) ;
  NVBVariant ( const QLatin1String & val ) ;
  NVBVariant ( const QStringList & val ) ;
  NVBVariant ( const QChar & c ) ;
  NVBVariant ( const QDate & val ) ;
  NVBVariant ( const QTime & val ) ;
  NVBVariant ( const QDateTime & val ) ;
  NVBVariant ( const QList<NVBVariant> & val ) ;
  NVBVariant ( const QMap<QString, NVBVariant> & val ) ;
  NVBVariant ( const QSize & val ) ;
  NVBVariant ( const QSizeF & val ) ;
  NVBVariant ( const QPoint & val ) ;
  NVBVariant ( const QPointF & val ) ;
  NVBVariant ( const QLine & val ) ;
  NVBVariant ( const QLineF & val ) ;
  NVBVariant ( const QRect & val ) ;
  NVBVariant ( const QRectF & val ) ;
  NVBVariant ( const QUrl & val ) ;
  NVBVariant ( const QLocale & l ) ;
  NVBVariant ( const QRegExp & regExp ) ;
  NVBVariant ( Qt::GlobalColor color ) ;*/
  NVBVariant ( const QVariant & val):QVariant(val) { ; }
template <typename T>
  NVBVariant ( const T & val):QVariant() { setValue(val); }
/*template <>
  NVBVariant ( const NVBDimension & val);
template <>
  NVBVariant ( const NVBPhysValue & val);*/
  virtual ~NVBVariant () {;}

//   QVariant toVariant () const ;
  QString toString (const QString & separator = QString("")) const ;
  NVBDimension toDimension () const;
  NVBPhysValue toPhysValue () const;
  NVBVariantList toList() const;

  bool isAList() const;

  bool operator!= ( const NVBVariant & v ) const ;
  NVBVariant & operator= ( const NVBVariant & variant ) ;
  bool operator== ( const NVBVariant & v ) const;
  bool operator>= ( const NVBVariant & v ) const;
  bool operator>  ( const NVBVariant & v ) const;
  bool operator<= ( const NVBVariant & v ) const;
  bool operator<  ( const NVBVariant & v ) const;

  operator QVariant() const { return QVariant::fromValue(*this); }
  operator NVBVariantList() const { return toList(); }
  operator NVBPhysValue() const { return toPhysValue(); }
  operator NVBDimension() const { return toDimension(); }
  operator QString() const { return toString(); }
};

bool operator<(const QVariant & l, const QVariant & r);
// bool variantLessThan(const NVBVariant & l, const NVBVariant & r) { return l < r; }

/// \e private Do not use in your implementations. Behaviour may change between minor versions
template <typename T>
bool variantListLessThan( const QList<T> & ll, const QList<T> & rl) {
      if (ll.isEmpty()) return true;
      if (rl.isEmpty()) return false;
      int i;
      for(i = 0; i < ll.size(); i++) {
        if (i == rl.size()) return false;
        if (ll.at(i) < rl.at(i)) return true;
        else if (rl.at(i) < ll.at(i)) return false;
        }
      return i != rl.size();
}

/// \e private Do not use in your implementations. Behaviour may change between minor versions
template <typename T>
bool variantListEqual( const QList<T> & ll, const QList<T> & rl) {
      if (ll.size() != rl.size()) return false;
      if (ll.isEmpty()) return true;
      int i;
      for(i = 0; i < ll.size(); i++) {
        if (ll.at(i) != rl.at(i)) return false;
        }
      return true;
}
#endif
