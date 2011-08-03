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

#include "NVBAxisSelector.h"
#include "NVBMap.h"
#include "NVBAxisMaps.h"
#include "NVBDataSource.h"
#include "NVBDataCore.h"

NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase & selector, const NVBDataSource* source);
NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase & selector, const NVBDataSource* source, NVBSelectorAxisInstance instance, axisindex_t start);
NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase & selector, const NVBDataSource* source, NVBSelectorAxisInstance instance, axisindex_t startRule, axisindex_t startAxis, axisindex_t matched, const NVBAxis& buddy);

template<typename T>
void uniquify( QList<T> & list ) {
	qSort(list);
	for(int i = 1; i<list.count(); i++) {
		if (list.at(i) == list.at(i-1))
			list.removeAt(--i);
		}
}


class NVBSelectorRulePrivate;
/*
**
 * 
 *  \class NVBAxisSelector
 * 
 * This class is a part of the mechanism that allows one
 * to check if the available data conforms to a set of rules.
 * E.g. one can check if the data has enough axes for a 2D FFT
 * transform or select two axes with the same units -
 * the possibilities are quite vast.
 * 
 * The examples below show some use cases.
 * 
 * \li Make sure the dataset has at least 3 axes:
 * \code
 * NVBAxisSelector s;
 * s.addAxis().need(3);
 * \endcode
 * 
 * \li Make sure the dataset has 2 axes with the same units and at least one more
 * \code
 * NVBAxisSelector s;
 * s.addAxis().need(2,NVBSelectorAxis::Units);
 * s.addAxis();
 * \endcode
 * 
 * \li Make sure the dataset has 2 metric axes
 * \code
 * NVBAxisSelector s;
 * s.addAxisByUnits("m").need(2);
 * \endcode
 * 
 * \li Select the X and Y axes
 * \code
 * NVBAxisSelector s;
 * s.addAxisByName("X");
 * s.addAxisByName("Y");
 * \endcode
 *
 * 
 * All CASES
 * \code
 * NVBAxisSelector s;
 * s.addAxisByIndex(0).need(2); // ERROR
 * s.addAxisByIndex(0).need(2,NVBSelectorAxis::SameUnits); // The buddy ignores the index rule
 * s.addAxisByIndex(0).need(2,NVBSelectorAxis::SameMap);   // The buddy ignores the index rule
 * s.addAxisByName("X").need(2); // ERROR
 * s.addAxisByName("X").need(2,NVBSelectorAxis::SameUnits); // The buddy ignores the name rule
 * s.addAxisByName("X").need(2,NVBSelectorAxis::SameMap);   // The buddy ignores the name rule
 * s.addAxisByUnits("m").need(2,NVBSelectorAxis::SameUnits); // Equivalent to s.addAxisByUnits("m").need(2)
 * s.addAxisByUnits("m").need(2,NVBSelectorAxis::SameMap);  // Same units && some same map 
 * s.addAxisByUnits("m").need(2,NVBSelectorAxis::SameMap | NVBSelectorAxis::SameUnits);  // Same PhysValue map
 * s.addAxisByMinLength(100).need(3,NVBSelectorAxis::SameUnits); // 3 axes with min length 100 and same units
 * s.addAxisByMinLength(100).need(3,NVBSelectorAxis::SameLength); // 3 axes with same length >= 100
 * s.addAxis().need(3,NVBSelectorAxis::SameMap); // 3 axes sharing a map
 * s.addAxisByLength(100).need(3,NVBSelectorAxis::SameMap); // 3 axes length 100 sharing a map
 * s.addAxisByMinLength(100).need(3,NVBSelectorAxis::SameMap | NVBSelectorAxis::SameLength); // 3 axes with same length >= 100 sharing a map
 * s.addAxisByTypeId<int>().need(2,NVBSelectorAxis::SameUnits); // both axes will have a map of type int, and same units
 * \endcode 
 * 
 * two types - TypeId and Units - suppose a map on this axis only
 * They will only match 2D or 3D maps if need(>1,SameMap) is specified
 * 
 * ALTERNATIVE API:
 * 
 * addMap(Inside)
 * addMap(Any)
 * 
 * map.byDimensions(3)
 * map.byMinDimensions(2)
 * map.byType<int>()
 * 
 *

struct NVBSelectorRules {
	NVBSelectorRulePrivate * p;

	NVBSelectorRules();
	~NVBSelectorRules() {if (p) delete p;}
	
	bool hasRuleFor(NVBAxisPropertyType type);
	bool hasRuleFor(NVBMapPropertyType type);
	
	bool matches(const NVBAxis & axis, QList<axisindex_t> moreaxes ) const;
};

struct NVBSelectorMap : public NVBSelectorRules {
	NVBSelectorRulePrivate * p;

	enum NVBMapCoverage {
		Partial = 0,
		Full = 1
		};
	
	enum NVBMapProperty {
		};
	
	NVBSelectorMap(NVBMapCoverage c = Full) : NVBSelectorRules() {;}
	~NVBSelectorRules();
	
	NVBSelectorMap & byDimensions(axisindex_t d);
	NVBSelectorMap & byMinDimensions(axisindex_t d);
	NVBSelectorMap & byMapType(NVBAxisMap::MapType t);
	NVBSelectorMap & byValueType(NVBAxisMap::ValueType t);
	NVBSelectorMap & byTypeID(int type);

	template <typename T>
	inline NVBSelectorMap & byType<T>() { return byTypeID(qMetaTypeId<T>()); }
	
};
*/

// NVBSelectorAxis

struct NVBAxisProperty {
	NVBSelectorAxis::NVBAxisPropertyType type;
	int i;
	NVBUnits u;
	QString n;

	NVBAxisProperty(NVBSelectorAxis::NVBAxisPropertyType t, int ii)
		: type(t)
		, i(ii)
		{;}
	NVBAxisProperty(QString name)
		: type(NVBSelectorAxis::Name)
		, n(name)
		{;}
	NVBAxisProperty(NVBUnits uu)
		: type(NVBSelectorAxis::Units)
		, u(uu)
		{;}
	};

struct NVBSelectorAxisPrivate {
	int refCount;
	int mult;
	NVBSelectorAxis::NVBAxisPropertyType multType;
	QList<NVBAxisProperty> axisProperties;

	NVBSelectorAxisPrivate() : refCount(0), mult (1) {;}
};

NVBSelectorAxis::NVBSelectorAxis() : p(0) {
	p = new NVBSelectorAxisPrivate();
	p->refCount = 1;
	}

NVBSelectorAxis::NVBSelectorAxis(const NVBSelectorAxis & other) {
	p = other.p;
	p->refCount += 1;
	}

NVBSelectorAxis::~NVBSelectorAxis() {
	p->refCount -= 1;
	if (p->refCount < 1) delete(p);
	}

bool NVBSelectorAxis::needMore(int matched) const {
	if (p) return p->mult > matched;
	else return false;
}

bool NVBSelectorAxis::hasRuleFor(NVBSelectorAxis::NVBAxisPropertyType type)
{
	if (!p) return false;
	for (int i = p->axisProperties.count() - 1; i>=0; i--)
		if (p->axisProperties.at(i).type == type)
			return true;
	return false;
}


NVBSelectorAxis& NVBSelectorAxis::byIndex(int index)
	{
		if (p) {
			if (p->mult > 1)
				NVBOutputError("Multiple axes cannot match by index");
			else
				p->axisProperties.prepend(NVBAxisProperty(NVBSelectorAxis::Index, index));
			}
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byMapDimensions(int dimension)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::MapDimensions, dimension));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byMinLength(int length)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::MinLength, length));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byMaxLength(int length)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::MaxLength, length));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byLength(int length)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::Length, length));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byTypeId(int typeId)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::TypeID, typeId));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byUnits(NVBUnits dimension)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(dimension));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byName(QString name)
	{
		if (p) {
			if (p->mult > 1)
				NVBOutputError("Multiple axes cannot match by name");
			else
				p->axisProperties.append(NVBAxisProperty(name));
			}
		return *this;
	}


NVBSelectorAxis& NVBSelectorAxis::need(int more_axes, NVBSelectorAxis::NVBAxisPropertyType t)
	{
		if (p) {
			if (p->mult != 1)
				NVBOutputError("This rule was already marked for multiple axes. Parameters will be overwritten.");
			switch(t) {
				case Index:
				case Name:
				case MinLength:
				case MaxLength:
					NVBOutputError("Multiple axes can't match on the same index, name or min/max length. Reverting to default.");
					p->multType = Invalid;
					break;
				default:
					if (hasRuleFor(Name) || hasRuleFor(Index)) {
						NVBOutputError("Multiple axes cannot match by index or name");
						p->multType = Invalid;
						return *this;
				}
			else
				p->multType = t;
			}
			p->mult = more_axes;
			}
		return *this;
	}

bool NVBSelectorAxis::matches(const NVBAxis & axis, const NVBAxis & buddy) const
	{
		NVBOutputDMsg(QString("Trying to match axis %1").arg(axis.name()));
		if (!p) return true;
		NVBOutputDMsg(QString("Buddy axis %1").arg(buddy.isValid() ? buddy.name() : "not specified"));
		if (!buddy.isValid() || p->multType == Invalid || p->mult == 1) {
			if (p->axisProperties.isEmpty()) {
				NVBOutputDMsg("Empty rules matched");
				return true;
				}
			foreach(NVBAxisProperty ap, p->axisProperties) {
				switch(ap.type) {
					case Index :
						NVBOutputDMsg(QString("Trying to match by index %1").arg(ap.i));
						if (!buddy.isValid() && axis.dataSource()->axis(ap.i) != axis) return false;
						break;
					case Name :
						NVBOutputDMsg(QString("Trying to match by name %1").arg(ap.n));
						if (axis.name() != ap.n) return false;
						break;
					case MinLength :
						NVBOutputDMsg(QString("Trying to match by min. length %1").arg(ap.i));
						if (axis.length() < (axissize_t) ap.i) return false;
						break;
					case MaxLength :
						NVBOutputDMsg(QString("Trying to match by max. length %1").arg(ap.i));
						if (axis.length() > (axissize_t) ap.i) return false;
						break;
					case Length :
						NVBOutputDMsg(QString("Trying to match by length %1").arg(ap.i));
						if (axis.length() != (axissize_t) ap.i) return false;
						break;
					case MapDimensions : {
						NVBOutputDMsg(QString("Trying to match by %1D map").arg(ap.i));
						bool b = false;
						foreach(NVBAxisMapping m, axis.maps())
							b = b || (m.map->dimension() == ap.i);
						if (!b) return false;
						break;
						}
					case TypeID : {
						NVBOutputDMsg(QString("Trying to match by type %1").arg(QMetaType::typeName(ap.i)));
						bool b = false;
						foreach(NVBAxisMapping m, axis.maps())
							b = b || (m.map->valType() == ap.i);
						if (!b) return false;
						break;
						}
					case Units :
						NVBOutputDMsg(QString("Trying to match by units [%1]").arg(ap.u.baseUnit()));
						NVBOutputDMsg(axis.physMap() ? QString("Axis is in [%1]").arg(axis.physMap()->units().baseUnit()) : QString("No units available"));
						if (!axis.physMap() || !axis.physMap()->units().isComparableWith(ap.u)) return false;
						break;
					default:
						return false;
					};
				};
			return true;
			}
		else {
			if (buddy.dataSource() != axis.dataSource()) return false;
			switch(p->multType) {
				case Index :
					NVBOutputError("Two axes can't match by index");
					return false;
				case Name :
					NVBOutputDMsg(QString("Trying to match buddy by name %1").arg(buddy.name()));
					return (buddy.name() == axis.name());
				case MinLength :
					NVBOutputDMsg(QString("Trying to match buddy by min. length %1").arg(buddy.length()));
					return (buddy.length() >= axis.length());
				case MaxLength :
					NVBOutputDMsg(QString("Trying to match buddy by max. length %1").arg(buddy.length()));
					return (buddy.length() <= axis.length());
				case Length :
					NVBOutputDMsg(QString("Trying to match buddy by length %1").arg(buddy.length()));
					return (buddy.length() == axis.length());
				case MapDimensions : {
					NVBOutputDMsg(QString("Trying to match buddy by map"));
					// This case is interpreted as : we look for a map spanning both axes with map dimension as specified.
					axisindex_t i = buddy.parentIndex(); // we are looking in this axis's maps for the one also on buddy
					axisindex_t md = 0;
					foreach(NVBAxisProperty q, p->axisProperties) {
						if (q.type == MapDimensions) {
							md = (axisindex_t) q.i;
							break;
							}
						}
					bool b = false;
					foreach(NVBAxisMapping m, axis.maps())
						b = b || ((!md || m.map->dimension() == md) && m.axes.contains(i) && m.axes.count() >= p->mult);
					return b;
					// The last clause is there to guard agains a case where there are two maps with correct
					// dimensions, we need three axes, but one of the maps has the first and the second,
					// and the other the first and the third.
					// This method might still fail if we need three axes, and the two matching maps are on 1,2,4 and 1,3,5
					// The matching will select axes 1,2 and 3. However, this is quite unlikely to happen.
					// FIXME This case should be documented, however.
					// One possible workaround is to change the matching procedure.
					}
				case TypeID : {
					NVBOutputDMsg(QString("Trying to match buddy by type"));
					// This case is interpreted as : we look for a map spanning both axes with map type as specified.
					axisindex_t i = buddy.parentIndex(); // we are looking in this axis's maps for the one also on buddy
					int mt = QMetaType::Void;
					foreach(NVBAxisProperty q, p->axisProperties) {
						if (q.type == TypeID) {
							mt = q.i;
							break;
							}
						}
					bool b = false;
					foreach(NVBAxisMapping m, axis.maps())
						b = b || ((!mt || m.map->valType() == mt) && m.axes.contains(i) && m.axes.count() >= p->mult);
					return b;
					}
				case Units : {
					NVBOutputDMsg(QString("Trying to match buddy by units [%1]").arg(buddy.physMap() ? buddy.physMap()->units().baseUnit() : "NULL"));
					// This case is interpreted as follows :
					// If the rules include units, we look for a map spanning both axes with map units as specified.
					// If the rules don't include units, we look for a map with same units.
					bool inc = false;
					foreach(NVBAxisProperty q, p->axisProperties) {
						if (q.type == Units) {
							inc = true;
							break;
							}
						}
					if (inc)
						return buddy.maps().at(
							buddy.maps().indexOf(
								NVBAxisMapping(
									buddy.physMap(),
									QVector<axisindex_t>()
							))).axes.contains(axis.parentIndex());
					else {
						if (buddy.physMap() && axis.physMap())
							return buddy.physMap()->units().isComparableWith(axis.physMap()->units());
						return !buddy.physMap() && !axis.physMap();
						}
					}
				default:
					return false;
				};			
			}
}

// NVBSelectorCase


NVBSelectorCase::NVBSelectorCase(const NVBSelectorCase & other) {
	id = other.id;
	t = other.t;
	optimal = other.optimal;
	switch (other.t) {
		case NVBSelectorCase::AND :
			axes = other.axes;
			break;
		case NVBSelectorCase::OR :
			cases = other.cases;
			break;
		default:
			break;
		}
	}

NVBSelectorCase & NVBSelectorCase::addCase(int caseId, Type caseType) {
	if (t == NVBSelectorCase::AND) {
		NVBOutputError("Trying to add a Case to an AND-case. Case type changed to OR.");
		axes.clear();
		}

	t = NVBSelectorCase::OR;

	cases.append( NVBSelectorCase(caseId,caseType) );
	return cases.last();
}

NVBSelectorAxis & NVBSelectorCase::addAxis() {
	if (t == NVBSelectorCase::OR) {
		NVBOutputError("Trying to add an Axis to an OR-case. Case type changed to AND.");
		cases.clear();
		}

	t = NVBSelectorCase::AND;

	optimal = axes.isEmpty();

	axes.append(NVBSelectorAxis());
	return axes.last();
}

NVBSelectorCase& NVBSelectorCase::setType(NVBDataSet::Type type)
{
	target = type;
	return *this;
}

bool NVBSelectorCase::matches(const NVBDataSet* dataSet) {
	return instantiate(dataSet).isValid();
}


/**
	* @brief optimizes axis selector rules to put index-limited ones first.
	**/
void NVBSelectorCase::optimize() {
	switch (t) {
		case NVBSelectorCase::AND : {
			if (optimal) return;
			optimal = true;
			for(int i0 = 0, i = 0, in = axes.count()-1; i <= in ; i+=1) {
				NVBSelectorAxis a = axes.at(i);
				if (a.p) {
					if (a.p->axisProperties.isEmpty()) {
						if (axes.at(in).p->axisProperties.isEmpty() && a.p->multType != NVBSelectorAxis::Invalid)
							continue;
						axes.swap(i,in);
						in -= 1;
						i -= 1;
					}
					else if (a.p->axisProperties.first().type == NVBSelectorAxis::Index && !a.needMore(1)) {
						if (i != i0) axes.swap(i,i0);
						i0 += 1;
						}
					}
				}
			break;
			}
		case NVBSelectorCase::OR : {
			foreach(NVBSelectorCase c, cases)
				c.optimize();
			break;
			}
		default:
			NVBOutputError("Unrecognized selector type");
			break;
		}
	}

/**
 *
 * \fn NVBSelectorCase::instantiate(QList<NVBDataSource *> dataSources)
 * 
 * Armed with a list of datasources, this function tries to find the best match between
 * them. The best match is defined as the datasource that has the biggest number of matching axes.
 * In case of competition between different cases and/or datasources, the priority is with
 * the first in the list.
 * 
 * The resulting instance might have invalid indices in the matchedAxes(), that correspond
 * to unmatched axes. 
 */

NVBSelectorFileInstance NVBSelectorCase::instantiate(const QList< NVBDataSource* > * dataSources)
{
	if (t != NVBSelectorCase::AND) {
		NVBOutputError("Only one case per selector is supported when instantiating on list");
		return NVBSelectorFileInstance(*this);
		}

	if (axes.count() == 0) {
		NVBOutputPMsg("Called with empty case : might be a bug");
		return NVBSelectorFileInstance(*this);		
		}

	optimize();

	return NVBSelectorFileInstance(*this,*dataSources);
		}
		
NVBSelectorSourceInstance NVBSelectorCase::instantiateOneSource(const QList< NVBDataSource* > * dataSources)
{
	optimize();

	return NVBSelectorFileInstance(*this,*dataSources).matchedInstances().first();
}


NVBSelectorDataInstance NVBSelectorCase::instantiateOneDataset(const QList< NVBDataSource* > * dataSources)
{
	optimize();

	return NVBSelectorFileInstance(*this,*dataSources).matchedInstances().first().matchedInstances().first();	
}

NVBSelectorSourceInstance NVBSelectorCase::instantiate(const NVBDataSource* dataSource)
{
	optimize();
	
	return NVBSelectorSourceInstance(*this,dataSource);
}

NVBSelectorDataInstance NVBSelectorCase::instantiateOneDataset(const NVBDataSource* dataSource)
{
	optimize();

	return NVBSelectorSourceInstance(*this,dataSource).matchedInstances().first();
}

NVBSelectorDataInstance NVBSelectorCase::instantiate(const NVBDataSet* dataSet) {
	NVBOutputDMsg(QString("Trying to match case %1").arg(id));
	switch (t) {
		case NVBSelectorCase::AND : {
			return NVBSelectorDataInstance(*this,dataSet);
			break;
			}
		case NVBSelectorCase::OR : {
			foreach(NVBSelectorCase c, cases) {
				NVBSelectorDataInstance i = c.instantiate(dataSet);
				if (i.isValid()) return i;
				}
			break;
			}
		case NVBSelectorCase::Undefined : {
			NVBOutputError(QString("Case %1 has not been well-defined").arg(id));
			break;
			}
		default:
			NVBOutputError(QString("Unrecognized selector type %1 in case %2").arg(t).arg(id));
			break;
		}
		return NVBSelectorDataInstance(*this);
}


/**
	* When NVBSelectorInstance is constructed on a dataset, it is looking for a set of axes,
	* matching the supplied rules. The axes defined by the rules are aggregated into \fn matchedAxes(),
	* and the other axes into \fn otherAxes(). If the rules couldn't be matched, the constructed instance
	* is invalid.
	*
	* @param selector Rules to match with
	* @param ds Dataset to match to
	**/

/**
* When NVBSelectorInstance is constructed on a datasource, it is matching each axis of the rules
* against an axis of this dataSource. The axes defined by the rules are aggregated into \fn matchedAxes(),
* with the non-matched rule places filled with '-1'. The other axes are put into \fn otherAxes().
* If no rule matched, the constructed instance is invalid.
* TODO To convert this instance into a dataset-based one, use \fn matchDataset()
*
* @param selector The NVBSelectorCase to be matched
* @param ds DataSource to match \a selector against
**/

// -------------------------------- NVBSelectorDataInstance

/// \a selector has to be an AND-selector
NVBSelectorDataInstance::NVBSelectorDataInstance(const NVBSelectorCase & selector, const NVBDataSet* dataset, NVBSelectorAxisInstanceList matched)
 : valid(false)
, dataSet(dataset)
 , s(selector)
	{
	if (dataset)
		initAxes(matched);
}


NVBSelectorDataInstance::NVBSelectorDataInstance(const NVBSelectorCase & selector, const NVBDataSet* dataset)
: valid(false)
, dataSet(dataset)
, s(selector)
{
	if (dataset)
		initAxes(instantiateRules(s,dataset->dataSource()));
}

#ifdef NVB_DEBUG
 QDebug operator<<(QDebug dbg, const NVBSelectorAxisInstance &i)
 {
     dbg.nospace() << "( " << i.additionalAxes << "+" << i.axes << ")";

     return dbg.space();
 }
#endif

void NVBSelectorDataInstance::initAxes(NVBSelectorAxisInstanceList matched)
{
	QVector<axisindex_t> dsaxes = dataSet->parentIndexes();
	
	otheraxes.clear();
	
//	qSort(dsaxes);
	foreach(NVBSelectorAxisInstance variant, matched) {
		matchedaxes.clear();
		foreach(axisindex_t vi, variant.axes) {
			int i = dsaxes.indexOf(vi);
			if (i >= 0 )
				matchedaxes.append(i);
			else
				break;
			}
		if (matchedaxes.count() == variant.axes.count()) {
			int add = variant.additionalAxes;
			if (matchedaxes.count() + add > dsaxes.count()) {
				matchedaxes.clear();
				return;
				}
			for(axisindex_t i=0; i < dsaxes.count(); i++)
				if (!matchedaxes.contains(i)) {
					if (add) {
						add -= 1;
						matchedaxes << i;
						}
					else
						otheraxes << i;
					}
			valid = true;
//			matchedmaps = variant.maps;
			return;
			}
		}
}

int NVBSelectorDataInstance::matchedCase() const
{
	return (valid ? s.id : -1);
}

NVBAxis NVBSelectorDataInstance::matchedAxis(axisindex_t i) const
{
	if (dataSet)
		return dataSet->axisAt(matchedaxes.at(i));
	return NVBAxis();
}

NVBAxis NVBSelectorDataInstance::otherAxis(axisindex_t i) const
{
	if (otheraxes.isEmpty() && valid && matchedaxes.count() != dataSet->nAxes() ) { // fill in
		otheraxes.reserve(dataSet->nAxes());
		for(int i = 0; i < dataSet->nAxes(); i++)
			if (!matchedaxes.contains(i))
				otheraxes.append(i);
		otheraxes.squeeze();
		}
		
	if (dataSet)
		return dataSet->axisAt(i);
	return NVBAxis();
}

// ------------- NVBSelectorSourceIstance

NVBSelectorSourceInstance::NVBSelectorSourceInstance(const NVBSelectorCase& selector, const NVBDataSource* source)
: s(selector)
{
	this->source = source;
	fillInstances(selector,source);	
	qSort(allIndexes); // TODO if we don't sort this, the indexes and sets have the same order. do we need it?
}

void NVBSelectorSourceInstance::fillInstances(const NVBSelectorCase& selector, const NVBDataSource* source)
{
	if (selector.t == NVBSelectorCase::OR) {
		foreach(NVBSelectorCase subs, selector.cases)
			fillInstances(subs, source);
			return;
			}
			
	QList<NVBSelectorDataInstance> listInst;
	QList<NVBDataSet*> listS;
	QList<int> listIx;

	NVBSelectorAxisInstanceList axes = instantiateRules(selector,source);

	for(int ix = 0 ; ix < source->dataSets().count(); ix += 1) {
		NVBSelectorDataInstance i = NVBSelectorDataInstance(selector,source->dataSets().at(ix),axes);
		if (i.isValid()) {
			listInst << i;
			listS << source->dataSets().at(ix);
			if (!allSets.contains(listS.last()))
				allSets.append(listS.last());
			listIx << ix;
			if (!allIndexes.contains(ix))
				allIndexes.append(ix);
			}
		}
	
	if (!listInst.isEmpty()) {
		instances.insert(selector.id,listInst);
		allInstances.append(listInst);
		sets.insert(selector.id,listS);
//		allSets.append(listS);
		indexes.insert(selector.id,listIx);
//		allIndexes.append(listIx);
		}
	
}

NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase& selector, const NVBDataSource* source, NVBSelectorAxisInstance instance, axisindex_t startRule, axisindex_t startAxis, axisindex_t matched, const NVBAxis& buddy)
{

	if (startAxis == source->nAxes()) {
		NVBOutputDMsg(QString("No valid additional axes"));
		return NVBSelectorAxisInstanceList();
	}
	
	NVBSelectorAxisInstanceList list;
	
	for(axisindex_t i = startAxis; i < source->nAxes(); i++) {
		if (instance.axes.contains(i)) {
			NVBOutputDMsg(QString("Skipping already matched axis %1").arg(i));
			continue;
			}
		NVBOutputDMsg(QString("Trying to match axis %1 against rule %2%3").arg(i).arg(startRule).arg(QString(buddy.isValid() ? " with buddy" : "")));
		if (selector.axes.at(startRule).matches(source->axis(i), buddy)) {
			instance.axes << i;
			matched += 1;
			NVBSelectorAxisInstanceList lcont;
			if (!selector.axes.at(startRule).needMore(matched))
				lcont = instantiateRules(selector,source,instance,startRule+1);
			else {
				NVBOutputDMsg(QString("More axes needed..."));
				lcont = instantiateRules(selector,source,instance,startRule,i+1,matched,source->axis(i));
				}
			if (!lcont.isEmpty()) {
				NVBOutputDMsg(QString("Added a new combination. Trying next branch..."));
				list.append(lcont);
				}
			else
				NVBOutputDMsg(QString("Fail. Trying next branch..."));				
			matched -= 1;
			instance.axes.removeLast();
			}
		else
			NVBOutputDMsg(QString("Match axis %1 against rule %2 failed").arg(i).arg(startRule));
		}
	
	return list;
}

NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase& selector, const NVBDataSource* source, NVBSelectorAxisInstance instance, axisindex_t start)
{
	if (start == selector.axes.count()) {
		NVBOutputDMsg(QString(start ? "All rules matched" : "Empty case"));
		return NVBSelectorAxisInstanceList() << instance;
	}
	
	const NVBSelectorAxis a = selector.axes.at(start);
	
	if (!a.p || (a.p->axisProperties.isEmpty() && a.p->multType == NVBSelectorAxis::Invalid)) {
		NVBOutputDMsg(QString("Rule %1 is a 'counting rule'. Wrapping up the tail.").arg(start));
		
		if (a.p)
			instance.additionalAxes += a.p->mult;
		else
			instance.additionalAxes += 1;
		start += 1;
		
		while(start < selector.axes.count()) {
			if (selector.axes.at(start).p) {
				NVBOutputDMsg(QString("Added %1 arbitrary axes").arg(selector.axes.at(start).p->mult));
				instance.additionalAxes += selector.axes.at(start).p->mult;
				}
			else {
				NVBOutputDMsg(QString("Rule data empty for rule %1. Added 1 arbitrary axis").arg(start));
				instance.additionalAxes += 1;
				}
			start += 1;
			}
			
		return NVBSelectorAxisInstanceList() << instance;
	}

	return instantiateRules(selector,source,instance,start,0,0,NVBAxis());
}

NVBSelectorAxisInstanceList instantiateRules(const NVBSelectorCase& selector, const NVBDataSource* source) {
	NVB_ASSERT(selector.t == NVBSelectorCase::AND,"Cannot instantiate container cases");

	// First, check the first axes for simplicity (with specified index)
	
	axisindex_t start = 0;
	NVBSelectorAxisInstance instance;
	while(start < selector.axes.count()) {
		NVBOutputDMsg(QString("Checking rule %1 for simplicity").arg(start));
		const NVBSelectorAxis a = selector.axes.at(start);
		// Check if the rule is for "just an axis"
			if (!a.p || a.p->axisProperties.isEmpty() || a.p->axisProperties.first().type != NVBSelectorAxis::Index || a.needMore(1)) {
			NVBOutputDMsg(QString("Rule %1 is not simple: %2").arg(start).arg(a.p ? (a.p->axisProperties.isEmpty() ? "Empty rule data" : (a.p->axisProperties.first().type == NVBSelectorAxis::Index ? "More than one axis needed" : "Rule is not an index rule") ) : "NULL rule data"));
				break;
				}
			axisindex_t k = (axisindex_t)a.p->axisProperties.first().i;
		if (a.matches(source->axis(k))) {
				NVBOutputDMsg(QString("Index axis matched at %1").arg(k));
			instance.axes << k;
			start += 1;
				}
		else {
				NVBOutputDMsg(QString("Index axis didn't match at %1").arg(k));
			return NVBSelectorAxisInstanceList();
				}
			}

	return instantiateRules(selector,source,instance,start);
	}

// ------------- NVBSelectorFileInstance

NVBSelectorFileInstance::NVBSelectorFileInstance(const NVBSelectorCase & selector, const QList< NVBDataSource* >& sources) : s(selector)
	{
	fillInstances(selector,sources);
	fillLists();
			}
			
void NVBSelectorFileInstance::fillInstances(const NVBSelectorCase & selector, const QList< NVBDataSource* > & sources)
{
	if (selector.t == NVBSelectorCase::OR) {
		foreach(NVBSelectorCase subs, selector.cases)
			fillInstances(subs, sources);
		return;
			}
			
	QList<NVBSelectorSourceInstance> listInst;
	QList<NVBDataSource*> listS;
	QList<int> listIx;
	
	for(int ix = 0 ; ix < sources.count(); ix += 1) {
		NVBSelectorSourceInstance i = NVBSelectorSourceInstance(selector,sources.at(ix));
		if (i.isValid()) {
			listInst << i;
			listS << sources.at(ix);
			listIx << ix;
			}
	}

	if (!listInst.isEmpty()) {
		instances.insert(selector.id,listInst);
		allInstances.append(listInst);
		this->sources.insert(selector.id,listS);
		allSources.append(listS);
		indexes.insert(selector.id,listIx);
		allIndexes.append(listIx);
	}
}

void NVBSelectorFileInstance::fillLists()
{
	uniquify(allSources);
	uniquify(allIndexes);
}

const NVBSelectorSourceInstance& NVBSelectorFileInstance::instFromDatasource(const QList< NVBSelectorSourceInstance >& list, const NVBDataSource* source) const
{
	static NVBSelectorSourceInstance empty(NVBSelectorCase(),0);
	
	for(int j=0; j<list.count(); j++)
		if (list.at(j).matchingSource() == source)
			return list.at(j);
		
	empty = NVBSelectorSourceInstance(s,source);
	return empty;
	}
