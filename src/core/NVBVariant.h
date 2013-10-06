//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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
#include <QtCore/QStringList>
#include "NVBUnits.h"

class NVBVariant;
Q_DECLARE_METATYPE(NVBVariant);

class NVBVariantList;
Q_DECLARE_METATYPE(NVBVariantList);

/**
 * \class NVBVariantList
 * 
 * NVBVariantList is a list of NVBVariant values with an added ability to
 * convert the list to a QString. The individual values are converted to QString
 * and the resulting strings are concatenated, separated by separator().
 */
/// A list of NVBVariant
class NVBVariantList : public QList<NVBVariant> {

	private:
		QString _separator;
	public:
		/// Returns the separator for string conversion
		QString separator() const { return _separator; }
		/// Sets the separator for string conversion
		void setSeparator(const QString & separator) { _separator = separator; }

		/// Converts the list to string with elements separated with separator()
		QString toString(QString separator = QString()) const;
		/// Converts every list element to a string and returns a list of there strings
		QStringList toStringList() const;
	};

/**
 * \class NVBVariant
 * 
 * NVBVariant is an extension of QVariant that natively supports
 * Novembre types such as NVBUnits, NVBPhysValue and NVBVariantList.
 * The support includes correct ordering and string conversion.
 * 
 * NVBVariant is the type of comment values returned by file plugins
 * (see NVBFileInfo::getComment()).
 */
/// Novembre version of QVariant, supporting core classes
class NVBVariant: public QVariant {
	private:
	public:
		/// Constructs an invalid NVBVariant
		NVBVariant (): QVariant() {;}
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
		/// Constructs an NVBVariant from QVariant
		NVBVariant ( const QVariant & val): QVariant(val) { ; }
		/// Constructs an NVBVariant containing a value of arbitrary type
		template <typename T>
		NVBVariant ( const T & val): QVariant() { setValue(val); }
		/*template <>
		  NVBVariant ( const NVBUnits & val);
		template <>
		  NVBVariant ( const NVBPhysValue & val);*/
		/// Deconstructs NVBVariant
		virtual ~NVBVariant () {;}

//   QVariant toVariant () const ;
		/// Converts NVBVariant to a string representation
		QString toString () const ;
		/// Returns an NVBUnits contained in this NVBVariant
		NVBUnits toDimension () const;
		/// Returns an NVBPhysValue contained in this NVBVariant
		NVBPhysValue toPhysValue () const;
		/// Returns a list contained in this NVBVariant
		NVBVariantList toList() const;

		/// Returns true if this NVBVariant contains an NVBVariantList or a QVariantList
		bool isAList() const;

		/// Copies the value from another NVBVariant to this one
		NVBVariant & operator= ( const NVBVariant & variant ) ;

		/// Compares two NVBVariants for inequality
		bool operator!= ( const NVBVariant & v ) const ;
		/// Compares two NVBVariants for equality
		bool operator== ( const NVBVariant & v ) const;
		bool operator>= ( const NVBVariant & v ) const;
		bool operator>  ( const NVBVariant & v ) const;
		bool operator<= ( const NVBVariant & v ) const;
		bool operator<  ( const NVBVariant & v ) const;

		bool operator>= ( const NVBPhysValue & v ) const;
		bool operator>  ( const NVBPhysValue & v ) const;
		bool operator<= ( const NVBPhysValue & v ) const;
		bool operator<  ( const NVBPhysValue & v ) const;

		/// Converts NVBVariant to QVariant
		operator QVariant() const { return QVariant::fromValue(*this); }
		/// Returns a list contained in this NVBVariant
		operator NVBVariantList() const { return toList(); }
		/// Returns an NVBPhysValue contained in this NVBVariant
		operator NVBPhysValue() const { return toPhysValue(); }
		/// Returns an NVBUnits contained in this NVBVariant
		operator NVBUnits() const { return toDimension(); }
		/// Converts NVBVariant to a string representation
		operator QString() const { return toString(); }
	};

/// Compares two QVariants
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
