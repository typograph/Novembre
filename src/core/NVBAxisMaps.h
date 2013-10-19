//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBAXISMAPS_H
#define NVBAXISMAPS_H

#include "NVBMap.h"
#include "NVBUnits.h"
#include <QtCore/QList>
#include <QtGui/QTransform>


template <class T>
class NVBAxisTMap : public NVBAxisMap {
	protected:
		QList<T> vs;
	public:
		NVBAxisTMap(): NVBAxisMap() {;}
		NVBAxisTMap(QList<T> values): NVBAxisMap(), vs(values) {;}
		~NVBAxisTMap() {;}

		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Template; }
		virtual inline int dimension() const { return 1; }
		virtual inline int valType() const { return qMetaTypeId<T>(); }

		T value(axissize_t i) const {
			if (i < (axissize_t) vs.size())
				return vs.at(i);

			return vs.at( (axissize_t)vs.size() - 1);
			}

		virtual NVBAxisTMap<T> * copy() { return new NVBAxisTMap<T>(vs); }

		NVBVariant value(QVector<axissize_t> indexes) {
			return NVBVariant::fromValue<T>(value(indexes.first()));
			}
	};

class NVBAxisPhysMap : public NVBAxisTMap<NVBPhysValue> {
	private:
		double o, d;
		NVBUnits dim;
		NVBAxisMap::MapType t;
	public:
		NVBAxisPhysMap(QList<NVBPhysValue> values): NVBAxisTMap<NVBPhysValue>(values), t(NVBAxisMap::General) {;}
		NVBAxisPhysMap(QList<double> values, NVBUnits units): NVBAxisTMap<NVBPhysValue>(), t(NVBAxisMap::General) {
			QList<NVBPhysValue> pvalues;
			foreach(double v, values)
			pvalues << NVBPhysValue(v, units);
			vs = pvalues;
			}
		NVBAxisPhysMap(QVector<double> values, NVBUnits units): NVBAxisTMap<NVBPhysValue>(), t(NVBAxisMap::General) {
			QList<NVBPhysValue> pvalues;
			foreach(double v, values)
			pvalues << NVBPhysValue(v, units);
			vs = pvalues;
			}
		NVBAxisPhysMap(NVBPhysValue origin, NVBPhysValue interval)
			: NVBAxisTMap<NVBPhysValue>()
			, o(origin.getValue())
			, d(interval.getValue())
			, dim(origin.getDimension())
			,	t(NVBAxisMap::Linear)
			{;}
		NVBAxisPhysMap(double origin, double interval, NVBUnits dimension)
			: NVBAxisTMap<NVBPhysValue>()
			, o(origin)
			, d(interval)
			, dim(dimension)
			,	t(NVBAxisMap::Linear)
			{;}

		inline NVBAxisMap::MapType mapType() const { return t; }
		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Physical; }

		inline NVBUnits units() const { return dim; }

		NVBPhysValue value(axissize_t i) const {
			if (t == NVBAxisMap::Linear)
				return NVBPhysValue(o + d * i, dim);

			return NVBAxisTMap<NVBPhysValue>::value(i);
			}

		NVBPhysValue closestValue(NVBPhysValue v) {
			if (t == NVBAxisMap::Linear)
				return NVBPhysValue(o + d * round((v.getValue(dim) - o) / d), dim);

			QList<NVBPhysValue>::const_iterator pos = qLowerBound(vs, v);

			if (pos == vs.end())
				return vs.last();

			if (pos == vs.begin())
				return vs.first();

			return (abs((*pos - v).getValue()) < abs((*(pos - 1) - v).getValue()) ? *pos : * (pos - 1));
			}

		axissize_t closestValueIndex(NVBPhysValue v) {
			if (t == NVBAxisMap::Linear)
				return round((v.getValue(dim) - o) / d);

			QList<NVBPhysValue>::const_iterator pos = qLowerBound(vs, v);

			if (pos == vs.end())
				return vs.count() - 1;

			if (pos == vs.begin())
				return 0;

			return (abs((*pos - v).getValue()) < abs((*(pos - 1) - v).getValue()) ? (axissize_t)(pos - vs.begin()) : (axissize_t)(pos - 1 - vs.begin()));
			}

		inline NVBPhysValue origin() const { return NVBPhysValue(o, dim); }
		inline NVBPhysValue interval() const { return NVBPhysValue(d, dim); }
//		NVBPhysValue end() const;

		virtual NVBAxisPhysMap * copy() {
			switch (t) {
				case NVBAxisMap::General :
					return new NVBAxisPhysMap(vs);

				case NVBAxisMap::Linear :
					return new NVBAxisPhysMap(o, d, dim);

				default:
					NVBOutputError("Unrecognized map type");
					return 0;
				}
			}

	};

class NVBAxisPointMap : public NVBAxisTMap<NVBPhysPoint> {
	public:
		NVBAxisPointMap(QList<QPointF> points, NVBUnits dimension): NVBAxisTMap<NVBPhysPoint>() {
			foreach(QPointF p, points)
			vs << NVBPhysPoint(p, dimension);
			}
		NVBAxisPointMap(QList<NVBPhysPoint> points): NVBAxisTMap<NVBPhysPoint>(points) {;}

		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Point; }

		virtual NVBAxisPointMap * copy() { return new NVBAxisPointMap(vs); }
	};

class NVBAxes2DGridMap : public NVBAxisMap {
	private:
		NVBPhysPoint o;
		QTransform t;
	public:
		NVBAxes2DGridMap(NVBPhysPoint origin, QTransform transform): NVBAxisMap(), o(origin), t(transform) {;}
		NVBAxes2DGridMap(const NVBAxisPhysMap * x, const NVBAxisPhysMap * y): NVBAxisMap() {
			o = NVBPhysPoint(x->value(0), y->value(0));
			t = QTransform::fromScale((x->value(1) - o.x()).getValue(), (y->value(1) - o.y()).getValue());
			}

		inline NVBAxisMap::MapType mapType() const { return NVBAxisMap::Linear2D; }
		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Point; }
		virtual inline int dimension() const { return 2; }
		virtual inline int valType() const { return qMetaTypeId<NVBPhysPoint>(); }

		inline NVBPhysPoint value(axissize_t i, axissize_t j) const {
			return o + t.map(QPointF(i, j));
			}

		NVBVariant value(QVector<axissize_t> indexes) {
			return NVBVariant::fromValue<NVBPhysPoint>(value(indexes.first(), indexes.at(1)));
			}

		inline NVBPhysPoint origin() const { return o; }
		inline QTransform transformation() const { return t; }

		virtual NVBAxes2DGridMap * copy() { return new NVBAxes2DGridMap(o, t); }
	};

class NVBAxisColorMap : public NVBAxisTMap<QColor> {
	public:
		NVBAxisColorMap(QList<QColor> colors): NVBAxisTMap<QColor>(colors) {;}

		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Color; }

		virtual NVBAxisColorMap * copy() { return new NVBAxisColorMap(vs); }
	};

template <class T>
class NVBAxisFuncMap : public NVBAxisMap {
	typedef T (*VFunc)(axissize_t);
	protected:
		VFunc getvalue;
	public:
		NVBAxisFuncMap(VFunc function):NVBAxisMap(),getvalue(function) {;}
		~NVBAxisFuncMap() {;}

		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Template; }
		virtual inline int dimension() const { return 1; }
		virtual inline int valType() const { return qMetaTypeId<T>(); }

		T value(axissize_t i) const {
			return getvalue(i);
			}

		virtual NVBAxisFuncMap<T> * copy() { return new NVBAxisFuncMap<T>(getvalue); }

		NVBVariant value(QVector<axissize_t> indexes) {
			return NVBVariant::fromValue<T>(value(indexes.first()));
			}
};

template <class T>
class NVBAxesNDFuncMap : public NVBAxisMap {
	typedef T (*VFunc)(QVector<axissize_t>);
	protected:
		VFunc getvalue;
		axisindex_t n;
	public:
		NVBAxesNDFuncMap(VFunc function, axisindex_t naxes):NVBAxisMap(),getvalue(function),n(naxes) {;}
		~NVBAxesNDFuncMap() {;}

		inline NVBAxisMap::ValueType mappingType() const { return NVBAxisMap::Template; }
		virtual inline int dimension() const { return n; }
		virtual inline int valType() const { return qMetaTypeId<T>(); }

		virtual NVBAxesNDFuncMap<T> * copy() { return new NVBAxesNDFuncMap<T>(getvalue,n); }

		NVBVariant value(QVector<axissize_t> indexes) {
			return NVBVariant::fromValue<T>(getvalue(indexes));
			}
};

#endif
