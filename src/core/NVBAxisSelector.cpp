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

#include "NVBAxisSelector.h"
#include "NVBMap.h"
#include "NVBAxisMaps.h"
#include "NVBDataSource.h"
#include "NVBDataCore.h"

/**
 *
 *  \class NVBAxisSelector
 *
 * AxisSelector is a helper class that allows one to find a set of axes and maps in a given
 * DataSet, DataSource or a list of DataSource's that satisfy a set of requirements.
 * E.g. one can check if the data has enough axes for a 2D FFT transform or
 * select two axes with the same units.
 *
 * The AxisSelector should be initialized by constructing a set of rules, using the add* functions.
 * Then the set is matched against the axes with an instantiate* function. The resulting instance
 * can then be used to get to the necessary axes.
 * 
 * The examples below show the initialization for some use cases.
 *
 * \li Make sure the dataset has at least 3 axes:
 * \code
 * NVBAxisSelector s;
 * s.setDataMinAxes(3);
 * \endcode
 *
 * \li Make sure the dataset has 2 axes with the same units and at least one more
 * \code
 * NVBAxisSelector s;
 * s.setDataMinAxes(3);
 * s.addAxis().needMore(1,NVBSelectorAxis::Units);
 * \endcode
 *
 * \li Make sure the dataset has 2 metric axes
 * \code
 * NVBAxisSelector s;
 * s.addAxisByUnits("m").needMore(1);
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
 * s.addAxisByIndex(0).needMore(1); // No error, second axis skips index rules
 * s.addAxisByIndex(0).needMore(1,NVBSelectorAxis::SameUnits); // The buddy ignores the index rule
 * s.addAxisByIndex(0).needMore(1,NVBSelectorAxis::SameMap);   // The buddy ignores the index rule
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
 * ----------------
 *
 * Example:
 *
 * Datasource with 5 axes, 4 maps, 4 datasets
 *
 * Axes:
 * Time, 100 pts, in seconds
 * Pos, 10 pts, dimentionless
 * Measurement, 5 pts, dimentionless
 * Temperature, 5 pts, in Kelvin
 * Field, 100 pts, in Tesla
 *
 * Maps:
 * NVBPhysPoint on Pos
 * QColor on Pos + Temperature + Field
 * QColor on Pos + Temperature
 * QColor on Pos + Field
 *
 * Datasets:
 * Time x Pos x Measurement x Temperature - Spectroscopy (dI/dU)
 * Time x Pos x Measurement x Field - Spectroscopy (dI/dU)
 * Pos x Temperature x Field - General (Elevation)
 * Time - Spectroscopy (Voltage)
 *
 * c.setDataType(Spectroscopy)
 * c.addAxis().byName(Field)
 * c.addMap().byValueType(NVBAxisMap::Point)
 * c.addMap(Partial).byValueType(NVBAxisMap::Color).addAxis.byName(Temperature)
 *
 * OK, maybe only explicitly mentioned axes should be in the list...
 *
 *
*/

// NVBSelectorCase

NVBSelectorCase & NVBSelectorCase::addCase(int caseId) {
	if (t == NVBSelectorCase::AND) {
		NVBOutputError("Trying to add a Case to an AND-case. Case type changed to OR.");
		dataset.clear();
		}

	t = NVBSelectorCase::OR;

	cases.append( NVBSelectorCase(caseId) );
	return cases.last();
	}

NVBSelectorAxis NVBSelectorCase::addAxis() {
	if (t == NVBSelectorCase::OR) {
		NVBOutputError("Trying to add an Axis to an OR-case. Case type changed to AND.");
		cases.clear();
		}

	t = NVBSelectorCase::AND;

// 	optimal = axes.isEmpty();

	return dataset.addAxis();
	}

NVBSelectorMap NVBSelectorCase::addMap() {
	if (t == NVBSelectorCase::OR) {
		NVBOutputError("Trying to add a Map to an OR-case. Case type changed to AND.");
		cases.clear();
		}

	t = NVBSelectorCase::AND;

// 	optimal = axes.isEmpty();

	return dataset.addMap();
	}


NVBSelectorCase& NVBSelectorCase::setDataType(NVBDataSet::Type type) {
	t = AND;
	dataset.byType(type);
	return *this;
	}

NVBSelectorCase& NVBSelectorCase::setDataName(QString name) {
	t = AND;
	dataset.byName(name);
	return *this;
	}

NVBSelectorCase& NVBSelectorCase::setDataUnits(NVBUnits units) {
	t = AND;
	dataset.byUnits(units);
	return *this;
	}

NVBSelectorCase& NVBSelectorCase::setDataMaxAxes(axisindex_t naxes) {
	t = AND;
	dataset.byMaxAxes(naxes);
	return *this;
	}

NVBSelectorCase& NVBSelectorCase::setDataMinAxes(axisindex_t naxes) {
	t = AND;
	dataset.byMinAxes(naxes);
	return *this;
	}

NVBSelectorCase& NVBSelectorCase::setDataNAxes(axisindex_t naxes) {
	t = AND;
	dataset.byNAxes(naxes);
	return *this;
	}


bool NVBSelectorCase::matches(const NVBDataSet* dataSet) {
	return instantiate(dataSet).isValid();
	}


/**
	* @brief optimizes axis selector rules to put index-limited ones first.
	**/
/*
void NVBSelectorCase::optimize() {
	switch (t) {
		case NVBSelectorCase::AND : {
			if (optimal) return;
			optimal = true;
			for(int i0 = 0, i = 0, in = axes.count()-1; i <= in ; i+=1) {
				NVBSelectorAxis a = axes.at(i);
				if (a.p) {
					if (a.p->rules.isEmpty()) {
						if (axes.at(in).p->rules.isEmpty() && a.p->multType != NVBSelectorAxis::Invalid)
							continue;
						axes.swap(i,in);
						in -= 1;
						i -= 1;
					}
					else if (a.p->rules.first().type == NVBSelectorAxis::Index && !a.needMore(1)) {
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
*/

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

NVBSelectorFileInstance NVBSelectorCase::instantiate(const QList< NVBDataSource* > * dataSources) {
//	if (t != NVBSelectorCase::AND) {
//		NVBOutputError("Only one case per selector is supported when instantiating on list");
//		return NVBSelectorFileInstance(*this);
//		}

// 	optimize();

	return NVBSelectorFileInstance(*this, *dataSources);
	}

NVBSelectorSourceInstance NVBSelectorCase::instantiateOneSource(const QList< NVBDataSource* > * dataSources) {
// 	optimize();
	NVBSelectorFileInstance i = NVBSelectorFileInstance(*this, *dataSources);

	if (i.matchedInstances().isEmpty())
		return NVBSelectorSourceInstance(*this, 0);
	else
		return i.matchedInstances().first();
	}


NVBSelectorDataInstance NVBSelectorCase::instantiateOneDataset(const QList< NVBDataSource* > * dataSources) {
// 	optimize();
	NVBSelectorSourceInstance i = instantiateOneSource(dataSources);

	if (i.matchedInstances().isEmpty())
		return NVBSelectorDataInstance(*this);

	return i.matchedInstances().first();
	}

NVBSelectorSourceInstance NVBSelectorCase::instantiate(const NVBDataSource* dataSource) {
// 	optimize();

	return NVBSelectorSourceInstance(*this, dataSource);
	}

NVBSelectorDataInstance NVBSelectorCase::instantiateOneDataset(const NVBDataSource* dataSource) {
// 	optimize();

	NVBSelectorSourceInstance i = instantiate(dataSource);

	if (i.isValid() && !i.matchedInstances().isEmpty())
		return i.matchedInstances().first();

	return NVBSelectorDataInstance(*this);
	}

NVBSelectorDataInstance NVBSelectorCase::instantiate(const NVBDataSet* data) {
#ifdef NVB_DEBUG_AXISSELECTOR
	NVBOutputDMsg(QString("Trying to match case %1").arg(id));
#endif

	switch (t) {
		case NVBSelectorCase::AND : {
			return NVBSelectorDataInstance(*this, data);
			break;
			}

		case NVBSelectorCase::OR : {
			foreach(NVBSelectorCase c, cases) {
				NVBSelectorDataInstance i = c.instantiate(data);

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
NVBSelectorDataInstance::NVBSelectorDataInstance(const NVBSelectorCase & selector, const NVBDataSet* dataset)
	: valid(false)
	, dataSet(dataset)
	, s(selector) {
	useDataSet(dataset);

	if (dataset)
		initAxes(s.match(dataset));
	}

NVBSelectorDataInstance::NVBSelectorDataInstance(const NVBSelectorCase & selector, const NVBDataSet* dataset, NVBSelectorAxisInstanceList matched)
	: valid(false)
	, dataSet(dataset)
	, s(selector) {
	useDataSet(dataset);

	if (dataset)
		initAxes(matched);
	}

NVBSelectorDataInstance::NVBSelectorDataInstance(const NVBSelectorDataInstance & other)
	: valid(other.valid)
	, dataSet(other.dataSet)
	, s(other.s)
	, matchedaxes(other.matchedaxes)
	, matchedmaps(other.matchedmaps)
	, otheraxes(other.otheraxes) {
	useDataSet(dataSet);
	}

NVBSelectorDataInstance::~NVBSelectorDataInstance() {
	releaseDataSet(dataSet);
	}

const NVBSelectorDataInstance& NVBSelectorDataInstance::operator=(const NVBSelectorDataInstance & other) {
	if (this != &other) {
		releaseDataSet(dataSet); // Just in case we are overwriting
		valid = other.valid;
		dataSet = other.dataSet;
		s = other.s;
		matchedaxes = other.matchedaxes;
		matchedmaps = other.matchedmaps;
		otheraxes = other.otheraxes;

		useDataSet(dataSet);
		}

	return *this;

	}


void NVBSelectorDataInstance::initAxes(NVBSelectorAxisInstanceList matched) {
	if (matched.isEmpty()) return;

	valid = true;

	matchedaxes = matched.first().axes.toVector();
	foreach(NVBAxisMap * map, matched.first().maps) {
		matchedmaps.append(dataSet->mapFromSource(map));
		}

	QList<axisindex_t> mm = matched.first().axes;
	uniquify(mm); // this sorts it

	for(axisindex_t other = 0; other < dataSet->nAxes(); other++)
		if (!mm.contains(other))
			otheraxes.append(other);

	if (matched.first().additionalAxes > 0) {
		// On that level it is unclear which axes can be additional
		// Current code counts all matched axes (on whichever level)
		// to be unavailable for initialization of additional axes.
		//
		// Technically, the additional axes should be the free axes
		// for the same level where the empty rules were found,
		// but we have no info about it.
		if (matched.first().additionalAxes > otheraxes.count())
			valid = false;
		else
			for(int i = 0; i < matched.first().additionalAxes; i++) {
				matchedaxes.append(otheraxes.last());
				otheraxes.pop_back();
				}
		}
	}

int NVBSelectorDataInstance::matchedCase() const {
	return (valid ? s.id : -1);
	}

NVBAxis NVBSelectorDataInstance::matchedAxis(axisindex_t i) const {
	if (dataSet)
		return dataSet->axisAt(matchedaxes.at(i));

	return NVBAxis();
	}

NVBAxis NVBSelectorDataInstance::otherAxis(axisindex_t i) const {
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
	: s(selector) {
	this->source = source;

	if (source) {
		fillInstances(selector, source);
		qSort(allIndexes); // TODO if we don't sort this, the indexes and sets have the same order. do we need it?
		}
	}

void NVBSelectorSourceInstance::fillInstances(const NVBSelectorCase& selector, const NVBDataSource* source) {
	if (!source) return;

	if (selector.t == NVBSelectorCase::OR) {
		foreach(NVBSelectorCase subs, selector.cases)
		fillInstances(subs, source);
		return;
		}

	QList<NVBSelectorDataInstance> listInst;
	QList<NVBDataSet*> listS;
	QList<int> listIx;

// 	NVBSelectorAxisInstanceList axes = selector.match(source);

	for(int ix = 0 ; ix < source->dataSets().count(); ix += 1) {
		NVBSelectorDataInstance i = NVBSelectorDataInstance(selector, source->dataSets().at(ix));

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
		instances.insert(selector.id, listInst);
		allInstances.append(listInst);
		sets.insert(selector.id, listS);
//		allSets.append(listS);
		indexes.insert(selector.id, listIx);
//		allIndexes.append(listIx);
		}

	}

// ------------- NVBSelectorFileInstance

NVBSelectorFileInstance::NVBSelectorFileInstance(const NVBSelectorCase & selector, const QList< NVBDataSource* >& sources) : s(selector) {
	fillInstances(selector, sources);
	fillLists();
	}

void NVBSelectorFileInstance::fillInstances(const NVBSelectorCase & selector, const QList< NVBDataSource* > & sources) {
	if (selector.t == NVBSelectorCase::OR) {
		foreach(NVBSelectorCase subs, selector.cases)
		fillInstances(subs, sources);
		return;
		}

	QList<NVBSelectorSourceInstance> listInst;
	QList<NVBDataSource*> listS;
	QList<int> listIx;

	for(int ix = 0 ; ix < sources.count(); ix += 1) {
		NVBSelectorSourceInstance i = NVBSelectorSourceInstance(selector, sources.at(ix));

		if (i.isValid()) {
			listInst << i;
			listS << sources.at(ix);
			listIx << ix;
			}
		}

	if (!listInst.isEmpty()) {
		instances.insert(selector.id, listInst);
		allInstances.append(listInst);
		this->sources.insert(selector.id, listS);
		allSources.append(listS);
		indexes.insert(selector.id, listIx);
		allIndexes.append(listIx);
		}
	}

void NVBSelectorFileInstance::fillLists() {
	uniquify(allSources);
	uniquify(allIndexes);
	}

const NVBSelectorSourceInstance& NVBSelectorFileInstance::instFromDatasource(const QList< NVBSelectorSourceInstance >& list, const NVBDataSource* source) const {
	static NVBSelectorSourceInstance empty(NVBSelectorCase(), 0);

	for(int j = 0; j < list.count(); j++)
		if (list.at(j).matchingSource() == source)
			return list.at(j);

	empty = NVBSelectorSourceInstance(s, source);
	return empty;
	}
