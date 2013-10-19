//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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
#include "NVBAxisSelectorHelpers.h"

template<typename T>
void uniquify( QList<T> & list ) {
	qSort(list);

	for(int i = 1; i < list.count(); i++) {
		if (list.at(i) == list.at(i - 1))
			list.removeAt(--i);
		}
	}

/*
 *
 * Axis rules:
 * A rule selects a single axis. Multiple axes may be selected
 * with a single rule created by the user, but those are really individual
 * but bound rules. Any property of the axis can be used as a selection criterion.
 * Any property of the selected axis can be used to find bound axes. There are
 * no methods to directly access map parameters.
 *
 * The instance contains a list of matched axes in the order the rules are specified,
 * and a list of matched maps in the same order.
 *
 * An axis rule can contain map rules. The matched map will contain the matched axis.
 * If the rule selects multiple axes, the map has to be shared between them. All the
 * map axes will be excluded from further matching (they will not be automatically
 * included in the list of matched axes).
 *
 * Any extra axis rules inside the map rule will be considered after the main axis rule.
 * They will appear in the matched list. The same is true for any further nesting levels.
 *
 * ----------------
 *
 * Map rules.
 * A rule selects a single map. A map can be limited to the axes of a dataset or to the axes
 * of its parent source. Any property of the map can be used for selection (note that this
 * does not extend to the properties of the mapping). There are not methods to directly access axis
 * parameters.
 *
 * The instance contains a list of matched maps in rule order. In the case that
 * there is more than one way to match maps, the preference is given to the earliest,
 * most tightly in-dataset matching maps. A matched map will have at least 1 axis
 * matched from within the dataset.
 *
 * A map rule can contain axis rules. A map will only match if one of the mapped axes matches
 * the rules. Axes matched this way will appear in the list of matched axes. However,
 * they will not be excluded from the list to match in the possible parent axis rule
 *
 * Nested map rules will appear in the map list in the same order.
 *
 * ----------------
 *
 * Dataset rules:
 * A dataset rule allows one to select a dataset by name, type or units.
 *
 * We can also think about requesting a specific position in the datasource or
 * a minimum number of datasets in the parent datasource.
 *
 * ----------------
 *
 * axis-0 > map-1 > axis-2 > map-3 > axis-4 > map-5
 * axis-A > map-B > axis-C
 *
 * The object in the instance will be grouped in two lists
 * axis-0, axis-2, axis-4, axis-A, axis-C
 * map-1, map-3, map-5
 *
 * Axis-0 may be the same as axis-4 or axis-A
 *
 * matching order
 * rule | matched | left
 *
 * 0 [0] [1,2,3,4,5,6] <- axes in dataset
 * 1 [0] [1,2] <- axes in map-1 & dataset
 * 2 [0,2] [1] <- axes in map-1 & dataset
 * 3 [2] [0,4] <- axes in map-3 & dataset, 0 not excluded
 * 4 [2,4] [0] <- but 0 and 4 may be the same
 *
 * NVBSelectorRules::matches( Dataset, [matched], [free] )
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

// NVBSelectorAxis

struct NVBSelectorRule {
	NVBSelectorRules::NVBSelectorRuleType type;
	// This is not a union, since initialisation of the objects is too cumbersome
	int i;
	NVBUnits u;
	QString n;

	NVBSelectorRule(const NVBSelectorRule & other)
		: type(other.type) {
		switch(type) {
			case NVBSelectorRules::Name :
				n = other.n;
				break;

			case NVBSelectorRules::Units :
				u = other.u;
				break;

			case NVBSelectorRules::Invalid :
				break;

			default :
				i = other.i;
				break;
			}
		}

	NVBSelectorRule & operator=(const NVBSelectorRule & other) {
		if (this != &other) {
			type = other.type;

			switch(type) {
				case NVBSelectorRules::Name :
					n = other.n;
					break;

				case NVBSelectorRules::Units :
					u = other.u;
					break;

				case NVBSelectorRules::Invalid :
					break;

				default :
					i = other.i;
					break;
				}

			return *this;
			}
		}

	NVBSelectorRule(NVBSelectorRules::NVBSelectorRuleType t, int ii)
		: type(t)
		, i(ii)
		{;}
	NVBSelectorRule(QString name)
		: type(NVBSelectorRules::Name)
		, n(name)
		{;}
	NVBSelectorRule(NVBUnits uu)
		: type(NVBSelectorRules::Units)
		, u(uu)
		{;}
	};

struct NVBSelectorRulesPrivate {
	// this object is shared, for easier copying
	int refCount;

	// The type of matching object - axis, map or dataset
	NVBSelectorRules::NVBSelectorRulesType type;

	// Used only by maps, indicates partial/full inclusion of map in the dataset
	NVBSelectorMap::NVBMapCoverage mapCoverage;

	// Used only by axes, indicates number of additional similar axes
	axisindex_t more;

	// Used only by axes, indicates what "similar" axis means
	NVBSelectorRules::NVBAxisBuddy buddyType;

	// Individual matching rules
	QList<NVBSelectorRule> rules;

	// Subrules, resulting in final instance entries
	// e.g. maps and buddy axes for axes or axes for maps
	QList<NVBSelectorRules> subrules;

	NVBSelectorRulesPrivate(NVBSelectorRules::NVBSelectorRulesType t)
		: refCount(0)
		, type(t)
		, more(0)
		, buddyType(NVBSelectorRules::Copy)
		{;}

	NVBSelectorRulesPrivate(NVBSelectorRulesPrivate & other)
		: refCount(0)
		, type(other.type)
		, mapCoverage(other.mapCoverage)
		, more(other.more)
		, buddyType(other.buddyType)
		, rules(other.rules) {
		foreach(NVBSelectorRules r, other.subrules)
		subrules.append(r.deep_copy());
		}

	void clear() {
		if (refCount > 1)
			NVBOutputError("Clearing private part of more that one ruleset");

		more = 0;
		buddyType = NVBSelectorRules::Copy;
		rules.clear();
		subrules.clear();
		}
	};

NVBSelectorRules::NVBSelectorRules(NVBSelectorRules::NVBSelectorRulesType type) {
	p = new NVBSelectorRulesPrivate(type);

	if (p) p->refCount = 1;
	}

NVBSelectorRules::NVBSelectorRules(const NVBSelectorRules& other) {
	if (this != &other) {
		p = other.p;

		if (p) p->refCount += 1;
		}
	}

NVBSelectorRules::~NVBSelectorRules() {
	if (p) {
		p->refCount -= 1;

		if (p->refCount <= 0) delete p;
		}
	}

void NVBSelectorRules::clear() {
	if (p) p->clear();
	}

bool NVBSelectorRules::hasRuleFor(NVBSelectorRules::NVBSelectorRuleType type) {
	return ruleIndexFor(type) >= 0;
	}

int NVBSelectorRules::ruleIndexFor(NVBSelectorRules::NVBSelectorRuleType type) {
	if (!p)
		return -1;

	for(int i = 0; i < p->rules.count(); i++)
		if (p->rules.at(i).type == type)
			return i;

	return -1;
	}

NVBSelectorAxisInstanceList NVBSelectorRules::match(const NVBDataSet* dataset, const NVBSelectorAxisInstance & instance) const {
#ifdef NVB_DEBUG_AXISSELECTOR
	NVBOutputDMsg(QString("Started matching %1").arg(p ? (p->type == Axis ? "axis" : p->type == Map ? "map" : p->type == Dataset ? "dataset" : "alien rule") : "empty rule"));
#endif

	if (!p || (p->type == None && p->subrules.isEmpty()))
		return (NVBSelectorAxisInstanceList () << instance);

	if (!dataset) return NVBSelectorAxisInstanceList();

	NVBSelectorAxisInstanceList results;

	switch (p->type) {
		case Dataset:
			if (matchDataset(dataset)) {
				QList<axisindex_t> l;

				for(axisindex_t i = 0; i < dataset->nAxes(); i++)
					l.append(i);

				results << instance;
				results.last().unmatched = l;
				results.last().parent_axis = -1;
				}

			break;

		case Axis:
			results = matchAxis(dataset, instance);
			break;

		case Map:
			results = matchMap(dataset, instance);
			break;

		default:
			break;
		}

#ifdef NVB_DEBUG_AXISSELECTOR
	NVBOutputDMsg(QString("Rules %1").arg(results.isEmpty() ? "didn't match" : "matched"));
#endif

	if (!results.isEmpty() && !p->subrules.isEmpty()) {

#ifdef NVB_DEBUG_AXISSELECTOR
		NVBOutputDMsg("Matching subrules...");
#endif

		foreach(NVBSelectorRules subrule, p->subrules) {
			NVBSelectorAxisInstanceList rcopy(results);
			results.clear();
			foreach(NVBSelectorAxisInstance i, rcopy) // foreach made a copy of results
			results.append(subrule.match(dataset, i));
			}

#ifdef NVB_DEBUG_AXISSELECTOR
		NVBOutputDMsg(QString("Subrules %1").arg(results.isEmpty() ? "didn't match" : "matched"));
#endif
		}

	// results cleanup

	for(NVBSelectorAxisInstanceList::iterator i = results.begin(); i < results.end(); i++)  {
		if (p->type != Axis)
			(*i).unmatched = instance.unmatched;

//			(*i).unmatched.removeOne((*i).parent_axis);
		(*i).parent_axis = instance.parent_axis;
		}

	return results;
	}

/*
 * The NVBSelectorAxisInstance returned in the list from the match* functions --
 *
 * NVBSelectorAxis::match
 * - Doesn't change parent_axis (never)
 * - Doesn't change maps (unless contains map rules on any level)
 * - Cuts unmatched by 1 (more if contains axis rules directly)
 * - Adds 1 to axes or addA (more if contains axis rules on any level)
 * for 1st level
 * - Sets parent_axis to matched
 * - Doesn't change maps
 * - Cuts unmatched by 1
 * - Adds 1 to axes or addA
 *
 * NVBSelectorMap::match
 * - Doesn't change parent_axis (never)
 * - Adds one to maps (more if contains map rules on any level)
 * - Doesn't change axes or addA (unless contains axis rules on any level)
 * - Doesn't change unmatched (never)
 * for 1st level
 * - Sets parent_axis to (-1)
 * - Adds one to maps
 * - Doesn't change axes or addA
 * - Sets unmatched to its axis indexes without parent_axis
 *
 * NVBSelectorDataset::match
 * - Doesn't change parent_axis (never)
 * - Doesn't change maps (unless contains map rules on any level)
 * - Doesn't change axes or addA (unless contains axis rules on any level)
 * - Doesn't change unmatched (never)
 * for 1st level
 * - Sets parent_axis to (-1)
 * - Doesn't change maps
 * - Doesn't change axes or addA
 * - Sets unmatched to all its axis indexes
 *
 * Conclusion - map match has to overwrite unmatched in all return values
 */

bool NVBSelectorRules::matchDataset(const NVBDataSet* dataset) const {
	if (p->rules.isEmpty()) return true;

	foreach(NVBSelectorRule r, p->rules) {
		switch(r.type) {
			case Invalid:
				break;
				
			case Name:
				if (dataset->name() != r.n) return false;
				break;

			case Units :
				if (!dataset->dimension().isComparableWith(r.u)) return false;
				break;

			case ObjType:
				if (dataset->type() != (NVBDataSet::Type)(r.i)) return false;
				break;

			case Size:
				if (dataset->nAxes() != (axisindex_t)(r.i)) return false;
				break;

			case MinSize:
				if (dataset->nAxes() < (axisindex_t)(r.i)) return false;
				break;

			case MaxSize:
				if (dataset->nAxes() > (axisindex_t)(r.i)) return false;
				break;

			default:
				return false;
			}
		}

	return true;
	}

NVBSelectorAxisInstanceList NVBSelectorRules::matchAxis(const NVBDataSet* dataset, const NVBSelectorAxisInstance& instance) const {
	// Buddy code - a rule has to be inserted
	if ( p->more > 0 &&
	     !(p->rules.isEmpty() && p->buddyType == Copy) &&
	     (p->subrules.isEmpty() || p->subrules.first().type() != Axis)
	   ) {
		p->subrules.prepend(NVBSelectorAxis(p));
		}

	NVBSelectorAxisInstanceList results;

	foreach(axisindex_t testIndex, instance.unmatched) {
#ifdef NVB_DEBUG_AXISSELECTOR
		NVBOutputDMsg(QString("Trying to match axis %1").arg(dataset->axisAt(testIndex).name()));
#endif

		if (!p || (
		      p->rules.isEmpty() && // No rules to match, i.e. matches
		      p->buddyType == Copy ) ) { // Any subaxes are copying, e.g. empty
			/*			( p->buddyType == Copy || // Any subaxes are copying, e.g. empty
							( p->more == 0 && instance.parent_axis == -1) // We are the only axis in the list
							) )*/
			results << NVBSelectorAxisInstance(instance);
			results.last().additionalAxes += 1 + (p ? p->more : 0);
#ifdef NVB_DEBUG_AXISSELECTOR
			NVBOutputDMsg("Empty rule chain");
#endif
			continue;
			}

		bool axisNotMatched = false;

#ifdef NVB_DEBUG_AXISSELECTOR
// 		NVBOutputDMsg(QString("Buddy axis %1").arg(buddy.isValid() ? buddy.name() : "not specified"));
#endif
		foreach(NVBSelectorRule rule, p->rules) {
			switch(rule.type) {
				case Invalid :
					break; // Matches, although this should never happen

				case Index :
					if (instance.parent_axis == -1) { // Skip this rule for buddies
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by index %1").arg(rule.i));
#endif
						axisNotMatched = (rule.i != testIndex);
						}

					break;

				case Name :
					if (instance.parent_axis == -1) { // Skip this rule for buddies
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by name %1").arg(rule.n));
#endif
						axisNotMatched = (dataset->axisAt(testIndex).name() != rule.n);
						}

					break;

				case MinSize :
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by min. length %1").arg(rule.i));
#endif
					axisNotMatched = (dataset->sizeAt(testIndex) < (axissize_t) rule.i);
					break;

				case MaxSize :
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by max. length %1").arg(rule.i));
#endif
					axisNotMatched = (dataset->sizeAt(testIndex) > (axissize_t) rule.i);
					break;

				case Size :
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by length %1").arg(rule.i));
#endif
					axisNotMatched = (dataset->sizeAt(testIndex) != (axissize_t) rule.i);
					break;

				case Units : {
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by units [%1]").arg(rule.u.baseUnit()));
#endif
					NVBAxisPhysMap * m = dataset->axisAt(testIndex).physMap();
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(m ? QString("Axis is in [%1]").arg(m->units().baseUnit()) : QString("No units available"));
#endif
					axisNotMatched = (!m || !m->units().isComparableWith(rule.u));
					break;
					}

				default:
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Invalid rule %1 when matching an axis").arg(rule.type));
#endif
					axisNotMatched = true;
				}
			}

		if (axisNotMatched) continue;

		if (instance.parent_axis > -1) { // Some buddies needed
			if (p->buddyType == Copy) { // The new rules are a copy of this one, i.e. no extra nothing needed
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Buddy is a copy - already matched"));
#endif
				}
			if (p->buddyType & SameLength) {
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by parent length %1").arg(dataset->sizeAt(instance.parent_axis)));
#endif
					axisNotMatched = (dataset->sizeAt(testIndex) != dataset->sizeAt(instance.parent_axis));
				}
			if (!axisNotMatched && (p->buddyType & SameUnits)) {
					NVBAxisPhysMap * mp = dataset->axisAt(instance.parent_axis).physMap();
					NVBAxisPhysMap * mc = dataset->axisAt(testIndex).physMap();
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Trying to match by parent units [%1]").arg(mp ? mp->units().baseUnit() : QString("no units")));
#endif
// 				axisNotMatched = ( mp && !mc ) || (!mp && mc);
				if (mp && mc) // Both maps exist and have units
						axisNotMatched = !mp->units().isComparableWith(mc->units());
				else if (mp || mc) // Only one map exists.
						axisNotMatched = true;
					}
			if (!axisNotMatched && (p->buddyType & SameMap)) {
#ifdef NVB_DEBUG_AXISSELECTOR
					NVBOutputDMsg(QString("Buddy is a map - it's already selected"));
#endif
					// Everything should have been done by the parent
				}
			}

		if (axisNotMatched) continue;

		// Buddy on SameMap needs a different unmatched list
		NVBSelectorAxisInstance intermediate(instance, testIndex);

		if ((p->buddyType & SameMap) && instance.parent_axis == -1) {
			foreach(NVBAxisMapping mp, dataset->axisAt(testIndex).maps()) {
				if (mp.axes.count() >= p->more + 1) {
					NVBSelectorAxisInstance final = intermediate;
					final.unmatched = dataset->mapFromSource<QList>(mp.axes);
					final.unmatched.removeOne(testIndex);
					results << final;
					}
				}
			}
		else
			results << intermediate;

#ifdef NVB_DEBUG_AXISSELECTOR
		NVBOutputDMsg(QString("Axis %1 matched").arg(dataset->axisAt(testIndex).name()));
#endif

		}

	return results;

	}

NVBSelectorAxisInstanceList NVBSelectorRules::matchMap(const NVBDataSet* dataset, const NVBSelectorAxisInstance& instance) const {
	NVBSelectorAxisInstanceList results;

	foreach(axisindex_t testIndex, instance.unmatched) {
#ifdef NVB_DEBUG_AXISSELECTOR
		NVBOutputDMsg(QString("Trying to match map on axis %1").arg(dataset->axisAt(testIndex).name()));
#endif

		if (!p) return NVBSelectorAxisInstanceList(); // No map to match

		bool mapNotMatched = false;

		foreach(NVBAxisMapping mp, dataset->axisAt(testIndex).maps()) {
			foreach(NVBSelectorRule rule, p->rules) {
				switch(rule.type) {
					case Invalid :
						break; // Matches

					case Size :
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by length %1").arg(rule.i));
#endif
						mapNotMatched = (mp.axes.count() != rule.i);
						break;

					case MinSize :
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by min. length %1").arg(rule.i));
#endif
						mapNotMatched = (mp.axes.count() < rule.i);
						break;

					case MaxSize :
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by max. length %1").arg(rule.i));
#endif
						// I still don't understand why QList<>.count() is signed...
						mapNotMatched = ((axissize_t) mp.axes.count() > (axissize_t) rule.i);
						break;

					case TypeID :
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by type (%1)").arg(QMetaType::typeName(rule.i)));
						NVBOutputDMsg(QString("Map values are of type (%1)").arg(QMetaType::typeName(mp.map->valType())));
#endif
						mapNotMatched = (mp.map->valType() != rule.i);

					case ObjType :
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Trying to match by map type %1").arg(rule.i));
#endif
						mapNotMatched = (mp.map->mapType() != (NVBAxisMap::MapType)rule.i);

					default:
#ifdef NVB_DEBUG_AXISSELECTOR
						NVBOutputDMsg(QString("Invalid rule %1 when matching a map").arg(rule.type));
#endif
						mapNotMatched = true;
					};

				if (mapNotMatched) break;
				};

			if (!mapNotMatched) {
				QList<axisindex_t> mapIxs = dataset->mapFromSource<QList>(mp.axes);

				if (p->mapCoverage == NVBSelectorMap::Full) {
					if (mapIxs.indexOf(-1) >= 0)
						continue;
					}
				else
					mapIxs.removeAll(-1);

				if (instance.parent_axis >= 0)
					mapIxs.removeOne(instance.parent_axis);

				results << NVBSelectorAxisInstance(instance, mp.map, mapIxs);
				}
			}
		}
	return results;
	}

NVBSelectorRules::NVBSelectorRulesType NVBSelectorRules::type() {
	if (p) return p->type;

	return None;
	}

NVBSelectorRules::NVBSelectorRules(NVBSelectorRulesPrivate* d)
	: p(d) {
	if (p) p->refCount += 1;
	}

NVBSelectorRules& NVBSelectorRules::operator=(const NVBSelectorRules& other) {
	if (p) {
		p->refCount -= 1;

		if (p->refCount <= 0) delete p;
		}

	p = other.p;

	if (p)
		p->refCount += 1;

	return *this;
	}


void NVBSelectorRules::addRule(NVBUnits units) {
	if (p) {
		int i = ruleIndexFor(NVBSelectorRules::Units);

		if (i < 0)
			p->rules.append(NVBSelectorRule(units));
		else {
			NVBOutputError("Already limited by units. Updating rule.");
			p->rules[i].u = units;
			}
		}
	}

void NVBSelectorRules::addRule(QString name) {
	if (p) {
		int i = ruleIndexFor(NVBSelectorRules::Name);

		if (i < 0)
			p->rules.append(NVBSelectorRule(name));
		else {
			NVBOutputError("Already limited by label. Updating rule.");
			p->rules[i].n = name;
			}
		}
	}

void NVBSelectorRules::addRule(NVBSelectorRules::NVBSelectorRuleType type, int number) {
	if (p) {
		int i = ruleIndexFor(type);

		if (i < 0)
			p->rules.append(NVBSelectorRule(type, number));
		else {
			NVBOutputError(QString("Already limited by type %1. Updating rule.").arg(type));
			p->rules[i].i = number;
			}
		}
	}


NVBSelectorRules NVBSelectorRules::deep_copy() {
	if (!p) return NVBSelectorRules((NVBSelectorRulesPrivate*)0);

	NVBSelectorRulesPrivate * d = new NVBSelectorRulesPrivate(*p);
	return NVBSelectorRules(d);
	}

NVBSelectorAxis NVBSelectorDataset::addAxis() {
	if (p) {
		NVBSelectorAxis a;
		p->subrules.append(a);
		return a;
		}
	else {
		NVBOutputError("Private pointer is ZERO");
		return NVBSelectorAxis();
		}
	}

NVBSelectorMap NVBSelectorDataset::addMap() {
	if (p) {
		NVBSelectorMap m;
		p->subrules.append(m);
		return m;
		}
	else {
		NVBOutputError("Private pointer is ZERO");
		return NVBSelectorMap();
		}
	}


NVBSelectorMap::NVBSelectorMap(NVBSelectorMap::NVBMapCoverage c)
	: NVBSelectorRules(NVBSelectorRules::Map) {
	p->mapCoverage = c;
	}

NVBSelectorMap& NVBSelectorMap::byValueType(NVBAxisMap::ValueType t) {
	switch(t) {
		case NVBAxisMap::Physical :
			return byType<NVBUnits>();

		case NVBAxisMap::Point :
			return byType<NVBPhysPoint>();

		case NVBAxisMap::Template : // No sense
			return *this;

		case NVBAxisMap::Color :
			return byType<QColor>();

		case NVBAxisMap::Label :
			return byType<QString>();

		default:
			return *this;
		}
	}

NVBSelectorAxis NVBSelectorMap::addAxis() {
	if (p) {
		NVBSelectorAxis a;
		p->subrules.append(a);
		return a;
		}

	NVBOutputError("No data pointer. No place to add axis");
	return NVBSelectorAxis();
	}


NVBSelectorAxis::NVBSelectorAxis(NVBSelectorRulesPrivate* d)
	: NVBSelectorRules(Axis) {
	if (d->type != NVBSelectorRules::Axis || d->more == 0) return;

	p->more = d->more - 1;
	p->buddyType = d->buddyType;
	p->rules = d->rules;
	p->subrules = d->subrules;

	}


NVBSelectorAxis& NVBSelectorAxis::byIndex(int index) {
	if (p) {
		if (p->rules.isEmpty() || p->rules.first().type != NVBSelectorRules::Index)
			p->rules.prepend(NVBSelectorRule(NVBSelectorRules::Index, index));
		else
			p->rules.first().i = index;
		}

	return *this;
	}

NVBSelectorMap NVBSelectorAxis::addMap(NVBSelectorMap::NVBMapCoverage coverage) {
	if (p) {
		NVBSelectorMap m(coverage);
		p->subrules.append(m);
		return m;
		}

	NVBOutputError("No data pointer. No place to add map");
	return NVBSelectorMap(coverage);
	}

NVBSelectorAxis& NVBSelectorAxis::needMore(int more_axes, NVBSelectorAxis::NVBAxisBuddy t) {
	if (p) {
		if (p->more != 0)
			NVBOutputError("This rule was already marked for multiple axes. Parameters will be overwritten.");

		p->more = more_axes;
		p->buddyType = t;
		}
	else
		NVBOutputError("No data pointer. Cannot save rules");

	return *this;
	}

#ifdef NVB_DEBUG
QDebug operator<<(QDebug dbg, const NVBSelectorAxisInstance &i) {
	dbg.nospace() << "( " << i.additionalAxes << "+" << i.axes << "/" << i.unmatched << "*" << i.maps.size() << "p" << i.parent_axis << ")";

	return dbg.space();
	}
#endif
