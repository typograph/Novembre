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

#ifndef NVBAXISSELECTORHELPERS_H
#define NVBAXISSELECTORHELPERS_H

// #ifdef NVB_DEBUG
// #define NVB_DEBUG_AXISSELECTOR
// #endif

#include <QtCore/QList>
#include "NVBDataGlobals.h"
#include "NVBUnits.h"
#include "NVBAxis.h"
#include "NVBMap.h"

class NVBDataSet;
class NVBDataSource;

struct NVBSelectorAxisInstance;

struct NVBSelectorRulesPrivate;
struct NVBSelectorAxis;
struct NVBSelectorMap;

struct NVBSelectorAxisInstance {
	
	// The number of additional needed axes
	int additionalAxes;
	// The indexes of axes that have been matched
	QList<axisindex_t> axes;
	// The maps that have been matched
	QList<NVBAxisMap *> maps;
	
	// Helper items, do not use
	QList<axisindex_t> unmatched;
	axisindex_t parent_axis;
	
// 	NVBSelectorAxisInstance(int addA = 0) : additionalAxes(addA) {;} // the list is empty anyway
// 	NVBSelectorAxisInstance(int addA, QList<axisindex_t> list) : additionalAxes(addA) , axes(list) {;}
	
	NVBSelectorAxisInstance()
		: additionalAxes(0)
		, parent_axis(-1)
		{;}
	
	/// Creates an instance with \p matched moved from unmatched to axes
	NVBSelectorAxisInstance(const NVBSelectorAxisInstance & other, axisindex_t matched)
		: additionalAxes(other.additionalAxes)
		, axes(other.axes)
		, maps(other.maps)
		, unmatched(other.unmatched)
		, parent_axis(matched)
		{
			axes.append(matched);
			unmatched.removeOne(matched);
		}
		
	/// Creates an instance with \p map added to maps and unmatched set to \p indexes
	NVBSelectorAxisInstance(const NVBSelectorAxisInstance & other, NVBAxisMap * map, QList<axisindex_t> indexes)
		: additionalAxes(other.additionalAxes)
		, axes(other.axes)
		, maps(other.maps)
		, unmatched(indexes)
		, parent_axis(-1)
		{
			maps.append(map);
		}
		
};

typedef QList< NVBSelectorAxisInstance > NVBSelectorAxisInstanceList;

#ifdef NVB_DEBUG
QDebug operator<<(QDebug dbg, const NVBSelectorAxisInstance &i);
#endif

struct NVBSelectorRules {
	NVBSelectorRulesPrivate * p;

	enum NVBSelectorRulesType
		{ None
		,	Axis
		, Map
		, Dataset
		};

	enum NVBSelectorRuleType
		{ Invalid = 0
		, Index     // Axis, ? Dataset
		, Name      // Axis, Dataset
		, MinSize   // Axis, Map, Dataset
		, MaxSize   // Axis, ? Map, Dataset
		, Size      // Axis, Map
		, Units     // Axis, Dataset
		, TypeID    // Map
		, ObjType   // Map, Dataset
		};

	enum NVBAxisBuddy
		{	Copy = 0
		, SameUnits = 1
		, SameLength = 2
		, SameMap = 4
		};
		
	NVBSelectorRules(NVBSelectorRulesType type);
	NVBSelectorRules(const NVBSelectorRules & other);
	NVBSelectorRules(NVBSelectorRulesPrivate * d);
	~NVBSelectorRules();
	
	bool hasRuleFor(NVBSelectorRuleType type);
	int ruleIndexFor(NVBSelectorRuleType type);
	
	NVBSelectorAxisInstanceList match(const NVBDataSet* dataset, const NVBSelectorAxisInstance& instance ) const;
	bool matchDataset(const NVBDataSet* dataset) const;
	NVBSelectorAxisInstanceList matchAxis(const NVBDataSet * dataset, const NVBSelectorAxisInstance & instance ) const;
	NVBSelectorAxisInstanceList matchMap(const NVBDataSet * dataset, const NVBSelectorAxisInstance & instance	) const;
	
	NVBSelectorRulesType type();

	void addRule(NVBUnits units);
	void addRule(QString name);
	void addRule(NVBSelectorRuleType type, int number);
	
	NVBSelectorRules deep_copy();	
	
	NVBSelectorRules & operator=(const NVBSelectorRules & other);
	
	void clear();
};

struct NVBSelectorMap : public NVBSelectorRules {

	enum NVBMapCoverage {
		Partial = 0, /// Only some of this map's axes have to belong to the matched dataset
		Full = 1 /// All the axes of the map have to come from the dataset
		};
		
	/// Creates a map selector with a given axis coverage
	NVBSelectorMap(NVBMapCoverage c = Full);
	~NVBSelectorMap() {}
	
	NVBSelectorMap & byDimensions(axisindex_t d) { addRule(NVBSelectorRules::Size,d); return *this; }
	NVBSelectorMap & byMinDimensions(axisindex_t d) { addRule(NVBSelectorRules::MinSize,d); return *this; }
	NVBSelectorMap & byMapType(NVBAxisMap::MapType t) { addRule(NVBSelectorRules::ObjType,t); return *this; }
	NVBSelectorMap & byValueType(NVBAxisMap::ValueType t);
	NVBSelectorMap & byTypeID(int type)  { addRule(NVBSelectorRules::TypeID,type); return *this; }

	template <typename T>
	inline NVBSelectorMap & byType() { return byTypeID(qMetaTypeId<T>()); }
	
	// This function returns a copy instead of a reference.
	// This copy uses the same internal object, so anything that
	// happens to it will happen to the original
	// And I don't know how to make it return a reference
	NVBSelectorAxis addAxis();

	NVBSelectorMap & operator=(const NVBSelectorMap & other) { NVBSelectorRules::operator=(other); return *this; }

};

struct NVBSelectorAxis : public NVBSelectorRules {

	NVBSelectorAxis() : NVBSelectorRules(NVBSelectorRules::Axis) {;}
	NVBSelectorAxis(NVBSelectorRulesPrivate * d);
	NVBSelectorAxis(const NVBSelectorAxis & other) : NVBSelectorRules(other) {;}
	~NVBSelectorAxis() {}
	
	NVBSelectorAxis & byName(QString name) { addRule(name); return *this; }
	NVBSelectorAxis & byIndex(int index); // Not in header, since this sule is prepended
	NVBSelectorAxis & byMinLength(int length) { addRule(NVBSelectorRules::MinSize,length); return *this; }
	NVBSelectorAxis & byMaxLength(int length) { addRule(NVBSelectorRules::MaxSize, length); return *this; }
	NVBSelectorAxis & byLength(int length) { addRule(NVBSelectorRules::Size, length); return *this; }
	NVBSelectorAxis & byUnits(NVBUnits dimension) { addRule(dimension); return *this; }
	
	NVBSelectorAxis & needMore(int more_axes, NVBSelectorRules::NVBAxisBuddy t = Copy);

	// This function returns a copy instead of a reference.
	// This copy uses the same internal object, so anything that
	// happens to it will happen to the original
	// And I don't know how to make it return a reference
	NVBSelectorMap addMap(NVBSelectorMap::NVBMapCoverage coverage);

	NVBSelectorAxis & operator=(const NVBSelectorAxis & other) { NVBSelectorRules::operator=(other); return *this; }
	
};

struct NVBSelectorDataset : public NVBSelectorRules {

	NVBSelectorDataset() : NVBSelectorRules(NVBSelectorRules::Dataset) {;}
	NVBSelectorDataset(const NVBSelectorDataset & other) : NVBSelectorRules(other) {;}
	~NVBSelectorDataset() {}
	
	NVBSelectorDataset & byName(QString name) { addRule(name); return *this; }
	NVBSelectorDataset & byUnits(NVBUnits units) { addRule(units); return *this; }
	NVBSelectorDataset & byType(NVBDataSet::Type type)
		{ addRule(NVBSelectorRules::ObjType, (int)type); return *this; }
	NVBSelectorDataset & byNAxes(axisindex_t naxes) {addRule(NVBSelectorRules::Size,naxes); return *this;}
	NVBSelectorDataset & byMinAxes(axisindex_t naxes) { addRule(NVBSelectorRules::MinSize,naxes);	return *this; }
	NVBSelectorDataset & byMaxAxes(axisindex_t naxes) { addRule(NVBSelectorRules::MaxSize,naxes); return *this; }
	

	NVBSelectorAxis addAxis();
	NVBSelectorMap addMap();

	NVBSelectorAxisInstanceList match(const NVBDataSet* dataset ) const
		{ return NVBSelectorRules::match(dataset,NVBSelectorAxisInstance()); }

	NVBSelectorDataset & operator=(const NVBSelectorDataset & other) { NVBSelectorRules::operator=(other); return *this; }	
};

#endif // NVBAXISSELECTORHELPERS_H
