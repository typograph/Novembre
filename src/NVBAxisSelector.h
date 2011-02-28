/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef NVBAXISSELECTOR_H
#define NVBAXISSELECTOR_H

#include <QtCore/QList>
#include <QtGui/QColor>
#include "NVBUnits.h"
#include "NVBAxis.h"
#include "NVBDataGlobals.h"

class NVBDataSet;
class NVBDataSource;
struct NVBSelectorAxisPrivate;
class NVBSelectorInstance;

struct NVBSelectorAxis {
	NVBSelectorAxisPrivate * p;

	enum NVBAxisPropertyType {
		Invalid = 0,
		Index,
		MinLength,
		Units,
		MapDimensions,
		TypeID
	};

	NVBSelectorAxis();
	NVBSelectorAxis(const NVBSelectorAxis & other);
	~NVBSelectorAxis();
	
	NVBSelectorAxis & byIndex(int index);
	NVBSelectorAxis & byMinLength(int length);

	NVBSelectorAxis & byUnits(NVBUnits dimension);
	NVBSelectorAxis & byMapDimensions(int dimension);
	NVBSelectorAxis & byTypeId(int typeId);
	
	template <class T>
	inline NVBSelectorAxis & byType() { return byTypeId(qMetaTypeId<T>()); }

	NVBSelectorAxis & need(int more_axes, NVBAxisPropertyType t = Invalid);

	bool matches(const NVBAxis & axis, const NVBAxis & buddy = NVBAxis() ) const;
	bool needMore(int matched) const;
	
};

struct NVBSelectorCase {
	enum Type { Undefined, AND, OR };
	int id;
	Type t;
	QList<NVBSelectorCase> cases;
	QList<NVBSelectorAxis> axes;

	NVBSelectorCase(int caseId = 0, Type caseType = Undefined)
	: id(caseId)
	, t(caseType)
	{;}

	NVBSelectorCase(const NVBSelectorCase & other) ;

	NVBSelectorCase & addCase(int caseId = 0, Type caseType = Undefined);

	NVBSelectorAxis & addAxis();
	inline NVBSelectorAxis & addAxisByIndex(int index) { return addAxis().byIndex(index); }
	inline NVBSelectorAxis & addAxisByMinLength(int length) { return addAxis().byMinLength(length); }

	inline NVBSelectorAxis & addAxisByUnits(NVBUnits dimension) { return addAxis().byUnits(dimension); }
	inline NVBSelectorAxis & addAxisByMapDimensions(int dimension) { return addAxis().byMapDimensions(dimension); }
	inline NVBSelectorAxis & addAxisByTypeId(int typeId) { return addAxis().byTypeId(typeId); }

template <class T>
	inline NVBSelectorAxis & addAxisByType() { return addAxisByTypeId(qMetaTypeId<T>()); }

	bool matches(const NVBDataSet * dataSet);

	NVBSelectorInstance instantiate(const NVBDataSet * dataSet);
//	NVBSelectorInstance instantiate(const NVBDataSource * dataSource);

	void optimize();
//	NVBSelectorAxis & addDependentAxis();
};

typedef NVBSelectorCase NVBAxisSelector;
typedef NVBSelectorCase NVBAxesProps;

class NVBSelectorInstance {
	private:
		bool valid;
		const NVBSelectorCase * s;
		const NVBDataSource * dataSource;
		const NVBDataSet * dataSet;
		QVector<axisindex_t> matchedaxes;
		QVector<axisindex_t> otheraxes;

	private:
		bool matchAxes(axisindex_t start);
		
		/// Constructs a new selector instance on the given \a dataset, using predetermined axis sets
		NVBSelectorInstance(const NVBSelectorCase * selector, const NVBDataSet * dataset, QVector<axisindex_t> mas, QVector<axisindex_t> oas);
	public:
		NVBSelectorInstance(const NVBSelectorCase * selector = 0);
		/// Constructs a new selector instance on the given \a dataset, using \a selector rules
		NVBSelectorInstance(const NVBSelectorCase * selector, const NVBDataSet * dataset);
//		/// Constructs a new selector instance on the given \a datasource, using \a selector rules
//		NVBSelectorInstance(const NVBSelectorCase * selector, const NVBDataSource * datasource);

		/// Sub-instantiates this instance on \a dataset.
		NVBSelectorInstance matchDataset(const NVBDataSet * dataset);
		/// Sub-instantiates this instance on dataset number \a i of the datasource.
		NVBSelectorInstance matchDataset(axisindex_t i);

		inline bool isValid() const { return valid; }
		inline void reset() { valid = false; }

		inline const NVBDataSet * matchedDataset() const { return dataSet; }
		inline int matchedCase() const { return s ? s->id : -1; }

		const NVBAxis & matchedAxis(axisindex_t i) ;
		const NVBAxis & otherAxis(axisindex_t i) ;

		inline QVector<axisindex_t> matchedAxes() { return matchedaxes; }
		inline QVector<axisindex_t> otherAxes() { return otheraxes; }
};

#define forEachSliceAcross(instance) \
	forEachSlice(instance.matchedDataset(), instance.matchedAxes(), instance.otherAxes())

#define forEachSliceAlong(instance) \
	forEachSlice(instance.matchedDataset(), instance.otherAxes(), instance.matchedAxes())

#define forSingleSliceAcross(instance) \
	forSingleSlice(instance.matchedDataset(), instance.matchedAxes(), instance.otherAxes())

#define forSingleSliceAlong(instance) \
	forSingleSlice(instance.matchedDataset(), instance.otherAxes(), instance.matchedAxes())

#endif // NVBAXISSELECTOR_H
