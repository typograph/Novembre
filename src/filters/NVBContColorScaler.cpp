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
#include "NVBContColorScaler.h"

#include <float.h>
#include <QIcon>
#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <QPainter>

#include "../../icons/ccoloring.xpm"

#define cbarWidth 200
#define cbarHeight 50
#define markerRadius 5
#define widgetMargin 3

NVBContColorScalerDelegate::NVBContColorScalerDelegate(NVB3DDataSource * source, NVBRescaleColorModel * model): NVB3DFilterDelegate(source), page(0), colors(model) {
	connectSignals();
	followSource();
	connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));
	}

void NVBContColorScalerDelegate::setModel(NVBRescaleColorModel * model) {
	emit colorsAboutToBeChanged();

	if (colors) delete colors;

	colors = model;
	connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));
	emit colorsChanged();
	}

void NVBContColorScalerDelegate::connectSignals() {
	page = (NVB3DDataSource*)provider;

	connect(provider, SIGNAL(dataAboutToBeChanged()), SLOT(parentColorsAboutToBeChanged()));
	connect(provider, SIGNAL(dataAboutToBeChanged()), SIGNAL(dataAboutToBeChanged()));
	connect(provider, SIGNAL(dataAdjusted()), SIGNAL(dataAdjusted()));
	connect(provider, SIGNAL(dataChanged()), SIGNAL(dataChanged()));
	connect(provider, SIGNAL(dataChanged()), SLOT(parentColorsChanged()));

	parentColorsChanged();

	connect(provider, SIGNAL(colorsAboutToBeChanged()), SLOT(parentColorsAboutToBeChanged()));
	connect(provider, SIGNAL(colorsChanged()), SLOT(parentColorsChanged()));

	provider->override(this);

	}

void NVBContColorScalerDelegate::setSource(NVBDataSource * source) {
	// Make sure there's no page any more. page = 0 is not there, to be sure ghost data requests don't fail.

	if (page) page->disconnect(this);

	// If underlying page type is not good, self-destruct

	if (source->type() != NVB::TopoPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	NVB3DFilterDelegate::setSource(source);

	}

void NVBContColorScalerDelegate::parentColorsChanged() {
	colors->setModel(page->getColorModel());
	}

void NVBContColorScalerDelegate::parentColorsAboutToBeChanged() {
	colors->setModel(0);
	}

// -------------------------

NVBContColorScaler::NVBContColorScaler(NVB3DDataSource * source):
	QWidget(), provider(source), histogram(0), model(0), dirty(false) {
	movingrect = 0;
	z1 = z2 = c1 = c2 = 0;

//   setMinimumSize(QSize(50,heightForWidth(50)));
	setWindowTitle("Color scaling");
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	colors = new NVBRescaleColorModel(source->getColorModel());
	connect(colors, SIGNAL(adjusted()), SLOT(update()));
	page = new NVBContColorScalerDelegate(source, colors);
//   connect(this,SIGNAL(colorsChanged()),page,SIGNAL(colorsChanged()));

	setSource(source);
	}

QAction * NVBContColorScaler::action() {
	return new QAction(QIcon(_clr_limits), QString("Color adjust"), 0);
	}

void NVBContColorScaler::setSource(NVBDataSource * source) {
	if (source->type() == NVB::TopoPage) {
		if (provider)
			provider->disconnect(this);

		provider = (NVB3DDataSource*)source;

		invalidateModel();

		connect(source, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
		connect(source, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);
		connect(source, SIGNAL(colorsChanged()), SLOT(update()));
		connect(source, SIGNAL(dataAdjusted()), SLOT(parentDataChanged()));
		connect(source, SIGNAL(dataChanged()), SLOT(parentDataChanged()));
		}
	}

void NVBContColorScaler::invalidateModel() {
	colors->setModel(provider->getColorModel());
	colors->overrideLimits(provider->getZMin(), provider->getZMax());
	}

void NVBContColorScaler::paintModel(QPainter * painter, double zmin, double zmax, int w, int h) {
	scaler<int, double> sc(0, w - 1, zmin, zmax);

	for (int i = 0; i < w; i++) {
		painter->setPen(QPen(colors->colorize(sc.scale(i))));
		painter->drawLine(i, 0, i, h - 1);
		}
	}

void NVBContColorScaler::paintHistogram(QPainter * painter, int w, int h) {

	QVector<uint> hdata(abs(w) + 1, 0);
	const double * pdata = provider->getData();

	if (!pdata) return;

	scaler<double, int> ww(page->getZMin(), page->getZMax(), 0, abs(w) - 1);

	for (int i = provider->resolution().width() * provider->resolution().height() - 1; i >= 0; i--) {
		if (FINITE(pdata[i]))
			hdata[abs(ww.scaleInt(pdata[i]))] += 1;
		}

	painter->setPen(QPen(Qt::blue));

	uint max = hdata.at(0);

	for (int i = 1; i < abs(w); i++)
		max = qMax(max, hdata.at(i));

	scaler<int, int> hh(0, max, h - 1, 1);

	if (w > 0)
		for (int i = 1; i < w; i++)
			painter->drawLine(i - 1, hh.scaleInt(hdata.at(i - 1)), i, hh.scaleInt(hdata.at(i)));
	else
		for (int i = -1; i > w; i--)
			painter->drawLine(i + 1, hh.scaleInt(hdata.at(abs(i + 1))), i, hh.scaleInt(hdata.at(abs(i))));

	}

NVBDataSource * NVBContColorScaler::filter() {
	return page;
	}

void NVBContColorScaler::paintEvent(QPaintEvent * event) {
	Q_UNUSED(event);

	double pzmin = qMin(qMin(colors->zMin(), page->getZMin()), qMin(colors->zMax(), page->getZMax()));
	double pzmax = qMax(qMax(colors->zMin(), page->getZMin()), qMax(colors->zMax(), page->getZMax()));

	scaler<double, int> zsc(pzmin, pzmax, minx(), maxx());

	QPainter painter(this);

	if (dirty && !movingrect) {
		z1 = zsc.scaleInt(page->getZMin());
		z2 = zsc.scaleInt(page->getZMax());
		c1 = zsc.scaleInt(colors->zMin());
		c2 = zsc.scaleInt(colors->zMax());
		dirty = false;
		}

	painter.save();
	painter.translate(z1, histy());
	paintHistogram(&painter, z2 - z1 + 1, histh());
	painter.restore();

	painter.save();
	painter.translate(minx(), zy());
	paintModel(&painter, pzmin, pzmax, maxx() - minx() + 1, cy() - zy() + 1);
	painter.restore();

	painter.setPen(QPen(Qt::green));
	painter.drawLine(z1, histy(), z1, cy());
	painter.drawLine(z2, histy(), z2, cy());
	painter.setPen(QPen(Qt::red));
	painter.drawLine(c1, histy(), c1, cy());
	painter.drawLine(c2, histy(), c2, cy());

	painter.setPen(QPen(Qt::black));
	painter.setBrush(QBrush(Qt::white));

#if QT_VERSION >= 0x040400
	painter.drawEllipse(QPoint(z1, zy()), markerRadius, markerRadius);
	painter.drawEllipse(QPoint(z2, zy()), markerRadius, markerRadius);
	painter.drawEllipse(QPoint(c1, cy()), markerRadius, markerRadius);
	painter.drawEllipse(QPoint(c2, cy()), markerRadius, markerRadius);
#else
	painter.drawEllipse(z1 - markerRadius, zy() - markerRadius, 2 * markerRadius, 2 * markerRadius);
	painter.drawEllipse(z2 - markerRadius, zy() - markerRadius, 2 * markerRadius, 2 * markerRadius);
	painter.drawEllipse(c1 - markerRadius, cy() - markerRadius, 2 * markerRadius, 2 * markerRadius);
	painter.drawEllipse(c2 - markerRadius, cy() - markerRadius, 2 * markerRadius, 2 * markerRadius);
#endif

	painter.end();
	}

QSize NVBContColorScaler::sizeHint() const {
	return QSize(cbarWidth, cbarHeight + histh() + widgetMargin);
	}

int NVBContColorScaler::heightForWidth(int w) const {
	Q_UNUSED(w);
	return -1;
	}

void NVBContColorScaler::mousePressEvent(QMouseEvent * event) {
	movingrect = 0;

	if (abs(zy() - event->y()) <= markerRadius) {
		if (abs(z1 - event->x()) <= markerRadius)
			movingrect = &z1;
		else if (abs(z2 - event->x()) <= markerRadius)
			movingrect = &z2;
		}
	else if (abs(cy() - event->y()) <= markerRadius) {
		if (abs(c1 - event->x()) <= markerRadius)
			movingrect = &c1;
		else if (abs(c2 - event->x()) <= markerRadius)
			movingrect = &c2;
		}
	}

void NVBContColorScaler::mouseMoveEvent(QMouseEvent * event) {
	if (movingrect) {
		// The new X position
		int mousex = event->x();

		// Restrict movement to rectangle
		if (mousex < minx()) {
			if (*movingrect == minx()) return;

			mousex = minx();
			}

		if (mousex > maxx()) {
			if (*movingrect == maxx()) return;

			mousex = maxx();
			}

		// Motion
		if (*movingrect - minx() == 0 && qMin(c1, c2) != qMin(z1, z2)) {
			// The marker is alone on the left, move others
			*movingrect = mousex;

			// Find the new leftmost corner
			int newmin = qMin(qMin(z1, z2), qMin(c1, c2));
			scaler<int, int> isc(newmin, maxx(), minx(), maxx());

			// Move corners
			z1 = qMax(qMin(isc.scaleInt(z1), maxx()), minx());
			z2 = qMax(qMin(isc.scaleInt(z2), maxx()), minx());
			c1 = qMax(qMin(isc.scaleInt(c1), maxx()), minx());
			c2 = qMax(qMin(isc.scaleInt(c2), maxx()), minx());
			}
		else if (maxx() - *movingrect == 0 && qMax(c1, c2) != qMax(z1, z2)) {
			// The marker is alone on the right, move others
			*movingrect = mousex;

			// Find the new rightmost corner
			int newmax = qMax(qMax(z1, z2), qMax(c1, c2));
			scaler<int, int> isc(minx(), newmax, minx(), maxx());

			z1 = qMax(qMin(isc.scaleInt(z1), maxx()), minx());
			z2 = qMax(qMin(isc.scaleInt(z2), maxx()), minx());
			c1 = qMax(qMin(isc.scaleInt(c1), maxx()), minx());
			c2 = qMax(qMin(isc.scaleInt(c2), maxx()), minx());
			}
		else {
			int movingdir = (*movingrect > mousex) ? 1 : -1;
			*movingrect = mousex;

			if (abs(c1 - c2) < 2) {
				while (abs(c1 - c2) < 2) *movingrect -= movingdir;

				int over = (movingdir > 0) ? (*movingrect - minx()) : (maxx() - *movingrect);

				if (over < 0) {
					c1 -= movingdir * over;
					c2 -= movingdir * over;
					}
				}
			else if (abs(z1 - z2) < 2) {
				while (abs(z1 - z2) < 2) *movingrect -= movingdir;

				int over = (movingdir > 0) ? (*movingrect - minx()) : (maxx() - *movingrect);

				if (over < 0) {
					z1 -= movingdir * over;
					z2 -= movingdir * over;
					}
				}
			}

		scaler<int, double> zsc(z1, z2, page->getZMin(), page->getZMax());
		colors->setLimits(zsc.scale(c1), zsc.scale(c2));
		update();
		}
	}

void NVBContColorScaler::mouseReleaseEvent(QMouseEvent * event) {
	Q_UNUSED(event);

	if (movingrect) {
//     if (movingrect == &z1) {
//       colors->setimits();
//       }
//     else if (movingrect == &z2) {
//       }
//     else if (movingrect == &c1) {
//       }
//     else if (movingrect == &c2) {
//       }
		movingrect = 0;
		}
	}

void NVBContColorScaler::resizeEvent(QResizeEvent * event) {
	Q_UNUSED(event);
	dirty = true;
//   paintHistogram();
	}

int NVBContColorScaler::cy() const {
	return height() - widgetMargin - markerRadius;
	}

int NVBContColorScaler::zy() const {
	return 2 * widgetMargin + markerRadius + histh();
	}

int NVBContColorScaler::maxx() const {
	return width() - widgetMargin - markerRadius;
	}

int NVBContColorScaler::minx() const {
	return widgetMargin + markerRadius;
	}

int NVBContColorScaler::histy() const {
	return widgetMargin;
	}

int NVBContColorScaler::histw() const {
	return maxx() - minx() + 1;
	}

int NVBContColorScaler::histh() const {
	return 100;
	}

QSize NVBContColorScaler::minimumSizeHint() const {
	return QSize(50, 40 + cy() - zy());
	}

void NVBContColorScaler::parentDataChanged() {
	scaler<int, double> zsc(z1, z2, page->getZMin(), page->getZMax());
//  colors->overrideLimits(zsc.scale(c1),zsc.scale(c2));
	colors->setLimits(zsc.scale(c1), zsc.scale(c2));
	update();
	}


