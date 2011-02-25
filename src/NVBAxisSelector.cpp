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

// NVBSelectorAxis

struct NVBAxisProperty {
	enum NVBAxisPropertyType {
		Invalid = 0,
		Index,
		MinLength,
		Units,
		MapDimensions,
		TypeID
	};

	NVBAxisPropertyType type;
	int i;
	NVBUnits u;

	NVBAxisProperty(NVBAxisPropertyType t, int ii)
		: type(t)
		, i(ii)
		{;}
	NVBAxisProperty(NVBAxisPropertyType t, NVBUnits uu)
		: type(t)
		, u(uu)
		{;}
	};

struct NVBSelectorAxisPrivate {
	int refCount;
	int mult;
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
		if (p) p->axisProperties.prepend(NVBAxisProperty(NVBAxisProperty::Index, index));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byMapDimensions(int dimension)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBAxisProperty::MapDimensions, dimension));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byMinLength(int length)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBAxisProperty::MinLength, length));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byTypeId(int typeId)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBAxisProperty::TypeID, typeId));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::byUnits(NVBUnits dimension)
	{
		if (p) p->axisProperties.append(NVBAxisProperty(NVBAxisProperty::Units, dimension));
		return *this;
	}

NVBSelectorAxis& NVBSelectorAxis::need(int more_axes)
	{
		if (p) p->mult = more_axes;
		return *this;
	}

bool NVBSelectorAxis::matches(const NVBAxis & axis, bool buddy) const
	{
		NVBOutputDMsg(QString("Trying to match axis \"%1\"").arg(axis.name()));
		if (!p) return true;
		bool b; // Cannot define b within switch-case
		foreach(NVBAxisProperty ap, p->axisProperties) {
			switch(ap.type) {
				case NVBAxisProperty::Index :
					NVBOutputDMsg(QString("Trying to match by index %1").arg(ap.i));
					if (!buddy && axis.dataSource()->axis(ap.i) != axis) return false;
					break;
				case NVBAxisProperty::MinLength :
					NVBOutputDMsg(QString("Trying to match by min. length %1").arg(ap.i));
					if (axis.length() < (axissize_t) ap.i) return false;
					break;
				case NVBAxisProperty::MapDimensions :
					NVBOutputDMsg(QString("Trying to match by %1D map").arg(ap.i));
					b = false;
					foreach(NVBAxisMapping m, axis.maps())
						 b = b || (m.map->dimension() == ap.i);
					if (!b) return false;
					break;
				case NVBAxisProperty::TypeID :
					NVBOutputDMsg(QString("Trying to match by type %1").arg(QMetaType::typeName(ap.i)));
					b = false;
					foreach(NVBAxisMapping m, axis.maps())
						 b = b || (m.map->valType() == ap.i);
					if (!b) return false;
					break;
				case NVBAxisProperty::Units :
					NVBOutputDMsg(QString("Trying to match by units %1").arg(ap.u.baseUnit()));
					NVBOutputDMsg(axis.physMap() ? QString("Axis is in %1").arg(axis.physMap()->units().baseUnit()) : QString("No units available"));
					if (!axis.physMap() || !axis.physMap()->units().isComparableWith(ap.u)) return false;
					break;
				default:
					return false;
				};
			};
		return true;
	}

// NVBSelectorCase


NVBSelectorCase::NVBSelectorCase(const NVBSelectorCase& other) {
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
					else if (axes.at(i).p->axisProperties.first().type == NVBAxisProperty::Index && !axes.at(i).needMore(1)) {
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

QColor NVBSelectorInstance::associatedColor(const NVBDataSlice& slice) {
	Q_UNUSED(slice)
/* test code layout for colors

	getMapsByType<QColor>(sliceAxes,::Inside)

	if 0 :
		getMapsByType<QColor>(sliceAxes,::Outside)
	if 1:
		mapValueAt(sliceindex)
	if >1:
		select one that has less axes outside / spans more axes /is the first one

*/

	return Qt::black;
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
			if (!a.p || a.p->axisProperties.isEmpty() || a.p->axisProperties.first().type != NVBAxisProperty::Index || a.needMore(1)) {
				NVBOutputDMsg(QString("Rule %1 is not simple: %2").arg(i).arg(a.p ? (a.p->axisProperties.isEmpty() ? "Empty rule data" : (a.p->axisProperties.first().type == NVBAxisProperty::Index ? "More than one axis needed" : "Rule is not an index rule") ) : "NULL rule data"));
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
		valid = matchAxes(i);
	}

#if 0
/**
* When NVBSelectorInstance is constructed on a datasource, it is looking for a set of axes of this dataSource,
* matching the supplied rules. The axes defined by the rules are aggregated into \fn matchedAxes(),
* and the other axes into \fn otherAxes(). If the rules couldn't be matched, the constructed instance
* is invalid. To convert this instance into a dataset-based one, use \fn matchDataset()
*
* @param selector The NVBSelectorCase to be matched
* @param ds DataSource to match \a selector against
**/
NVBSelectorInstance::NVBSelectorInstance(const NVBSelectorCase* selector, const NVBDataSource* ds)
 : valid(false)
 , s(selector)
	{
		if (!ds || !selector) {
			NVBOutputError(ds ? "NULL selector" : "NULL datasource");
			return;
			}
		if (s->axes.count() > ds->nAxes()) {
			NVBOutputDMsg(QString("A datasource with %1 axes will not match a selector with %2").arg(ds->nAxes()).arg(s->axes.count()));
			return;
			}
			
		dataSource = ds;

		// Fill in otheraxes and check first axes that use direct indexes
		for(axisindex_t i = 0; i < ds->nAxes(); i += 1)
			otherAxes() << i;

		// Matching axes is recursive, so let's use another function
		valid = matchAxes(0);
	}
#endif

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
	{;}

bool NVBSelectorInstance::matchAxes(axisindex_t start)
	{
		if (start == s->axes.count()) {
			NVBOutputDMsg(QString("All rules matched"));
			return true;
			}
		int matched = 0;

		foreach(axisindex_t i, otheraxes) {
			NVBOutputDMsg(QString("Trying to match axis %1 against rule %2").arg(i).arg(start));
			if (s->axes.at(start).matches(dataSet->axisAt(i), matched)) {
				matchedaxes << i;
				otheraxes.remove(otheraxes.indexOf(i));
				matched += 1;
				if (!s->axes.at(start).needMore(matched) && matchAxes(start+1)) return true;
				matched -= 1;
				otheraxes.append(i);
				matchedaxes.remove(matchedaxes.count()-1);
				}
			else
				NVBOutputDMsg(QString("Match axis %1 against rule %2 failed").arg(i).arg(start));
			}
			
		NVBOutputDMsg(QString("Rule %1 didn't match any axis").arg(start));
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

const NVBAxis& NVBSelectorInstance::matchedAxis(axisindex_t i) {
	// TODO find out which one is correct
//	if (dataSource) return dataSource->axis(matchedaxes.at(i));
	return dataSet->axisAt(matchedaxes.at(i));
	}

const NVBAxis& NVBSelectorInstance::otherAxis(axisindex_t i) {
	// TODO find out which one is correct
//	if (dataSource) return dataSource->axis(otheraxes.at(i));
	return dataSet->axisAt(otheraxes.at(i));
	}






