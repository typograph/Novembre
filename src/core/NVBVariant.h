//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBVARIANT_H
#define NVBVARIANT_H

#include <QtCore/QVariant>
#include "NVBDimension.h"

class NVBVariant;
Q_DECLARE_METATYPE(NVBVariant);

typedef QList<NVBVariant> NVBVariantList;
Q_DECLARE_METATYPE(NVBVariantList);

class NVBVariant: public QVariant {
	private:
	public:
		NVBVariant(): QVariant() {;}
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
		NVBVariant(const QVariant & val): QVariant(val) { ; }
		template <typename T>
		NVBVariant(const T & val): QVariant() { setValue(val); }
		/*template <>
		  NVBVariant ( const NVBDimension & val);
		template <>
		  NVBVariant ( const NVBPhysValue & val);*/
		virtual ~NVBVariant() {;}

//   QVariant toVariant () const ;
		QString toString(const QString & separator = QString("")) const ;
		NVBDimension toDimension() const;
		NVBPhysValue toPhysValue() const;
		NVBVariantList toList() const;

		bool isAList() const;

		bool operator!= (const NVBVariant & v) const ;
		NVBVariant & operator= (const NVBVariant & variant) ;
		bool operator== (const NVBVariant & v) const;
		bool operator>= (const NVBVariant & v) const;
		bool operator> (const NVBVariant & v) const;
		bool operator<= (const NVBVariant & v) const;
		bool operator< (const NVBVariant & v) const;

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
bool variantListLessThan(const QList<T> & ll, const QList<T> & rl) {
	if (ll.isEmpty()) return true;

	if (rl.isEmpty()) return false;

	int i;

	for (i = 0; i < ll.size(); i++) {
		if (i == rl.size()) return false;

		if (ll.at(i) < rl.at(i)) return true;
		else if (rl.at(i) < ll.at(i)) return false;
		}

	return i != rl.size();
	}

/// \e private Do not use in your implementations. Behaviour may change between minor versions
template <typename T>
bool variantListEqual(const QList<T> & ll, const QList<T> & rl) {
	if (ll.size() != rl.size()) return false;

	if (ll.isEmpty()) return true;

	int i;

	for (i = 0; i < ll.size(); i++) {
		if (ll.at(i) != rl.at(i)) return false;
		}

	return true;
	}
#endif
