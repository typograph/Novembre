#ifndef NVBAXISMAPS_H
#define NVBAXISMAPS_H

#include "NVBMap.h"
#include "NVBDimension.h"
#include <QtCore/QList>
#include <QtGui/QTransform>


template <class T>
class NVBAxisTMap : public NVBAxisMap {
  protected:
    QList<T> vs;
  public:
    NVBAxisTMap():NVBAxisMap() {;}
    NVBAxisTMap(QList<T> values):NVBAxisMap(),vs(values) {;}
    ~NVBAxisTMap() {;}

    inline NVBAxisMap::Type mappingType() const { return NVBAxisMap::Template; }
    virtual inline int dimension() const { return 1; }
    virtual inline int valType() const { return qMetaTypeId<T>(); }

    T value(int i) const {
      if (i>=0 && i<vs.size())
				return vs.at(i);
      return T();
      }

		QVariant value(QList<int> indexes) {
			return QVariant::fromValue<T>(value(indexes.first()));
			}
};

class NVBAxisPhysMap : public NVBAxisTMap<NVBPhysValue> {
  private:
		double o, d;
		NVBDimension dim;
    NVBAxisMap::MapType t;
  public:
    NVBAxisPhysMap(QList<NVBPhysValue> values):NVBAxisTMap<NVBPhysValue>(values),t(NVBAxisMap::General) {;}
		NVBAxisPhysMap(NVBPhysValue origin, NVBPhysValue interval)
			: NVBAxisTMap<NVBPhysValue>()
			, o(origin.getValue())
			, d(interval.getValue())
			, dim(origin.getDimension())
			,	t(NVBAxisMap::Linear)
			{;}
		NVBAxisPhysMap(double origin, double interval, NVBDimension dimension)
			: NVBAxisTMap<NVBPhysValue>()
			, o(origin)
			, d(interval)
			, dim(dimension)
			,	t(NVBAxisMap::Linear)
			{;}

    inline NVBAxisMap::MapType mapType() const { return t; }
    inline NVBAxisMap::Type mappingType() const { return NVBAxisMap::Physical; }

    NVBPhysValue value(int i) const {
      if (t == NVBAxisMap::Linear)
				return NVBPhysValue(o + d*i,dim);
      return NVBAxisTMap<NVBPhysValue>::value(i);
      }

		inline NVBPhysValue origin() const { return NVBPhysValue(o,dim); }
		inline NVBPhysValue interval() const { return NVBPhysValue(d,dim); }
//    NVBPhysValue end() const;
};

class NVBAxisPointMap : public NVBAxisTMap<NVBPhysPoint> {
  public:
    NVBAxisPointMap(QList<QPointF> points, NVBDimension dimension):NVBAxisTMap<NVBPhysPoint>() {
      foreach(QPointF p, points)
			vs << NVBPhysPoint(p,dimension);
      }
    NVBAxisPointMap(QList<NVBPhysPoint> points):NVBAxisTMap<NVBPhysPoint>(points) {;}
    ~NVBAxisPointMap();

    inline NVBAxisMap::Type mappingType() const { return NVBAxisMap::Point; }
};

class NVBAxes2DGridMap : public NVBAxisMap {
  private:
    NVBPhysPoint o;
    QTransform t;
  public:
    NVBAxes2DGridMap(NVBPhysPoint origin, QTransform transform):NVBAxisMap(),o(origin),t(transform) {;}

    inline NVBAxisMap::MapType mapType() const { return NVBAxisMap::Linear2D; }
    inline NVBAxisMap::Type mappingType() const { return NVBAxisMap::Grid; }
    virtual inline int dimension() const { return 2; }

    inline NVBPhysPoint value(int i, int j) const {
      return o + t.map(QPointF(i,j));
      }

		QVariant value(QList<int> indexes) {
			return QVariant::fromValue<NVBPhysPoint>(value(indexes.first(),indexes.at(1)));
			}

    inline NVBPhysPoint origin() const { return o; }
    inline QTransform transformation() const { return t; }
};

#endif
