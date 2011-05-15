/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "NVBSingleView.h"
#include <QtGui/QGridLayout>
#include "NVBSingle2DView.h"
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include "NVBPosLabel.h"
#include "NVBLogger.h"
#include <QtGui/qlayoutitem.h>
#include "NVBDataCore.h"

template <class T, class S>
QVector<T> mappedVector(const QVector<T> & values, const QVector<S> & indexes) {
	QVector<S> r;
	foreach(int i, indexes)
		r << values.at(i);
	return r;
}

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

NVBAveragingDataSet::NVBAveragingDataSet(const NVBDataSet* parent)
: NVBDataSet(parent->dataSource(),parent->name(),0,parent->dimension(),parent->parentIndexes())
, orig(parent)
, axmap(parent->parentIndexes())
{

}

NVBAveragingDataSet::~NVBAveragingDataSet()
{
}

const double * NVBAveragingDataSet::data() const {
	if (d)
		return NVBDataSet::data();
	else
		return orig->data();
}

void NVBAveragingDataSet::setAveragingAxes(QVector< axisindex_t > axes)
{
	if (d) free(d);
	as = orig->parentIndexes();
	
	if (!axes.isEmpty()) {
		d = averageDataSet(orig,axes);
		for(int i = axes.count()-1; i>=0; i--) // Inverse order to make it easier
			as.remove(axes.at(i));
		}
	else
		d = 0;

	asizes.clear();
	
	emit dataChanged();
}



// ---------------------

const QStringList axisOpStrings = QStringList() << "Use as X" << "Use as Y" << "Average" << "Slice";
const QStringList axisOpStringsShort = QStringList() << "Use as X" << "Use as Y";

NVBSingleView::NVBSingleView(const NVBDataSet* dataSet, QWidget* parent)
: QWidget(parent)
, ds(dataSet)
, ods(new NVBAveragingDataSet(ds))
{
	if (!dataSet) {
		NVBOutputError("Null dataset supplied");
		return;
		}

	axisOps.resize(ds->nAxes());
	axisSlices.resize(ds->nAxes());

	QGridLayout * l = new QGridLayout(this);
	setLayout(l);
	
	l->addWidget(view = new NVBSingle2DView(ods,this),0,0,1,4);
	
	connect(&cbs,SIGNAL(mapped(int)),this,SLOT(axisOpChanged(int)));
	connect(&slds,SIGNAL(mapped(int)),this,SLOT(slicePosChanged(int)));
	
	axes << 0 << 1;
	
	for(axisindex_t i = 2; i < ds->nAxes(); i++)
		av_axes << i;
	
	for(axisindex_t i = 0; i < ds->nAxes(); i++) {

		QLabel * lbl = new QLabel(ds->axisAt(i).name(),this);
		l->addWidget(lbl,i+1,0);

		QComboBox * cb = new QComboBox(this);
		if (ds->nAxes() == 2)
			cb->addItems(axisOpStringsShort);
		else
			cb->addItems(axisOpStrings);
		axisOps[i] = qMin((AxisOp)i,Average);
		cb->setCurrentIndex(axisOps.at(i));
		l->addWidget(cb,i+1,1);
		cbs.setMapping(cb,i);
		connect(cb,SIGNAL(currentIndexChanged(int)),&cbs,SLOT(map()));

		QSlider * sld = new QSlider(Qt::Horizontal,this);
		sld->setRange(0,ds->axisAt(i).length()-1);
		sld->setTickPosition(QSlider::TicksBothSides);
		sld->setEnabled(false);
		l->addWidget(sld,i+1,2);
		axisSlices[i] = 0;
		slds.setMapping(sld,i);
		connect(sld,SIGNAL(valueChanged(int)),&slds,SLOT(map()));

		NVBPosLabel * pl = new NVBPosLabel(sld,ds->axisAt(i).physMap(),this);
		pl->setEnabled(false);
		pl->setText(QString());
		l->addWidget(pl,i+1,3);
	}
	
	updateAverage();
}


NVBSingleView::~NVBSingleView()
{

}

void NVBSingleView::axisOpChanged(int index)
{
	QComboBox * cb = qobject_cast<QComboBox*>(cbs.mapping(index));
	NVB_ASSERT(cb,"No ComboBox mapped");
	AxisOp nopt = (AxisOp)cb->currentIndex();
	AxisOp aopt = axisOps.at(index);
	
	if (aopt == nopt) return;
	
	switch (aopt) {
		case asX:
		case asY:
			switch (nopt) {
				case asX:  //Swap X and Y
				case asY: {
					int ix = axisOps.indexOf(nopt);
					axisOps[ix] = aopt;
					axisOps[index] = nopt;
					break;
					}
//					break;
				case Average:
				case Slice: {
					if (axisOps.count() < 3) return;
					int i;
					for(i = 0; axisOps.at(i) == asX || axisOps.at(i) == asY; i++) {;} // There is always at least one
					axisOps[i] = aopt;
					axisOps[index] = nopt;
					break;
					}
				default:
					NVBOutputError("Invalid way to treat an axis");
				}
			break;
		case Average:
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
				case Slice:
					axisOps[index] = nopt;
					break;
				default:
					NVBOutputError("Invalid way to treat an axis");
				}
			break;
		default:
			NVBOutputError("Invalid way to treat an axis");
		}

	updateWidgets();
	updateAverage();
}

void NVBSingleView::slicePosChanged(int pos)
{
	axisSlices[pos] = qobject_cast< QAbstractSlider* >(slds.mapping(pos))->value();
	axisindex_t numAv = 0;
	for(int i = 0; i < pos; i++)
		if (axisOps.at(i) != Slice)
			numAv += 1;
	sl_ixes[pos - numAv] = axisSlices[pos];
	view->setSliceIndexes(sl_ixes);
}

void NVBSingleView::updateAverage()
{
	axisindex_t numAv = 0;
//	bool avAxesChanged = false;
	
	sl_axes.clear();
	sl_ixes.clear();
	av_axes.clear();
	
	for(int i = 0; i < axisOps.count(); i++)
		switch(axisOps.at(i)) {
			case asX:
				axes[0] = i-numAv;
				break;
			case asY:
				axes[1] = i-numAv;
				break;
			case Average:
				numAv += 1;
				av_axes << i;
				break;
			case Slice:
				sl_axes << i - numAv;
				sl_ixes << axisSlices.at(i);
				break;
			}
	
//	if (avAxesChanged) {
		view->setDataSet(0);
		ods->setAveragingAxes(av_axes);
		view->setDataSet(ods);
//	}
	view->setXYAxes(axes);
	view->setSliceIndexes(sl_ixes);	
}

void NVBSingleView::updateWidgets()
{
	QGridLayout * l = qobject_cast< QGridLayout* >(layout());
	for(int i = 0; i<axisOps.count(); i++) {
		QComboBox * cb  = qobject_cast<QComboBox*>(l->itemAtPosition(i+1,1)->widget()); // Combobox 
		if (cb)
			cb->setCurrentIndex((int)axisOps.at(i));
		else
			NVBOutputError(QString("The item at (%1,1) is not a combobox").arg(i+1));
		
		l->itemAtPosition(i+1,2)->widget()->setEnabled(axisOps.at(i) == Slice); // Slider 
		l->itemAtPosition(i+1,3)->widget()->setEnabled(axisOps.at(i) == Slice); // Label 
		}
}
