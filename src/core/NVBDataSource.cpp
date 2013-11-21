//
// Copyright 2006 Timofey <typograph@elec.ru>
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


#include "NVBDataSource.h"

void useDataSource(NVBDataSource* source) {
	if (!source) return;

	source->refCount++;
	}

void releaseDataSource(NVBDataSource* source) {
	if (!source) return;

	source->refCount--;

	if (!source->refCount) delete source;
	}

QRectF NVBSpecDataSource::occupiedArea() const {
	qreal top, bottom, left, right;
	top = bottom = positions().first().y();
	left = right = positions().first().x();
	foreach(QPointF point, positions()) {
		top    = qMin(top, point.y());
		bottom = qMax(bottom, point.y());
		left   = qMin(left, point.x());
		right  = qMax(right, point.x());
		}
	return QRectF(left, top, right - left, bottom - top);
	}

QRectF NVBSpecDataSource::boundingRect() const {
	QRectF rect;
	foreach(QwtData * d, getData()) {
		rect = rect.united(d->boundingRect());
		}
	return rect.normalized();
	}

