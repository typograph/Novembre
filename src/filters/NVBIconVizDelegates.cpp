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
#include "NVBIconVizDelegates.h"
#include "../core/NVBContColoring.h"

QImage * colorizeWithPlaneSubtraction(NVB3DDataSource * page, const QSize & size = QSize()) {
	const double * pdata = page->getData();

	if (!pdata) return 0;

	double xnorm = 0, ynorm = 0;

	int iw = page->resolution().width();
	int ih = page->resolution().height();
	int sz = iw * ih;

	int nx = 0, ny = 0;

	for (int i = 0; i < sz; i += iw)
		if (FINITE(pdata[i]) && FINITE(pdata[i + iw - 1])) {
			xnorm += pdata[i] - pdata[i + iw - 1];
			nx += 1;
			}

	for (int i = 0; i < iw; i += 1)
		if (FINITE(pdata[i]) && FINITE(pdata[i + sz - iw])) {
			ynorm += pdata[i] - pdata[i + sz - iw];
			ny += 1;
			}

	if (nx > 0)
		xnorm /= (iw - 1) * nx;

	if (ny > 0)
		ynorm /= ny * iw * (ih - 1);

	double * ndata = (double *) malloc(sz * sizeof(double));

	double zmin, zmax;

	for (int i = 0; i < iw; i += 1)
		for (int j = 0; j < sz; j += iw)
			ndata[i + j] = pdata[i + j] + xnorm * i + ynorm * j;

	getMemMinMax<double>(ndata, sz, zmin, zmax);

	NVBRescaleColorModel * rm = new NVBRescaleColorModel(page->getColorModel());
	rm->setLimits(zmin, zmax);

	QImage * i = dynamic_cast<NVBContColorModel*>(rm)->colorize(ndata , page->resolution(), size);
	delete rm;
	free(ndata);
	return i;
	}

NVBTopoIconDelegate::NVBTopoIconDelegate(NVBDataSource * source): NVBIconVizDelegate(source), page(0) {
	if (source->type() != NVB::TopoPage)
		NVBOutputError("Not a topography page");
	else {
		cache = new QImage();
		setSource(source);
		}
	}

void NVBTopoIconDelegate::paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state) {
	if (cache)
		if (rect.size().width() > cache->size().width() || rect.size().height() > cache->size().height())
			if (cache->size().width() < page->resolution().width() || cache->size().height() < page->resolution().height())
				redrawCache(QSize(qMin(rect.size().width(), page->resolution().width()), qMin(rect.size().height(), page->resolution().height())));

	NVBIconVizDelegate::paint(painter, rect, mode, state);
	}

void NVBTopoIconDelegate::redrawCache(const QSize & size) {
	if (!page) return;

	if (cache) delete cache;

	cache = colorizeWithPlaneSubtraction(page, size);
	}

void NVBTopoIconDelegate::setSource(NVBDataSource * source) {
	if (page) page->disconnect(this);

	if (source->type() != NVB::TopoPage) {
		delete cache;
		cache = new QImage();
		page = 0;
		}
	else {
		page = (NVB3DDataSource*)(source);
		connectSource(source);
//  if (!page) throw nvberr_no_sense;
		redrawCache();
		}
	}

NVBSpecIconDelegate::NVBSpecIconDelegate(NVBDataSource * source): NVBIconVizDelegate(source) {
	if (source->type() != NVB::SpecPage)
		NVBOutputError("Not a spectroscopy page");
	else {
		page = (NVBSpecDataSource *)source;
		cache = new QImage();
		}
	}

void NVBSpecIconDelegate::paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state) {
	if (cache)
		if (rect.size().width() > cache->size().width() || rect.size().height() > cache->size().height())
			redrawCache(rect.size());

	NVBIconVizDelegate::paint(painter, rect, mode, state);
	}

void NVBSpecIconDelegate::redrawCache(const QSize & size) {
	if (!page) return;

	QList<QwtData *> datalist = page->getData();
	QList<QColor> colorlist = page->colors();

	if (cache) delete cache;

	if (size.isEmpty()) {
		cache = new QImage();
		return;
		}

	cache = new QImage(size, QImage::Format_ARGB32);

	if (!cache) {
		NVBOutputError("Not enough memory to recereate cache");
		return;
		}

	cache->fill(0x00FFFFFF);

//  scaler<double,int> h(page->getZMax(),page->getZMin(),0,npoints-1);

	QPainter painter;
	painter.begin(cache);

	for (int i = 0; i < datalist.size(); i++) {

		QwtData * data = datalist.at(i);
		QRectF zRect = data->boundingRect().normalized();

		painter.setPen(QPen(colorlist.at(i)));

		scaler<double, int> h(zRect.bottom(), zRect.top(), 0, size.height() - 1);
		scaler<double, int> w(zRect.left(), zRect.right(), 0, size.width() - 1);

		for (int j = 1; j < page->datasize().width(); j++) {
			painter.drawLine(w.scale(data->x(j - 1)), h.scale(data->y(j - 1)), w.scale(data->x(j)), h.scale(data->y(j)));
			}

		}

	painter.end();
	}

void NVBSpecIconDelegate::setSource(NVBDataSource * source) {
	if (page) page->disconnect(this);

	page = 0;

	if (source->type() != NVB::SpecPage) {
		delete cache;
		cache = new QImage();
		}
	else {
		page = (NVBSpecDataSource*)(source);
		connectSource(source);
		redrawCache();
		}
	}
