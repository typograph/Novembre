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

#include "NVBVariant.h"
#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDateTime>

QString NVBVariant::toString(const QString & separator) const {
	switch (userType()) {
#if QT_VERSION >= 0x040400

		case QVariant::Date :
			{ return toDate().toString(Qt::SystemLocaleShortDate); }

		case QVariant::Time :
			{ return toTime().toString(Qt::SystemLocaleShortDate); }

		case QVariant::DateTime :
			{ return toDateTime().toString(Qt::SystemLocaleShortDate); }

#else

		case QVariant::Date :
			{ return toDate().toString(Qt::SystemLocaleDate); }

		case QVariant::Time :
			{ return toTime().toString(Qt::SystemLocaleDate); }

		case QVariant::DateTime :
			{ return toDateTime().toString(Qt::SystemLocaleDate); }

#endif

		case QVariant::Bool :
			{ return toBool() ? QString("+") : QString(); }

		case QVariant::List : { // should never ever happen
			NVBOutputError("A QVariantList seems to be contained inside this NVBVariant object. Please use NVBVariantList instead.");
//       return QString();
			QStringList l;
			foreach(NVBVariant subv, toList()) {
				if (!subv.toString().isEmpty())
					l << subv;
				}
			return l.join(separator);
			}

		default : {
			const int ut = userType();

			if (ut < QMetaType::User)
				return QVariant::toString();

			if (ut == qMetaTypeId<NVBPhysValue>())
				return value<NVBPhysValue>().toString();

			if (ut == qMetaTypeId<NVBDimension>())
				return value<NVBDimension>().toStr();

			if (ut == qMetaTypeId<NVBVariantList>()) {
				QStringList l;
				foreach(NVBVariant subv, value<NVBVariantList>()) {
					if (!subv.toString().isEmpty())
						l << subv;
					}
				return l.join(separator);
				}
			else
				return QString();
			}
		}
	}

NVBDimension NVBVariant::toDimension() const {
	if (userType() == qMetaTypeId<NVBDimension>())
		return value<NVBDimension>();
	else
		return NVBDimension();
	}

NVBPhysValue NVBVariant::toPhysValue() const {
	if (userType() == qMetaTypeId<NVBPhysValue>())
		return value<NVBPhysValue>();
	else
		return NVBPhysValue();
	}

NVBVariantList NVBVariant::toList() const {
	if (userType() == QVariant::List) {
		NVBVariantList l;
		foreach(QVariant v, QVariant::toList())
		l << v;
		return l;
		}

	if (userType() == qMetaTypeId<NVBVariantList>())
		return value<NVBVariantList>();

	return NVBVariantList();
	}

bool NVBVariant::operator!=(const NVBVariant & v) const {
	return !operator ==(v);
	}

bool NVBVariant::operator==(const NVBVariant & v) const {
	int ut = userType();

	if (ut != v.userType())
		return false;

	if (ut < QMetaType::User)
		return QVariant::operator ==(v);

	if (ut == qMetaTypeId<NVBPhysValue>())
		return value<NVBPhysValue>() == v.toPhysValue();

	if (ut == qMetaTypeId<NVBDimension>())
		return value<NVBDimension>() == v.toDimension();

	if (ut == qMetaTypeId<NVBVariantList>())
		return variantListEqual<NVBVariant>(value<NVBVariantList>(), v.value<NVBVariantList>());

	return false;
	}

NVBVariant & NVBVariant::operator=(const NVBVariant & variant) {
	QVariant::operator =(variant);
	return *this;
	}

bool NVBVariant::operator<(const NVBVariant & v) const {
	int ut = userType();

	if (ut != v.userType())
		return false;

	if (ut < QMetaType::User)
		return ((QVariant) * this) < v;

	if (ut == qMetaTypeId<NVBPhysValue>())
		return toPhysValue() < v.toPhysValue();

	if (ut == qMetaTypeId<NVBDimension>())
		return toDimension().toStr() < v.toDimension().toStr();

	if (ut == qMetaTypeId<NVBVariantList>())
		return variantListLessThan<NVBVariant>(value<NVBVariantList>(), v.value<NVBVariantList>());

	return false;
	}

bool NVBVariant::operator<=(const NVBVariant & v) const {
	if (userType() != v.userType())
		return false;

	return operator ==(v) || operator <(v);
	}

bool NVBVariant::operator>(const NVBVariant & v) const {
	if (userType() != v.userType())
		return false;

	return !operator <=(v);
	}

bool NVBVariant::operator>=(const NVBVariant & v) const {
	if (userType() != v.userType())
		return false;

	return !operator <(v);
	}

bool operator<(const QVariant & l, const QVariant & r) {
	switch (l.type()) {
		case QVariant::Invalid:
			return (r.type() == QVariant::Invalid);

		case QVariant::Int:
			return l.toInt() < r.toInt();

		case QVariant::UInt:
			return l.toUInt() < r.toUInt();

		case QVariant::LongLong:
			return l.toLongLong() < r.toLongLong();

		case QVariant::ULongLong:
			return l.toULongLong() < r.toULongLong();

		case QVariant::Double:
			return l.toDouble() < r.toDouble();

		case QVariant::Char:
			return l.toChar() < r.toChar();

		case QVariant::Date:
			return l.toDate() < r.toDate();

		case QVariant::Time:
			return l.toTime() < r.toTime();

		case QVariant::DateTime:
			return l.toDateTime() < r.toDateTime();

		case QVariant::Bool :
			return !l.toBool();

		case QVariant::List : { // the two lists have same size
			return variantListLessThan<QVariant>(l.toList(), r.toList());
			}

		case QVariant::String:
		default:
			return l.toString().compare(r.toString()) < 0;
			/*        if (isSortLocaleAware())
			            return l.toString().localeAwareCompare(r.toString()) < 0;
			        else
			            return l.toString().compare(r.toString(), sortCaseSensitivity()) < 0;*/
		}

	return false;
	}

bool NVBVariant::isAList() const {
	return userType() == QVariant::List || userType() == qMetaTypeId<NVBVariantList>();
	}
