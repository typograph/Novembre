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
#include <QtCore/QMap>
#include "NVBDataSource.h"

class NVBDataSet;
class NVBDataSource;
struct NVBSelectorAxisPrivate;
class NVBSelectorDataInstance;
class NVBSelectorSourceInstance;
class NVBSelectorFileInstance;

struct NVBSelectorAxis {
	NVBSelectorAxisPrivate * p;

	enum NVBAxisPropertyType {
		Invalid = 0,
		Index,
		Name,
		MinLength,
		MaxLength,
		Length,
		Units,
		MapDimensions,
		TypeID
	};

	NVBSelectorAxis();
	NVBSelectorAxis(const NVBSelectorAxis & other);
	~NVBSelectorAxis();
	
	NVBSelectorAxis & byName(QString name);
	NVBSelectorAxis & byIndex(int index);
	NVBSelectorAxis & byMinLength(int length);
	NVBSelectorAxis & byMaxLength(int length);
	NVBSelectorAxis & byLength(int length);

	NVBSelectorAxis & byUnits(NVBUnits dimension);
	NVBSelectorAxis & byMapDimensions(int dimension);
	NVBSelectorAxis & byTypeId(int typeId);
	
	bool hasRuleFor(NVBAxisPropertyType type);
	
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
	NVBDataSet::Type target;
	QList<NVBSelectorCase> cases;
	QList<NVBSelectorAxis> axes;

	bool optimal; // to save on optimize() calls
	
	NVBSelectorCase(int caseId = 0, Type caseType = Undefined)
	: id(caseId)
	, t(caseType)
	, optimal(true)
	{;}

	NVBSelectorCase(const NVBSelectorCase & other) ;

	NVBSelectorCase & setType(NVBDataSet::Type type);
	
	NVBSelectorCase & addCase(int caseId = 0, Type caseType = Undefined);
	inline NVBSelectorCase & lastCase() { return (t == OR) ? cases.last() : *this; }

	NVBSelectorAxis & addAxis();
	inline NVBSelectorAxis & addAxisByName(QString name) { return addAxis().byName(name); }
	inline NVBSelectorAxis & addAxisByIndex(int index) { return addAxis().byIndex(index); }
	inline NVBSelectorAxis & addAxisByMinLength(int length) { return addAxis().byMinLength(length); }
	inline NVBSelectorAxis & addAxisByMaxLength(int length) { return addAxis().byMaxLength(length); }
	inline NVBSelectorAxis & addAxisByLength(int length) { return addAxis().byLength(length); }

	inline NVBSelectorAxis & addAxisByUnits(NVBUnits dimension) { return addAxis().byUnits(dimension); }
	inline NVBSelectorAxis & addAxisByMapDimensions(int dimension) { return addAxis().byMapDimensions(dimension); }
	inline NVBSelectorAxis & addAxisByTypeId(int typeId) { return addAxis().byTypeId(typeId); }

template <class T>
	inline NVBSelectorAxis & addAxisByType() { return addAxisByTypeId(qMetaTypeId<T>()); }

	bool matches(const NVBDataSet * dataSet);

	/// Returns an instance, that reports all datasources that have matching datasets
	NVBSelectorFileInstance instantiate(const QList<NVBDataSource *> * dataSources);
	/// Returns an instance, that reports one datasource from the list that has matching datasets
	NVBSelectorSourceInstance instantiateOneSource(const QList<NVBDataSource *> * dataSources);
	/// Returns an instance, that reports the first matching dataset from the list
	NVBSelectorDataInstance instantiateOneDataset(const QList<NVBDataSource *> * dataSources);
	
	/// Returns an instance, that reports all matching datasets (may be none)
	NVBSelectorSourceInstance instantiate(const NVBDataSource * dataSource);
	/// Returns an instance, that reports the first matching dataset from the list
	NVBSelectorDataInstance instantiateOneDataset(const NVBDataSource * dataSource);

	/// Returns an instance, that has info on all matching axes from the selector (may be invalid if no match)
	NVBSelectorDataInstance instantiate(const NVBDataSet * dataSet);

	void optimize();
//	NVBSelectorAxis & addDependentAxis();
};

typedef NVBSelectorCase NVBAxisSelector;
// typedef NVBSelectorCase NVBAxesProps;

struct NVBSelectorAxisInstance {
	int additionalAxes;
	QList<axisindex_t> axes;
// 	QList<NVBAxisMapping> maps;
	
	NVBSelectorAxisInstance(int addA = 0) : additionalAxes(addA) {;} // the list is empty anyway
	NVBSelectorAxisInstance(int addA, QList<axisindex_t> list) : additionalAxes(addA) , axes(list) {;}
};

typedef QList< NVBSelectorAxisInstance > NVBSelectorAxisInstanceList;

class NVBSelectorDataInstance {
	friend class NVBSelectorSourceInstance;
	private:
		bool valid;
		const NVBDataSet * dataSet;
	NVBSelectorCase s;
		QVector<axisindex_t> matchedaxes;
		mutable QVector<axisindex_t> otheraxes;

	NVBSelectorDataInstance(const NVBSelectorCase& selector, const NVBDataSet* dataset, NVBSelectorAxisInstanceList matched);
		
	void initAxes(NVBSelectorAxisInstanceList matched);
	
	public:
	NVBSelectorDataInstance() : valid(false), dataSet(0) {;}
	NVBSelectorDataInstance( const NVBSelectorCase& selector, const NVBDataSet* dataset = 0);

	/// Returns the dataset this instance was constructed for
	inline const NVBDataSet * matchingData() const { return dataSet; }

	int matchedCase() const;
	/// The instance is valid when all rules in one case matched
		inline bool isValid() const { return valid; }
	/// Make the instance invalid
		inline void reset() { valid = false; }

//	/// Returns the datasource this instance was constructed for
//	NVBDataSource * matchingSource();

	/// Returns the axes that matched the rules. The axes are returned in rule order
	inline const QVector<axisindex_t> & matchedAxes() const { return matchedaxes; }
	/// Returns the axes that complement the matched ones. The axes are returned in dataset order
	inline const QVector<axisindex_t> & otherAxes() const { return otheraxes; }
	/// Returns the maps that were required by the rules in rule order. Axis indexes are in dataset axes.
// 	inline const QList<NVBAxisMapping> & matchedMaps() const { return matchedmaps; }

	NVBAxis matchedAxis(axisindex_t i) const ;
	NVBAxis otherAxis(axisindex_t i) const ;
	
};

class NVBSelectorSourceInstance {
private:
	NVBSelectorCase s;

	QMap< int, QList<NVBSelectorDataInstance> > instances;
	QList<NVBSelectorDataInstance> allInstances;
	QMap< int, QList<int> > indexes;
	QList<int> allIndexes;
	QMap< int, QList<NVBDataSet*> > sets;
	QList<NVBDataSet*> allSets;
	
	const NVBDataSource * source;
	
public:
	NVBSelectorSourceInstance(const NVBSelectorCase & selector, const NVBDataSource * source);
	void fillInstances(const NVBSelectorCase & selector, const NVBDataSource* source);

	const NVBDataSource * matchingSource() const { return source;}
//	QVector<axisindex_t> matchedAxes();

	inline bool isValid() const { return !instances.isEmpty(); }
	inline void reset() { instances.clear(); }	
	
	const QList<NVBDataSet *> & matchedDatasets(int selectorCase) const
		{
			static QList<NVBDataSet*> empty;
			if (sets.contains(selectorCase))
				return sets.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<NVBDataSet *> & matchedDatasets() const { return allSets; }

	const QList<NVBSelectorDataInstance> & matchedInstances(int selectorCase) const
		{
			static QList<NVBSelectorDataInstance> empty;
			if (instances.contains(selectorCase))
				return instances.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<NVBSelectorDataInstance> & matchedInstances() const { return allInstances; }

	const QList<int> & matchedIndexes(int selectorCase) const
		{
			static QList<int> empty;
			if (indexes.contains(selectorCase))
				return indexes.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<int> & matchedIndexes() const { return allIndexes; }
};

class NVBSelectorFileInstance {
private:
	NVBSelectorCase s;
	
	QMap< int, QList<NVBSelectorSourceInstance> > instances;
	QList<NVBSelectorSourceInstance> allInstances;
	QMap< int, QList<int> > indexes;
	QList<int> allIndexes;
	QMap< int, QList<NVBDataSource*> > sources;
	QList<NVBDataSource*> allSources;
	
	void fillInstances(const NVBSelectorCase & selector, const QList<NVBDataSource *> & sources);
	void fillLists();

	const NVBSelectorSourceInstance & instFromDatasource(const QList<NVBSelectorSourceInstance> & list, const NVBDataSource * source) const;
	
public:
	NVBSelectorFileInstance(const NVBSelectorCase & selector, const QList<NVBDataSource *> & sources =  QList<NVBDataSource *>());

	const QList<NVBDataSource *> & matchedDatasources(int selectorCase) const
		{ 
			static QList<NVBDataSource*> empty;
			if (sources.contains(selectorCase))
				return sources.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<NVBDataSource *> & matchedDatasources() const { return allSources; }

	const QList<NVBSelectorSourceInstance> & matchedInstances(int selectorCase) const
		{
			static QList<NVBSelectorSourceInstance> empty;
			if (instances.contains(selectorCase))
				return instances.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<NVBSelectorSourceInstance> & matchedInstances() const { return allInstances; }
	
	inline const NVBSelectorSourceInstance & matchedInstance(const NVBDataSource * source) const
		{ return NVBSelectorFileInstance::instFromDatasource(allInstances,source); }
	inline const NVBSelectorSourceInstance & matchedInstance(const NVBDataSource * source, int selectorCase) const
		{ return NVBSelectorFileInstance::instFromDatasource(matchedInstances(selectorCase),source); }

	const QList<int> & matchedIndexes(int selectorCase) const
		{
			static QList<int> empty;
			if (indexes.contains(selectorCase))
				return indexes.find(selectorCase).value();
			else
				return empty;
		}
	inline const QList<int> & matchedIndexes() const { return allIndexes; }
};

#define forEachSliceAcross(instance) \
	forEachSlice(instance.matchingData(), instance.matchedAxes(), instance.otherAxes())

#define forEachSliceAlong(instance) \
	forEachSlice(instance.matchingData(), instance.otherAxes(), instance.matchedAxes())

#define forNSlicesAcross(instance,N) \
	forNSlices(instance.matchingData(), N, instance.matchedAxes(), instance.otherAxes())

#define forNSlicesAlong(instance,N) \
	forNSlices(instance.matchingData(), N, instance.otherAxes(), instance.matchedAxes())

#define forSingleSliceAcross(instance) \
	forSingleSlice(instance.matchingData(), instance.matchedAxes(), instance.otherAxes())

#define forSingleSliceAlong(instance) \
	forSingleSlice(instance.matchingData(), instance.otherAxes(), instance.matchedAxes())

#endif // NVBAXISSELECTOR_H
