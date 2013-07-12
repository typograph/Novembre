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
#include "NVBCurveClassifier.h"

#include "../../icons/classifier.xpm"

#include <QAction>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QBrush>
#include <QGraphicsView>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include <float.h>

//------------------

inline double sqr(double n) { return n * n; }

//double distance_norm(QwtData * d1, QwtData* d2) {
//	double n;
//	for(int i=0; i < d1->size() && i < d2->size(); i++)
//		n += sqr(d1->y(i) - d2->y(i));
//	return sqrt(n);
//}

double distance_norm(const double * d1, const QwtData* d2) {
	double n = 0;

	for (uint i = 0; i < d2->size(); i++)
		n += sqr(d1[i] - d2->y(i));

	return sqrt(n);
	}

void addToData(double * dest, const QwtData * source) {
	for (uint i = 0; i < source->size(); i++)
		dest[i] += source->y(i);
	}

void divData(double * dest, uint n, double factor) {
	for (uint i = 0; i < n; i++)
		dest[i] /= factor;
	}

// -----------------

NVBCurveClassifierWidget::NVBCurveClassifierWidget(): QWidget() {
	QGridLayout * g = new QGridLayout(this);
	QLabel * label = new QLabel("Classes");
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	g->addWidget(label, 0, 0);
	kBox = new QSpinBox();
	kBox->setRange(2, 17);
	connect(kBox, SIGNAL(valueChanged(int)), this, SIGNAL(kChanged(int)));
	g->addWidget(kBox, 0, 1, 1, 2);

	label = new QLabel("Iterations");
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	g->addWidget(label, 1, 0);
	miBox = new QSpinBox();
	miBox->setRange(1, 100);
	connect(miBox, SIGNAL(valueChanged(int)), this, SIGNAL(maxIterChanged(int)));
	g->addWidget(miBox, 1, 1);

	QPushButton * refr = new QPushButton(QIcon(_clr_refresh), QString());
	refr->setToolTip("Reclassify");
	refr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(refr, SIGNAL(clicked()), this, SIGNAL(recalculate()));
	g->addWidget(refr, 1, 2);

	setWindowTitle("Curve classifier");

	setK(6);
	setMaxIter(20);
	}

NVBCurveClassifierWidget::~ NVBCurveClassifierWidget() {}

int NVBCurveClassifierWidget::k() {
	if (kBox) return kBox->value();

	return 1;
	}

void NVBCurveClassifierWidget::setK(int k) {
	if (kBox) kBox->setValue(k);
	}

int NVBCurveClassifierWidget::maxIter() {
	if (miBox) return miBox->value();

	return 0;
	}

void NVBCurveClassifierWidget::setMaxIter(int mi) {
	if (miBox) miBox->setValue(mi);
	}

// ---------

NVBCurveClassifier::NVBCurveClassifier(NVBSpecDataSource * source): NVBSpecFilterDelegate(source), sprovider(0), nClasses(6), nIterations(20) {
	colors = new NVBMappingDiscrColorModel(QList<QColor>() << Qt::black << Qt::red << Qt::green << Qt::blue << Qt::cyan << Qt::magenta << Qt::yellow << Qt::gray << Qt::darkGray << Qt::gray << Qt::darkRed << Qt::darkGreen << Qt::darkBlue << Qt::darkCyan << Qt::darkMagenta << Qt::darkYellow << Qt::lightGray);

	connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));

	setSource(source);
	}

NVBCurveClassifier::~NVBCurveClassifier() {
	if (colors)
		delete colors;
	}

const NVBDiscrColorModel *NVBCurveClassifier::getColorModel() const {
	return colors;
	}

void NVBCurveClassifier::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	NVBSpecFilterDelegate::setSource(source);

	}

void NVBCurveClassifier::connectSignals() {
	sprovider = (NVBSpecDataSource*)provider;

	connect(sprovider, SIGNAL(dataAboutToBeChanged()), SIGNAL(dataAboutToBeChanged()));
	connect(sprovider, SIGNAL(dataAboutToBeChanged()), SIGNAL(colorsAboutToBeChanged()));
	connect(sprovider, SIGNAL(dataAdjusted()), SLOT(recalculate()));
	connect(sprovider, SIGNAL(dataAdjusted()), SIGNAL(dataAdjusted()));
	connect(sprovider, SIGNAL(dataChanged()), SLOT(recalculate()));
	connect(sprovider, SIGNAL(dataChanged()), SIGNAL(dataChanged()));

//	emit colorsAboutToBeChanged();
	recalculate();

	}

void NVBCurveClassifier::recalculate() {

	if (!sprovider) return;

	int nCurves = sprovider->datasize().height();
	int nPts = sprovider->datasize().width();

	QVector<int> classes(nCurves);
	classes.fill(0);

	if (nClasses < 2) {
		colors->setMapping(classes);
		return;
		}

	if (nClasses >= nCurves) {
		for (int i = 0; i < nCurves; i++)
			classes[i] = i;

		colors->setMapping(classes);
		return;
		}

	QVector<int> nMembers(nClasses);
	nMembers.fill(0);

	QVector<QVector<double> > centroids(nClasses);
	centroids.fill(QVector<double>(nPts));

	// Initialize centroids to random curves from the page

	QList<int> rnds;
	int rnd_indx = -1;

	for (int j = 0; j < nClasses; j++) {
		centroids[j].fill(0);

		while (rnd_indx < 0 || rnds.indexOf(rnd_indx) >= 0) {
			rnd_indx = rand() % nCurves;
			}

		rnds << rnd_indx;
		addToData(centroids[j].data(), sprovider->getData().at(rnd_indx));
		}

	// Run K-Means
	for (int iter = 0; iter < nIterations; iter++) { // Rather arbitrary MAX_ITER

		// For each example in X, assign it to the closest centroid

		nMembers.fill(0);

		for (int i = 0; i < nCurves; i++) {
			// Calculate distances and find minimum
			double minNorm = DBL_MAX;
			classes[i] = 0;

			for (int j = 0; j < nClasses; j++) {
				double norm = distance_norm(centroids.at(j).constData(), sprovider->getData().at(i));

				if (norm < minNorm) {
					minNorm = norm;
					classes[i] = j;
					}
				}
			}

		for (int j = 0; j < nClasses; j++) {
			centroids[j].fill(0);
			}

		for (int i = 0; i < nCurves; i++) {
			addToData(centroids[classes.at(i)].data(), sprovider->getData().at(i));
			nMembers[classes.at(i)] += 1;
			}

		for (int j = 0; j < nClasses; j++) {
			divData(centroids[j].data(), nPts, nMembers.at(j));
			}

		}

	// Reorder according to population;

	QMap<int, int> cls;

	for (int j = 0; j < nClasses; j++)
		cls.insert(-nMembers.at(j), j);

	QList<int> srtIndexes = cls.values();
	QVector<int> srtInvIndexes(nClasses);

	for (int i = 0; i < srtIndexes.size(); i++)
		srtInvIndexes[srtIndexes.at(i)] = i;

	for (int i = 0; i < nCurves; i++) {
		classes[i] = srtInvIndexes.at(classes.at(i));
		}

	colors->setMapping(classes);

	}

QAction * NVBCurveClassifier::action() {
	return new QAction(QIcon(_clr_class), QString("Curve classifier"), 0);
	}

QWidget *NVBCurveClassifier::widget() {
	NVBCurveClassifierWidget * w = new NVBCurveClassifierWidget();
	connect(w, SIGNAL(recalculate()), this, SLOT(recalculate()));
	connect(w, SIGNAL(kChanged(int)), this, SLOT(setClasses(int)));
	connect(w, SIGNAL(maxIterChanged(int)), this, SLOT(setIterations(int)));
	w->setK(nClasses);
	w->setMaxIter(nIterations);
	return w;
	}
