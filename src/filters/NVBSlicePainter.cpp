//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#include "NVBSlicePainter.h"
#include <QVBoxLayout>
#include <QAction>

#include "../../icons/slice.xpm"

NVBSliceColorModel::NVBSliceColorModel(const NVBContColorModel * _model, const NVBSpecDataSource * source): NVBDiscrColorModel(), model(0), sprovider(0), xindex(0), active(false) {
	setModel(_model);
	setSource(source);
	}

NVBSliceColorModel::~ NVBSliceColorModel() {
	}

QColor NVBSliceColorModel::colorize(int index) const {
	if (!active || !model || !sprovider)
		return Qt::black;

	return model->colorize(sprovider->getData().at(index)->y(xindex));
	}

void NVBSliceColorModel::setIndex(int index) {
	if (xindex != index) {
		xindex = index;
		emit adjusted();
		}
	}

void NVBSliceColorModel::setSource(const NVBSpecDataSource * source) {
	if (sprovider != source) {
		if (sprovider) disconnect(sprovider, 0, this, 0);

		sprovider = source;

		if (sprovider) {
			connect(sprovider, SIGNAL(dataAboutToBeChanged()), this, SLOT(deactivate()));
			connect(sprovider, SIGNAL(dataChanged()), this, SLOT(activate()));
			connect(sprovider, SIGNAL(dataAdjusted()), this, SIGNAL(adjusted()));

			if (model) activate();
			}
		}
	}

void NVBSliceColorModel::setModel(const NVBContColorModel * _model) {
	if (model != _model) {
		model = _model;

		if (model) {
			connect(model, SIGNAL(adjusted()), this, SIGNAL(adjusted()));

			if (sprovider) activate();
			}
		}
	}

// -----------------------

NVBSlicePainterWidget::NVBSlicePainterWidget(const NVBSpecDataSource * source): QWidget(), sprovider(0) {
	QVBoxLayout * l = new QVBoxLayout(this);

	sbox = new NVBPhysStepSpinBox(source->tDim(), source->boundingRect().left(), source->boundingRect().right(), source->datasize().width(), this);
	connect(sbox, SIGNAL(valueChanged(int)), this, SIGNAL(indexChanged(int)));

	l->addWidget(sbox);

	setSource(source);
	}

void NVBSlicePainterWidget::setSource(const NVBSpecDataSource * source) {
	if (sprovider != source) {
		if (sprovider) disconnect(sprovider, 0, this, 0);

		sprovider = source;
		updateLimits();

		if (sprovider) {
			connect(sprovider, SIGNAL(dataChanged()), this, SLOT(updateLimits()));
			connect(sprovider, SIGNAL(dataAdjusted()), this, SLOT(updateLimits()));
			}
		}
	}

void NVBSlicePainterWidget::updateLimits() {
	if (sprovider) {
		sbox->setLimits(sprovider->boundingRect().left(), sprovider->boundingRect().right(), sprovider->datasize().width());
		}
	}

// ----------------------------

NVBSlicePainter::NVBSlicePainter(NVBSpecDataSource * source, NVBContColorModel * model): NVBSpecFilterDelegate(source), swidget(0) {
	scolors = new NVBSliceColorModel();
	connect(scolors, SIGNAL(adjusted()), this, SIGNAL(colorsAdjusted()));
	ccolors = new NVBRescaleColorModel(model);
	scolors->setModel(ccolors);

	connectSignals();
	followSource();
	}

void NVBSlicePainter::connectSignals() {
	emit dataAboutToBeChanged();

	sprovider = (NVBSpecDataSource*)provider;

	if (scolors) scolors->setSource(sprovider);

	if (swidget) swidget->setSource(sprovider);

	ccolors->setLimits(sprovider->boundingRect().bottom(), sprovider->boundingRect().top());

	NVB_FORWARD_DATASIGNALS(sprovider);
	NVB_FORWARD_COLORSIGNALS(sprovider);

	emit dataChanged();

	}

QWidget * NVBSlicePainter::widget() {
	if (!swidget) {
		swidget = new NVBSlicePainterWidget(sprovider);
		connect(swidget, SIGNAL(indexChanged(int)), scolors, SLOT(setIndex(int)));
		}

	return swidget;
	}

QAction * NVBSlicePainter::action() {
	return new QAction(QIcon(_slice_icon), QString("Slice spectroscopy"), 0);
	}

void NVBSlicePainter::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	NVBSpecFilterDelegate::setSource(source);
	}


