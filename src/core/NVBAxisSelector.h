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

#ifndef NVBAXISSELECTOR_H
#define NVBAXISSELECTOR_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QColor>
#include "NVBUnits.h"
#include "NVBAxis.h"
#include "NVBDataGlobals.h"
#include "NVBDataSource.h"
#include "NVBAxisSelectorHelpers.h"

class NVBDataSet;
class NVBDataSource;

struct NVBSelectorAxisInstance;
class NVBSelectorDataInstance;
class NVBSelectorSourceInstance;
class NVBSelectorFileInstance;

class NVBSelectorCase {
		friend class NVBSelectorSourceInstance;
		friend class NVBSelectorFileInstance;
	public:
		int id;
	private:
		enum Type { Undefined, AND, OR };
		Type t;

		QList<NVBSelectorCase> cases;
		NVBSelectorDataset dataset;

// 	bool optimal; // to save on optimize() calls

	public:

		NVBSelectorCase(int caseId = 0)
			: id(caseId)
			, t(Undefined)
// 	, optimal(true)
			{;}

		NVBSelectorCase & addCase(int caseId = 0);
		inline NVBSelectorCase & lastCase() { return (t == OR) ? cases.last() : *this; }

		NVBSelectorCase & setDataType(NVBDataSet::Type type);
		NVBSelectorCase & setDataName(QString name);
		NVBSelectorCase & setDataUnits(NVBUnits units);
		NVBSelectorCase & setDataNAxes(axisindex_t naxes);
		NVBSelectorCase & setDataMinAxes(axisindex_t naxes);
		NVBSelectorCase & setDataMaxAxes(axisindex_t naxes);

		NVBSelectorAxis addAxis();
		inline NVBSelectorAxis addAxisByName(QString name) { return addAxis().byName(name); }
		inline NVBSelectorAxis addAxisByIndex(int index) { return addAxis().byIndex(index); }
		inline NVBSelectorAxis addAxisByMinLength(int length) { return addAxis().byMinLength(length); }
		inline NVBSelectorAxis addAxisByMaxLength(int length) { return addAxis().byMaxLength(length); }
		inline NVBSelectorAxis addAxisByLength(int length) { return addAxis().byLength(length); }
		inline NVBSelectorAxis addAxisByUnits(NVBUnits dimension) { return addAxis().byUnits(dimension); }

		NVBSelectorMap addMap();
		inline NVBSelectorMap addMapByDimensions(axisindex_t d) { return addMap().byDimensions(d); }
		inline NVBSelectorMap addMapByMinDimensions(axisindex_t d) { return addMap().byMinDimensions(d); }
		inline NVBSelectorMap addMapByMapType(NVBAxisMap::MapType t) { return addMap().byMapType(t); }
		inline NVBSelectorMap addMapByValueType(NVBAxisMap::ValueType t) { return addMap().byValueType(t); }
		inline NVBSelectorMap addMapByTypeId(int typeId) { return addMap().byTypeID(typeId); }

		template <class T>
		inline NVBSelectorMap & addMapByType() { return addMapByTypeId(qMetaTypeId<T>()); }

		bool matches(const NVBDataSet * dataSet);
		NVBSelectorAxisInstanceList match(const NVBDataSet * dataSet) const
			{ return dataset.match(dataSet); }
		// No idea how to implement this function
		// First, dataset rules cannot be instantiated well
		// Second, map coverage is meaningless
		//
		// On the other hand, this function is useful
		// e.g. in icon drawing, since selecting 2 axes
		// in all datasets is faster than in one.
// 	NVBSelectorAxisInstanceList match(const NVBDataSource * dataSource) const
// 		{ return dataset.match(dataSource); }

		/// Returns an instance that reports all datasources that have matching datasets
		NVBSelectorFileInstance instantiate(const QList<NVBDataSource *> * dataSources);
		/// Returns an instance that reports one datasource from the list that has matching datasets
		NVBSelectorSourceInstance instantiateOneSource(const QList<NVBDataSource *> * dataSources);
		/// Returns an instance that reports the first matching dataset from the list
		NVBSelectorDataInstance instantiateOneDataset(const QList<NVBDataSource *> * dataSources);

		/// Returns an instance that reports all matching datasets (may be none)
		NVBSelectorSourceInstance instantiate(const NVBDataSource * dataSource);
		/// Returns an instance that reports the first matching dataset from the list
		NVBSelectorDataInstance instantiateOneDataset(const NVBDataSource * dataSource);

		/// Returns an instance that has info on all matching axes from the selector (may be invalid if no match)
		NVBSelectorDataInstance instantiate(const NVBDataSet * dataSet);

// 	void optimize();
//	NVBSelectorAxis & addDependentAxis();
	};

typedef NVBSelectorCase NVBAxisSelector;
// typedef NVBSelectorCase NVBAxesProps;

class NVBSelectorDataInstance {
		friend class NVBSelectorSourceInstance;
	private:
		bool valid;
		const NVBDataSet * dataSet;
		NVBSelectorCase s;
		QVector<axisindex_t> matchedaxes;
		QList<NVBAxisMapping> matchedmaps;
		mutable QVector<axisindex_t> otheraxes;

		NVBSelectorDataInstance(const NVBSelectorCase& selector, const NVBDataSet* dataset, NVBSelectorAxisInstanceList matched);

		void initAxes(NVBSelectorAxisInstanceList matched);

	public:
		NVBSelectorDataInstance() : valid(false), dataSet(0) {;}
		NVBSelectorDataInstance(const NVBSelectorCase& selector, const NVBDataSet* dataset = 0);
		NVBSelectorDataInstance(const NVBSelectorDataInstance & other);
		~NVBSelectorDataInstance();

		const NVBSelectorDataInstance& operator=(const NVBSelectorDataInstance & other);

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
		inline const QList<NVBAxisMapping> & matchedMaps() const { return matchedmaps; }

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
		/// Constructs an invalid instance
		NVBSelectorSourceInstance(const NVBSelectorCase & selector) : s(selector) {;}

		/// Constructs an instance matching the \a source
		NVBSelectorSourceInstance(const NVBSelectorCase & selector, const NVBDataSource * source);
		void fillInstances(const NVBSelectorCase & selector, const NVBDataSource* source);

		const NVBDataSource * matchingSource() const { return source;}
//	QVector<axisindex_t> matchedAxes();

		inline bool isValid() const { return !instances.isEmpty(); }
		inline void reset() { instances.clear(); }

		const QList<NVBDataSet *> & matchedDatasets(int selectorCase) const {
			static QList<NVBDataSet*> empty;

			if (sets.contains(selectorCase))
				return sets.find(selectorCase).value();
			else
				return empty;
			}
		inline const QList<NVBDataSet *> & matchedDatasets() const { return allSets; }

		const QList<NVBSelectorDataInstance> & matchedInstances(int selectorCase) const {
			static QList<NVBSelectorDataInstance> empty;

			if (instances.contains(selectorCase))
				return instances.find(selectorCase).value();
			else
				return empty;
			}
		inline const QList<NVBSelectorDataInstance> & matchedInstances() const { return allInstances; }

		const QList<int> & matchedIndexes(int selectorCase) const {
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

		const QList<NVBDataSource *> & matchedDatasources(int selectorCase) const {
			static QList<NVBDataSource*> empty;

			if (sources.contains(selectorCase))
				return sources.find(selectorCase).value();
			else
				return empty;
			}
		inline const QList<NVBDataSource *> & matchedDatasources() const { return allSources; }

		const QList<NVBSelectorSourceInstance> & matchedInstances(int selectorCase) const {
			static QList<NVBSelectorSourceInstance> empty;

			if (instances.contains(selectorCase))
				return instances.find(selectorCase).value();
			else
				return empty;
			}
		inline const QList<NVBSelectorSourceInstance> & matchedInstances() const { return allInstances; }

		inline const NVBSelectorSourceInstance & matchedInstance(const NVBDataSource * source) const
			{ return NVBSelectorFileInstance::instFromDatasource(allInstances, source); }
		inline const NVBSelectorSourceInstance & matchedInstance(const NVBDataSource * source, int selectorCase) const
			{ return NVBSelectorFileInstance::instFromDatasource(matchedInstances(selectorCase), source); }

		const QList<int> & matchedIndexes(int selectorCase) const {
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
