#ifndef NVBAXIS_H
#define NVBAXIS_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include "NVBDataGlobals.h"
class NVBDataSource;
class NVBAxisMap;
class NVBAxisPhysMap;

struct NVBAxisMapping {
	NVBAxisMap * map;
	QVector<axisindex_t> axes;
	NVBAxisMapping() : map(0) {;}
	NVBAxisMapping(NVBAxisMap * m, QVector<axisindex_t> a) : map(m), axes(a) {;}
};

class NVBAxis {
	friend class NVBConstructableDataSource;
  protected:
    /// Parent data source (to be able to implement functions)
    NVBDataSource * p;
    /// Axis name
    QString n;
    /// Axis length
    axissize_t l;
    /// Relevant mappings
    QVector< NVBAxisMapping> ms;
		/// NVBPhysValue map - for easier access to units
		NVBAxisPhysMap * pm;

	  void addMapping( NVBAxisMapping mapping ) ;

	public:
    NVBAxis(NVBDataSource * parent, QString name, axissize_t length) : p(parent), n(name), l(length), pm(0) {;}
    NVBAxis(const NVBAxis & other) : p(other.p), n(other.n), l(other.l), ms(other.ms), pm(other.pm) {;}
    // NVBAxis doesn't own anything, neither the parent, nor the maps.
    ~NVBAxis() {;}

    inline QString name() const { return n; }
    inline axissize_t length() const { return l; }
    inline QVector<NVBAxisMapping> maps() const { return ms; }

    inline NVBAxisPhysMap * physMap() const { return pm; }

    inline NVBDataSource * dataSource() const { return p; }

		bool operator==(const NVBAxis & other) const {
			return p == other.p && n == other.n;
			}

		inline bool operator!=(const NVBAxis & other) const {
			return !operator==(other);
			}
};

// Q_DECLARE_METATYPE(NVBAxis)

#endif