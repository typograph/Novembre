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

/**
 * \class NVBPlotCurves
 *
 * A QwtPlotItem that draws a data array as a set of curves.
 *
 * When passed an @a NVBDataSet in constructor, will render its data
 * using the first axis as the X axis of the graph, and plotting
 * all the other axes as different curves. To change the X axis use @a setXAxis
 *
 * The number of curves that are drawn can be limited by @a setMaxCurves.
 * The rendered curves are selected uniformly among the available ones.
 *
 * The colour of the curves is defined by the axis colour maps available
 * in this dataset (see \a NVBDataSlice::associatedColor)
 */

#include "NVBPlotCurves.h"
#include "NVBDataCore.h"
#include "NVBAxisMaps.h"

/**
 * @fn NVBPlotCurves::NVBPlotCurves
 *
 * Creates a QwtPlotItem to render the dataset @a data
 * The name of the item (and the legend entry) will be set to
 * dataset's name.
 *
 * In case a NULL dataset is supplied, nothing is drawn.
 *
 * NVBPlotCurves does not assume ownership of the dataset.
 * The dataset can be safely deleted without deleting the NVBPlotCurves.
 */
NVBPlotCurves::NVBPlotCurves(const NVBDataSet* data)
	: QObject()
	, QwtPlotItem()
	, parent(0)
	, axisX(-1)
	, max_curves(-1)
	, xdata(0) {
	drawCurve = new QwtPlotCurve();

	if (!drawCurve)
		NVBOutputError("Not enough memory to allocate curve");

	setItemAttribute(AutoScale);
	setDataSet(data);
	}


NVBPlotCurves::~NVBPlotCurves() {
	setDataSet(0);

	if (drawCurve) delete drawCurve;
	}

/**
 * @fn NVBPlotCurves::draw
 *
 * Draw a maximum of @a maxCurves curves using the colours associated
 * with dataset's axes. (see @a forNSlices)
 */

void NVBPlotCurves::draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const {
	if (!parent) return;

	if (!drawCurve) return;

	if (!xdata) return;

	QVector<axisindex_t> xa;
	xa << axisX;

	forNSlices(parent, max_curves, slice_axes, xa) {
		drawCurve->setRawSamples(xdata, SLICE.data, parent->sizeAt(axisX));
		drawCurve->setPen(SLICE.associatedColor());
		drawCurve->draw(painter, xMap, yMap, canvasRect);
		}
	}

void NVBPlotCurves::setDataSet(const NVBDataSet* data) {
	releaseDataSet(parent);
	parent = 0;

	if (xdata) {
		free(xdata);
		xdata = 0;
		}

	rect = QRectF();

	if (data) {
		parent = data;
		useDataSet(parent);
		connect(parent, SIGNAL(dataChanged()), this, SLOT(parentDataChanged()));
		connect(parent, SIGNAL(dataReformed()), this, SLOT(parentAxesChanged()));
		connect(parent, SIGNAL(destroyed()), this, SLOT(setDataSet()));

		setTitle(data->name());

		parentAxesChanged();
		}
	}

/**
 * @fn NVBPlotCurves::setMaxCurves
 *
 * Drawing all curves may be costly, so the number of curves to draw may be limited to \a max.
 * A value of 0 removes the limitation.
 */
void NVBPlotCurves::setMaxCurves(unsigned int max) {
	max_curves = max;
	itemChanged();
	}

/**
 * @fn NVBPlotCurves::setXAxis
 *
 * Sets a new axis to be used as the X axis of the plot.
 */
void NVBPlotCurves::setXAxis(axisindex_t x) {
	if (axisX != x) {
		if (x >= parent->nAxes()) {
			NVBOutputError(QString("Axis index %1 out of range").arg(x));
			return;
			}

		if (axisX >= 0) // Else we have no x axis yet
			slice_axes.insert(axisX, axisX);

		axisX = x;
		slice_axes.remove(axisX);

		if (xdata)
			free(xdata);

		xdata = (double*) calloc(parent->sizeAt(x), sizeof(double));

		if (!xdata)
			NVBOutputError("Not enough memory to allocate xdata");
		else {
			NVBAxisPhysMap * xmap = parent->axisAt(x).physMap();

			if (xmap)
				for(axissize_t i = 0; i < parent->sizeAt(x); i++)
					xdata[i] = xmap->value(i).getValue();
			else
				for(axissize_t i = 0; i < parent->sizeAt(x); i++)
					xdata[i] = i;

			}

		parentDataChanged();
		}
	}

/**
 * Called when parent data changes to update things
 */
void NVBPlotCurves::parentDataChanged() {
	if (xdata && parent)
		rect.setCoords(xdata[0], parent->min(), xdata[parent->sizeAt(axisX) - 1], parent->max());
	else
		rect = QRectF();

	rect = rect.normalized();
	itemChanged();
	}

void NVBPlotCurves::parentAxesChanged() {
	slice_axes.fill(0, parent->nAxes());

	for(int i = 0; i < parent->nAxes(); i++)
		slice_axes[i] = i;

	axisX = -1;

	setXAxis(0);
	}


/*
void NVBPlotCurves::setSliceAxes(QVector< axisindex_t > axes)
{
	slice_axes = axes;
	setSliceIndexes();
}


void NVBPlotCurves::setSliceIndexes(QVector< axissize_t > indexes)
{
	if (indexes.count() == slice_axes.count())
		slice_indexes = indexes;
	else {
		if (!indexes.isEmpty())
			NVBOutputError("Number of supplied indexes doesn't match the number of axes");
		slice_indexes.fill(0,slice_axes.count());
		}

	regenerateSlice();
	itemChanged();
}

void NVBPlotCurves::regenerateSlice()
{
	if (!parent) return;
	if (ydata) free(ydata);

	ydata = sliceDataSet(parent,slice_axes,slice_indexes);

}
*/
