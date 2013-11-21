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
#include "QDimension.h"

#ifndef max
#ifdef qMax
#define max qMax
#define min qMin
#else

long max(long a, long b) {
	if (a > b)
		return a;
	else
		return b;
	}

double max(double a, double b) {
	if (a > b)
		return a;
	else
		return b;
	}
#endif
#endif

Q2DScaler::Q2DScaler() {
	multW = 0;
	multH = 0;
	}

QPointF * Q2DScaler::scale(QPointF * pt) const {
	(*pt) -= fromP;
	pt->rx() *= multW;
	pt->ry() *= multH;
	(*pt) += toP;
	return pt;
	}

QSizeF * Q2DScaler::scale(QSizeF * sz) const {
	sz->rwidth() *= multW;
	sz->rheight() *= multH;
	return sz;
	}

QRectF * Q2DScaler::scale(QRectF * rect) const {
	rect->setTopLeft(scale(rect->topLeft()));
	rect->setBottomRight(scale(rect->bottomRight()));
	return rect;
	}

QPoint * Q2DScaler::scale(QPoint * pt) const {
	(*pt) -= fromP.toPoint();
	pt->rx() = pt->x() * multW;
	pt->ry() = pt->y() * multH;
	(*pt) += toP.toPoint();
	return pt;
	}

QSize * Q2DScaler::scale(QSize * sz) const {
	sz->rwidth() *= multW;
	sz->rheight() *= multH;
	return sz;
	}

QRect * Q2DScaler::scale(QRect * rect) const {
	rect->setTopLeft(scale(rect->topLeft()));
	rect->setBottomRight(scale(rect->bottomRight()));
	return rect;
	}

Q2DScaler::Q2DScaler(QRectF rect1, QRectF rect2) {
	fromP = rect1.topLeft();
	fromS = rect1.size();
	toP = rect2.topLeft();
	toS = rect2.size();
	setMult();
	}

void Q2DScaler::setFrom(QRectF rect) {
	fromP = rect.topLeft();
	fromS = rect.size();
	setMult();
	}

void Q2DScaler::setTo(QRectF rect) {
	toP = rect.topLeft();
	toS = rect.size();
	setMult();
	}

void Q2DScaler::setMult() {
	multW = toS.width() / fromS.width();
	multH = toS.height() / fromS.height();
	}

Q2DSqScaler::Q2DSqScaler(): Q2DScaler() {
	}

Q2DSqScaler::Q2DSqScaler(QPointF pt1, double sz1, QPointF pt2, double sz2): Q2DScaler(QRectF(pt1, QSizeF(sz1, sz1)), QRectF(pt2, QSizeF(sz2, sz2))) {
	}

Q2DSqScaler::Q2DSqScaler(QRectF rect1, QRectF rect2): Q2DScaler(rect1, rect2) {
	if (multW != multH) {
		fromS.setHeight(fromS.width());
		toS.setHeight(toS.width());
		setMult();
		}
	}

void Q2DSqScaler::setFrom(QRectF rect) {
	fromP = rect.topLeft();
	fromS.setHeight(max(rect.width(), rect.height()));
	fromS.setWidth(fromS.height());
	setMult();
	}

void Q2DSqScaler::setTo(QRectF rect) {
	toP = rect.topLeft();
	toS.setHeight(max(rect.width(), rect.height()));
	toS.setWidth(toS.height());
	setMult();
	}

bool Q2DScaler::inflates() {
	return (fabs(multH) > 1) && (fabs(multW) > 1);
	}

bool Q2DScaler::deflates() {
	return (fabs(multH) < 1) && (fabs(multW) < 1);
	}

bool Q2DScaler::onetoone() {
	return (fabs(multH) == 1) && (fabs(multW) == 1);
	}

void Q2DScaler::scaleTo(double factor, QPointF ref) {
	if (ref != toP) {
		toP *= factor;
		ref *= (factor - 1);
		toP -= ref;
		}

	toS *= factor;
	setMult();
	}
