//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QLayoutItem>
#include <QtGui/QToolBar>
#include <QtGui/QActionGroup>
#include <QtGui/QAction>

#include "NVBSingleView.h"

#include "NVBLogger.h"
#include "NVBAxisSelector.h"
#include "NVBPosLabel.h"
#include "NVBDataCore.h"
#include "NVBSingle2DView.h"
#include "NVBSingleGraphView.h"

#include "../icons/singleview.xpm"

template <class T, class S>
QVector<T> mappedVector(const QVector<T> & values, const QVector<S> & indexes) {
	QVector<S> r;
	foreach(int i, indexes)
	r << values.at(i);
	return r;
	}

class NVBSingleViewSlices;

/*
NVBOverridenDataSet::NVBOverridenDataSet(const NVBDataSet* parent)
: NVBDataSet(parent->dataSource(),parent->name(),const_cast<double*>(parent->data()),parent->dimension(),parent->parentIndexes())
, orig(parent)
, axmap(parent->parentIndexes())
{
	// Const-casting the data is not the best solution, but we know that the base class will never change the data by itself.
}


NVBOverridenDataSet::NVBOverridenDataSet(const NVBDataSet* parent, double * data, QVector<axisindex_t> axes)
: NVBDataSet(parent->dataSource(),parent->name(),data,parent->dimension(),mappedVector<axisindex_t,axisindex_t>(parent->parentIndexes(),axes))
, orig(parent)
, axmap(axes)
{

}

NVBOverridenDataSet::~NVBOverridenDataSet()
{

}

void NVBOverridenDataSet::setData(double* data, QVector< axisindex_t > axes)
{
	if ( d && d != orig->data() && d != data ) free (d);
	d = data;
	axmap = mappedVector<axisindex_t,axisindex_t>(orig->parentIndexes(),axes);
	emit dataChanged();
}
*/
NVBAverageSlicingDataSet::NVBAverageSlicingDataSet(const NVBDataSet* parent)
	: NVBDataSet(parent->dataSource(), parent->name(), 0, parent->dimension(), parent->parentIndexes())
	, orig(parent)
	, avdata(0)
	, avas(parent->parentIndexes())
	, avsizes(parent->sizes()) {
	useDataSet(orig);
	}

NVBAverageSlicingDataSet::~NVBAverageSlicingDataSet() {
	if (avdata) free(avdata);

	releaseDataSet(orig);
	}

const double * NVBAverageSlicingDataSet::data() const {
	if (d)
		return NVBDataSet::data();
	else if (avdata)
		return avdata;
	else
		return orig->data();
	}

void NVBAverageSlicingDataSet::setAveragingAxes(QVector< axisindex_t > axes) {
	if (d) {
		free(d);
		d = 0;
		}

	if (avdata) {
		free(avdata);
		avdata = 0;
		}

	avas = orig->parentIndexes();
	avsizes = orig->sizes();

	avaxes.clear();
	slaxes.clear();

	asizes.clear(); // clear the cache of axis sizes

	if (!axes.isEmpty()) {
		avaxes = axes;
		avdata = averageDataSet(orig, avaxes);

		for(int i = avaxes.count() - 1; i >= 0; i--) { // Inverse order to make it easier
			avas.remove(avaxes.at(i));
			avsizes.remove(avaxes.at(i));
			}
		}

	as = avas;

	emit dataReformed();
	emit dataChanged();
	}

void NVBAverageSlicingDataSet::setSliceAxes(QVector< axisindex_t > axes) {
	QVector<axisindex_t> newslaxes = axes;

	for(int i = 0, j = 0; i < newslaxes.size(); i++) {
		while( j < avaxes.size() && avaxes.at(j) < newslaxes.at(i) )
			j += 1;

		newslaxes[i] -= j;
		}

	setSliceAxes2(newslaxes);
	}

void NVBAverageSlicingDataSet::setSliceAxes2(QVector< axisindex_t > axes) {
	if (d) {
		free(d);
		d = 0;
		}

	slaxes = axes;
	asizes.clear(); // clear the cache of axis sizes

	as = avas;

	for(int i = slaxes.count() - 1; i >= 0; i--) // Inverse order to make it easier
		as.remove(slaxes.at(i));

	QVector<axissize_t> slixes;
	slixes.fill(0, slaxes.size());

	emit dataReformed();

	setSliceIndexes(slixes);

	// FIXME recalculate index map
	}

void NVBAverageSlicingDataSet::setSliceIndexes(QVector< axissize_t > indexes) {

	if (slaxes.count()) {
		if (d)
			sliceNArray(avdata ? avdata : orig->data(), d, avsizes, slaxes, indexes);
		else
			d = sliceNArray(avdata ? avdata : orig->data(), avsizes, slaxes, indexes);
		}

	zmin = 1;
	zmax = -1;

	emit dataChanged();
	}

void NVBAverageSlicingDataSet::reset() {
	setAveragingAxes(QVector< axisindex_t >());
	}


// ---------------------

NVBSingleViewSliders::NVBSingleViewSliders(const NVBDataSet* ds, NVBSingleView::Type type, QWidget* parent)
	: QWidget(parent)
	, viewType(type) {
	axisOps.resize(ds->nAxes());
	axisSlices.resize(ds->nAxes());

	QGridLayout * l = new QGridLayout(this);
	setLayout(l);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(&cbs, SIGNAL(mapped(int)), this, SLOT(axisOpChanged(int)));
	connect(&slds, SIGNAL(mapped(int)), this, SLOT(slicePosChanged(int)));

	//	Initialize cases
	switch(type) {
		case NVBSingleView::Graph :
			nPlotAxes = 1;
			axes << 0;
			axisOps[0] = asX;
			break;

		case NVBSingleView::Image :
		case NVBSingleView::Landscape : {
			nPlotAxes = 2;
			NVBAxisSelector s;
			s.addAxisByName("X");
			s.addAxisByName("Y");
			NVBSelectorDataInstance i = s.instantiate(ds);

			if (i.isValid())
				axes = i.matchedAxes();
			else
				axes << 0 << 1;

			axisOps[axes[0]] = asX;
			axisOps[axes[1]] = asY;
			break;
			}

		default:
			nPlotAxes = 0;
			break;
		}

	for(axisindex_t i = 0; i < ds->nAxes(); i++)
		if (!axes.contains(i)) {
			av_axes << i;
			axisOps[i] = (type == NVBSingleView::Graph) ? Keep : Average;
			}

	QStringList contents;
	contents << "Use as X";

	if (nPlotAxes > 1)
		contents << "Use as Y";

	if (ds->nAxes() > nPlotAxes)
		contents << "Average" << "Slice";

	if (type == NVBSingleView::Graph)
		contents << "Keep";

	for(axisindex_t i = 0; i < ds->nAxes(); i++) {

		QLabel * lbl = new QLabel(ds->axisAt(i).name(), this);
		l->addWidget(lbl, i + 1, 0);

		QComboBox * cb = new QComboBox(this);
		cb->addItems(contents);
		l->addWidget(cb, i + 1, 1);
		cbs.setMapping(cb, i);
		connect(cb, SIGNAL(currentIndexChanged(int)), &cbs, SLOT(map()));

		QSlider * sld = new QSlider(Qt::Horizontal, this);
		sld->setRange(0, ds->axisAt(i).length() - 1);
		sld->setTickPosition(QSlider::TicksBothSides);
		sld->setEnabled(false);
		l->addWidget(sld, i + 1, 2);
		axisSlices[i] = 0;
		slds.setMapping(sld, i);
		connect(sld, SIGNAL(valueChanged(int)), &slds, SLOT(map()));

		NVBPosLabel * pl = new NVBPosLabel(sld, ds->axisAt(i).physMap(), this);
		pl->setEnabled(false);
		pl->setText(QString());
		l->addWidget(pl, i + 1, 3);
		}

	updateCache();
	updateWidgets();
	}


NVBSingleViewSliders::~NVBSingleViewSliders() {

	}


void NVBSingleViewSliders::axisOpChanged(int index) {
	QComboBox * cb = qobject_cast<QComboBox*>(cbs.mapping(index));
	NVB_ASSERT(cb, "No ComboBox mapped");
	int ci = cb->currentIndex();
	AxisOp nopt = ci < nPlotAxes ? (AxisOp)ci : (AxisOp)(ci + 2 - nPlotAxes);
	AxisOp aopt = axisOps.at(index);

	if (aopt == nopt) return;

	switch (aopt) {
		case asX:
		case asY:
			switch (nopt) {
				case asX:  //Swap X and Y
				case asY: {
					if (nPlotAxes == 1) {
						NVBOutputError("Invalid way to treat an axis");
						return;
						}

					int ix = axisOps.indexOf(nopt);
					axisOps[ix] = aopt;
					axisOps[index] = nopt;
					break;
					}

//					break;
				case Average:
				case Keep:
				case Slice: {
					if (axisOps.count() < 3) return;

					int i;

					// Find an axis that is neither X nor Y - there's always at least 1
					for(i = 0; axisOps.at(i) == asX || axisOps.at(i) == asY; i++) {;}

					axisOps[i] = aopt;
					axisOps[index] = nopt;
					break;
					}

				default:
					NVBOutputError("Invalid way to treat an axis");
				}

			break;

		case Average:
		case Keep:
		case Slice:
			switch (nopt) {
				case asX:
				case asY: { //Swap with current
					int ix = axisOps.indexOf(nopt);
					axisOps[ix] = aopt;
					axisOps[index] = nopt;
					break;
					}

				case Average:
				case Keep:
				case Slice:
					qobject_cast< QAbstractSlider* >(slds.mapping(index))->setValue(0);
					axisSlices[index] = 0;
					axisOps[index] = nopt;
					break;

				default:
					NVBOutputError("Invalid way to treat an axis");
				}

			break;

		default:
			NVBOutputError("Invalid way to treat an axis");
		}

	emit axisTargetsChanged(axisOps);

	updateCache();

	updateWidgets();
	}

void NVBSingleViewSliders::slicePosChanged(int pos) {
	axisSlices[pos] = qobject_cast< QAbstractSlider* >(slds.mapping(pos))->value();
	axisindex_t numAv = 0;

	for(int i = 0; i < pos; i++)
		if (axisOps.at(i) != Slice)
			numAv += 1;

	sl_ixes[pos - numAv] = axisSlices[pos];

	emit sliceIndexesChanged(sl_ixes);
	}

void NVBSingleViewSliders::updateCache() {
//	axisindex_t numAv = 0;
//	bool avAxesChanged = false;

	sl_axes.clear();
	sl_ixes.clear();
	av_axes.clear();

	for(int i = 0; i < axisOps.count(); i++)
		switch(axisOps.at(i)) {
			case asX:
				axes[0] = i;
				break;

			case asY:
				axes[1] = i;
				break;

			case Average:
				av_axes << i;
				break;

			case Slice:
				sl_axes << i;
				sl_ixes << axisSlices.at(i);
				break;

			case Keep:
				break;
			}

	emit axisTargetsChanged(axes.first(), axes.last(), av_axes, sl_axes);
	emit sliceIndexesChanged(sl_ixes);
	}

void NVBSingleViewSliders::updateWidgets() {
	QGridLayout * l = qobject_cast< QGridLayout* >(layout());

	for(int i = 0; i < axisOps.count(); i++) {
		QComboBox * cb  = qobject_cast<QComboBox*>(l->itemAtPosition(i + 1, 1)->widget()); // Combobox

		if (cb)
			cb->setCurrentIndex((int)axisOps.at(i) + (axisOps.at(i) < nPlotAxes ? 0 : nPlotAxes - 2 ));
		else
			NVBOutputError(QString("The item at (%1,1) is not a combobox").arg(i + 1));

		l->itemAtPosition(i + 1, 2)->widget()->setEnabled(axisOps.at(i) == Slice); // Slider
		l->itemAtPosition(i + 1, 3)->widget()->setEnabled(axisOps.at(i) == Slice); // Label
		}
	}

// ------------------



NVBSingleView::NVBSingleView(const NVBDataSet* dataSet, QWidget* parent)
	: QFrame(parent)
	, ds(0)
	, ods(0)
	, view2D(0)
	, viewGraph(0) {
	QVBoxLayout * vl = new QVBoxLayout(this);
	setLayout(vl);
	QHBoxLayout * menuLayout = new QHBoxLayout();
	vl->addLayout(menuLayout, 1);
	menuLayout->addStretch();
	viewTB = new QToolBar(this);
	QAction *tAct;
	QActionGroup * viewActions = new QActionGroup(this);
	viewActions->setExclusive(true);

	tAct = viewTB->addAction(QIcon(_sview_1D), "Graph", this, SLOT(showGraphView()));
	tAct->setCheckable(true);
	viewActions->addAction(tAct);
	tAct = viewTB->addAction(QIcon(_sview_2D), "2D view", this, SLOT(show2DView()));
	tAct->setCheckable(true);
	viewActions->addAction(tAct);
	tAct = viewTB->addAction(QIcon(_sview_3D), "3D view", this, SLOT(show3DView()));
	tAct->setCheckable(true);
	viewActions->addAction(tAct);
	tAct->setEnabled(false); // TODO - implement NVBSingle3DView
	menuLayout->addWidget(viewTB);

	vl->addStretch();

	viewTB->setEnabled(false);

	setDataSet(dataSet);

	}

void NVBSingleView::removeView()	{
	while (layout()->count() > 1) {
		QLayoutItem * tmp;
		tmp = layout()->takeAt(1);

		if (tmp) {
			if (tmp->widget()) delete tmp->widget();

			delete tmp;
			}
		}

	view2D = 0;
	viewGraph = 0;
	}

void NVBSingleView::createView(NVBSingleView::Type type) {
	removeView();

	viewTB->setEnabled(false);
	viewTB->actions().at(0)->setChecked(true);
	viewTB->actions().at(0)->setChecked(false);

	if (!ods) return;

	ods->reset();

	QVBoxLayout * vl = qobject_cast<QVBoxLayout*>(layout());

	if (!vl) {
		NVBOutputError("Layout type conversion failed");
		return;
		}

	NVBSingleViewSliders * sliders = 0;

	switch(type) {
		case NVBSingleView::Image :
			if (ds->nAxes() > 1) {
				vl->insertWidget(1, view2D = new NVBSingle2DView(ods, this), 1);
				vl->insertWidget(2, sliders = new NVBSingleViewSliders(ds, Image), 1);
				viewTB->actions().at(1)->setChecked(true);
				break;
				}

		case NVBSingleView::Graph :
			vl->insertWidget(1, viewGraph = new NVBSingleGraphView(ods, this), 1);
			vl->insertWidget(2, sliders = new NVBSingleViewSliders(ds, Graph), 1);
			viewTB->actions().at(0)->setChecked(true);
			break;

		case NVBSingleView::Landscape :
			break;
		}

	if (sliders) {
		connect(sliders, SIGNAL(axisTargetsChanged(axisindex_t, axisindex_t, QVector<axisindex_t>, QVector<axisindex_t>)), this, SLOT(targetsChanged(axisindex_t, axisindex_t, QVector<axisindex_t>, QVector<axisindex_t>)));
		connect(sliders, SIGNAL(sliceIndexesChanged(QVector<axissize_t>)), this, SLOT(sliceChanged(QVector<axissize_t>)));

		// Unfortunately, the first signal was emitted before we could connect

		targetsChanged(sliders->xAxis(), sliders->yAxis(), sliders->averagedAxes(), sliders->slicedAxes());
		sliceChanged(sliders->sliceIndexes());
		}

	viewTB->setEnabled(true);
	}


NVBSingleView::~NVBSingleView() {
	setDataSet(0);

	if (ods)
		delete ods;
	}

void NVBSingleView::hideEvent(QHideEvent* e) {
	QWidget::hideEvent(e);

	emit dismissed();
	}


void NVBSingleView::sliceChanged(QVector< axissize_t > indexes) {
	if (ods)
		ods->setSliceIndexes(indexes);
	}

void NVBSingleView::targetsChanged(axisindex_t x, axisindex_t y, QVector< axisindex_t > a, QVector< axisindex_t > s) {
	if (!ods) return;

	ods->setAveragingAxes(a);
	ods->setSliceAxes(s);

	axisindex_t dx = 0, dy = 0;

	foreach(axisindex_t i, a) {
		if (i < x) dx += 1;

		if (i < y) dy += 1;
		}
	foreach(axisindex_t i, s) {
		if (i < x) dx += 1;

		if (i < y) dy += 1;
		}

	x -= dx;
	y -= dy;

	// the ods now as only the necessary axes
	if (view2D) {
//		view2D->setDataSet(ods);
		view2D->setXYAxes(x, y);
		}
	else if (viewGraph) {
//		viewGraph->setDataSet(ods);
		viewGraph->setXAxis(x);
		}
	}

void NVBSingleView::setDataSet(const NVBDataSet* dataSet) {
	if (ds) {
		releaseDataSet(ds);
		ds = 0;
		}

	removeView();

	if (ods) {
		// ods is an orphan, so we can't releaseDataSet(ods)
		// as this will have no result. ods is definitely deleted
		// here, and this is done after the views are deleted
		delete ods;
		ods = 0;
		}

	if (dataSet) {
		ds = dataSet;
		useDataSet(ds);
		ods = new NVBAverageSlicingDataSet(ds);

		switch (ds->type()) {
			case NVBDataSet::Topography:
			case NVBDataSet::Undefined:
				createView(NVBSingleView::Image);
				break;

			case NVBDataSet::Spectroscopy:
			default:
				createView(NVBSingleView::Graph);
				break;
			}
		}
	}
