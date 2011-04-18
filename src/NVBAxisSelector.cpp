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

// NVBSelectorAxis

struct NVBAxisProperty {
	NVBSelectorAxis::NVBAxisPropertyType type;
	int i;
	NVBUnits u;

	NVBAxisProperty(NVBSelectorAxis::NVBAxisPropertyType t, int ii)
		: type(t)
		, i(ii)
		{;}
	NVBAxisProperty(NVBSelectorAxis::NVBAxisPropertyType t, NVBUnits uu)
		: type(t)
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

NVBSelectorAxis& NVBSelectorAxis::byIndex(int index)
	{
		if (p) p->axisProperties.prepend(NVBAxisProperty(NVBSelectorAxis::Index, index));
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
		if (p) p->axisProperties.append(NVBAxisProperty(NVBSelectorAxis::Units, dimension));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::need(int more_axes, NVBAxisPropertyType type)
	{
		if (p) {
			p->mult = more_axes;
			if (type == Index) {
				NVBOutputError("Multiple axes can't match on the same index. Reverting to default.");
				p->multType = Index;
				}
			else
				p->multType = type;
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
		NVBOutputError("Trying to add a Case to an AND-case");
		axes.clear();
		}

	t = NVBSelectorCase::OR;

	cases.append( NVBSelectorCase(caseId,caseType) );
	return cases.last();
}

NVBSelectorAxis & NVBSelectorCase::addAxis() {
	if (t == NVBSelectorCase::OR) {
		NVBOutputError("Trying to add an Axis to an OR-case");
		cases.clear();
		}

	t = NVBSelectorCase::AND;

	axes.append(NVBSelectorAxis());
	return axes.last();
}

// NVBAxisSelector

NVBSelectorInstance NVBSelectorCase::instantiate(const NVBDataSet* dataSet) {
	NVBOutputDMsg(QString("Trying to match case %1").arg(id));
	switch (t) {
		case NVBSelectorCase::AND : {
			return NVBSelectorInstance(this,dataSet);
			break;
			}
		case NVBSelectorCase::OR : {
			foreach(NVBSelectorCase c, cases) {
				NVBSelectorInstance i = c.instantiate(dataSet);
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
		return NVBSelectorInstance(this);
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

NVBSelectorInstance NVBSelectorCase::instantiate(QList<NVBDataSource *> dataSources) {
	// Brute-force : get an instance for each datasource, check where more axes matched
	// Easier if there's only one selector case
	if (t != NVBSelectorCase::AND) {
		NVBOutputError("Only one case per selector is supported when instantiating on list");
		return NVBSelectorInstance(this);
		}

	if (axes.count() == 0) {
		NVBOutputPMsg("Called with empty case : might be a bug");
		return NVBSelectorInstance(this);		
		}

	NVBSelectorInstance result(this);
	int maxmatched = 0;
	foreach(const NVBDataSource * source, dataSources) {
		NVBSelectorInstance i(this,source);
		int matched = i.matchedAxes().count() - i.matchedAxes().count(-1);
		if (matched > maxmatched) {
			result = i;
			maxmatched = matched;
			}
		}
		
	return result;
}

/*
NVBSelectorInstance NVBSelectorCase::instantiate(const NVBDataSource* dataSource) {
	NVBOutputDMsg(QString("Trying to match case %1").arg(id));
	switch (t) {
		case NVBSelectorCase::AND : {
			return NVBSelectorInstance(this,dataSource);
			break;
			}
		case NVBSelectorCase::OR : {
			foreach(NVBSelectorCase c, cases) {
				NVBSelectorInstance i = c.instantiate(dataSource);
				if (i.isValid()) return i;
				}
			break;
			}
		default:
			NVBOutputError(QString("Unrecognized selector type %1 in case %2").arg(t).arg(id));
			break;
		}
		return NVBSelectorInstance(this);
	}
*/
bool NVBSelectorCase::matches(const NVBDataSet* dataSet) {
	return instantiate(dataSet).isValid();
}


/**
	* @brief optimizes axis selector rules to put index-limited ones first.
	**/
void NVBSelectorCase::optimize() {
	switch (t) {
		case NVBSelectorCase::AND : {
			for(int i0 = 0, i = 0, in = axes.count()-1; i <= in ; i+=1)
				if (axes.at(i).p) {
					if (axes.at(i).p->axisProperties.isEmpty()) {
						axes.swap(i,in);
						in -= 1;
						i -= 1;
					}
					else if (axes.at(i).p->axisProperties.first().type == NVBSelectorAxis::Index && !axes.at(i).needMore(1)) {
						if (i != i0) axes.swap(i,i0);
						i0 += 1;
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
	* When NVBSelectorInstance is constructed on a dataset, it is looking for a set of axes,
	* matching the supplied rules. The axes defined by the rules are aggregated into \fn matchedAxes(),
	* and the other axes into \fn otherAxes(). If the rules couldn't be matched, the constructed instance
	* is invalid.
	*
	* @param selector Rules to match with
	* @param ds Dataset to match to
	**/
NVBSelectorInstance::NVBSelectorInstance(const NVBSelectorCase* selector, const NVBDataSet* ds)
 : valid(false)
 , s(selector)
	{
		if (!ds || !selector) {
			NVBOutputError(ds ? "NULL selector" : "NULL dataset");
			return;
			}
		if (s->axes.count() > ds->nAxes()) {
			NVBOutputDMsg(QString("A dataset with %1 axes will not match a selector with %2").arg(ds->nAxes()).arg(s->axes.count()));
			return;
			}

		dataSet = ds;
		dataSource = ds->dataSource();

		NVBOutputDMsg("Matching dataset " + ds->name());
		
		// Fill in otheraxes and check first axes that use direct indexes
		for(axisindex_t i = 0; i < ds->nAxes(); i += 1)
			otheraxes << i;

		// Don't waste time - empty case matches
		if ( selector->axes.isEmpty() ) {
			NVBOutputDMsg(QString("Empty case %1 matched").arg(selector->id));
			valid = true;
			return;
			}

		axisindex_t i = 0;
		while(i < selector->axes.count()) {
			NVBOutputDMsg(QString("Checking rule %1 for simplicity").arg(i));
			const NVBSelectorAxis a = s->axes.at(i);
			if (!a.p || a.p->axisProperties.isEmpty() || a.p->axisProperties.first().type != NVBSelectorAxis::Index || a.needMore(1)) {
				NVBOutputDMsg(QString("Rule %1 is not simple: %2").arg(i).arg(a.p ? (a.p->axisProperties.isEmpty() ? "Empty rule data" : (a.p->axisProperties.first().type == NVBSelectorAxis::Index ? "More than one axis needed" : "Rule is not an index rule") ) : "NULL rule data"));
				break;
				}
			axisindex_t k = (axisindex_t)a.p->axisProperties.first().i;
			if (a.matches(ds->axisAt(k))) {
				NVBOutputDMsg(QString("Index axis matched at %1").arg(k));
				matchedaxes << k;
				otheraxes.remove(otheraxes.indexOf(k));
				i += 1;
				}
			else {
				NVBOutputDMsg(QString("Index axis didn't match at %1").arg(k));
				return;
				}
			}
		// Matching axes is recursive, so let's use another function
		valid = matchAxes(i,false);
	}

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
NVBSelectorInstance::NVBSelectorInstance(const NVBSelectorCase* selector, const NVBDataSource* ds)
 : valid(false)
 , s(selector)
 , dataSet(0)
	{
		if (!ds || !selector) {
			NVBOutputError(ds ? "NULL selector" : "NULL datasource");
			return;
			}
			
		dataSource = ds;

		// Fill in otheraxes and check first axes that use direct indexes
		for(axisindex_t i = 0; i < ds->nAxes(); i += 1)
			otheraxes << i;

		axisindex_t i = 0;
		while(i < selector->axes.count()) {
			NVBOutputDMsg(QString("Checking rule %1 for simplicity").arg(i));
			const NVBSelectorAxis a = s->axes.at(i);
			if (!a.p || a.p->axisProperties.isEmpty() || a.p->axisProperties.first().type != NVBSelectorAxis::Index || a.needMore(1)) {
				NVBOutputDMsg(QString("Rule %1 is not simple: %2").arg(i).arg(a.p ? (a.p->axisProperties.isEmpty() ? "Empty rule data" : (a.p->axisProperties.first().type == NVBSelectorAxis::Index ? "More than one axis needed" : "Rule is not an index rule") ) : "NULL rule data"));
				break;
				}
			axisindex_t k = (axisindex_t)a.p->axisProperties.first().i;
			if (a.matches(ds->axes().at(k))) {
				NVBOutputDMsg(QString("Index axis matched at %1").arg(k));
				matchedaxes << k;
				otheraxes.remove(otheraxes.indexOf(k));
				i += 1;
				}
			else { // this axis will not match anything else, so we skip it
				NVBOutputDMsg(QString("Index axis didn't match at %1").arg(k));
				matchedaxes << -1;
				i += 1;
				}
			}

		// Matching axes is recursive, so let's use another function
		valid = matchAxes(i,true);

	}

NVBSelectorInstance::NVBSelectorInstance(const NVBSelectorCase* selector, const NVBDataSet* dataset, QVector< axisindex_t > mas, QVector< axisindex_t > oas)
	: valid(true)
	, s(selector)
	, dataSource(dataset->dataSource())
	, dataSet(dataset)
	, matchedaxes(mas)
	, otheraxes(oas)
	{;}

NVBSelectorInstance::NVBSelectorInstance(const NVBSelectorCase* selector)
	: valid(false)
	, s(selector)
	, dataSource(0)
	, dataSet(0)
	{;}

bool NVBSelectorInstance::matchAxes(axisindex_t start, bool skipunmatched)
	{
		if (start == s->axes.count()) {
			NVBOutputDMsg(QString(skipunmatched ? "No more rules" : "All rules matched"));
			return !skipunmatched || otheraxes.count() != dataSource->nAxes();
			}
			
		int matched = 0;

		foreach(axisindex_t i, otheraxes) {
			NVBOutputDMsg(QString("Trying to match axis %1 against rule %2").arg(i).arg(start));
			if (s->axes.at(start).matches(sourceAxis(i), matched ? sourceAxis(matchedaxes.last()) : NVBAxis())) {
				matchedaxes << i;
				otheraxes.remove(otheraxes.indexOf(i));
				matched += 1;
				if (!s->axes.at(start).needMore(matched)) {
					if (matchAxes(start+1,skipunmatched))
						return true;
					else {
						matched -= 1;
						otheraxes.append(i);
						matchedaxes.remove(matchedaxes.count()-1);
						}
					}
				}
			else
				NVBOutputDMsg(QString("Match axis %1 against rule %2 failed").arg(i).arg(start));
			}
			
		NVBOutputDMsg(QString("Rule %1 didn't match any axis").arg(start));
		if (skipunmatched) {
			matchedaxes << -1;
			return matchAxes(start+1,skipunmatched);
			}
		else
			return false;
	}

/**
* 
*
* @param dataset p0:...
* @return NVBSelectorInstance
**/
NVBSelectorInstance NVBSelectorInstance::matchDataset(const NVBDataSet* ds)	{
	if (!isValid() || dataSet || !dataSource) return NVBSelectorInstance();
	if (!dataSource->dataSets().contains(const_cast<NVBDataSet*>(ds))) return NVBSelectorInstance();
	QVector<axisindex_t> subMatched, subOther;
	foreach(axisindex_t i, matchedAxes()) {
		axisindex_t k = ds->asizes.indexOf(i);
		(k >= 0 ? subMatched : subOther) << k;
	}
	return NVBSelectorInstance(s,ds,subMatched,subOther);
}

NVBSelectorInstance NVBSelectorInstance::matchDataset(axisindex_t i) {
	if (!isValid() || dataSet || !dataSource) return NVBSelectorInstance();
	const NVBDataSet * ds = dataSource->dataSets().at(i);
	QVector<axisindex_t> subMatched, subOther;
	foreach(axisindex_t i, matchedAxes()) {
		axisindex_t k = ds->asizes.indexOf(i);
		(k >= 0 ? subMatched : subOther) << k;
	}
	return NVBSelectorInstance(s,ds,subMatched,subOther);
}

NVBAxis NVBSelectorInstance::matchedAxis(axisindex_t i) {
	if (dataSet)
		return dataSet->axisAt(matchedaxes.at(i));
	if (dataSource && matchedAxes().at(i) >= 0)
		return dataSource->axis(matchedaxes.at(i));
	return NVBAxis();
	}

NVBAxis NVBSelectorInstance::otherAxis(axisindex_t i) {
	if (dataSet)
		return dataSet->axisAt(otheraxes.at(i));
	if (dataSource)
		return dataSource->axis(otheraxes.at(i));
	return NVBAxis();
	}

NVBAxis NVBSelectorInstance::sourceAxis(axisindex_t i) {
	if (dataSet)
		return dataSet->axisAt(i);
	if (dataSource)
		return dataSource->axis(i);
	return NVBAxis();
	}






