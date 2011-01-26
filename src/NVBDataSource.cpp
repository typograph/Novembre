//
// C++ Implementation: NVBDataSource
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
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

QRectF NVBSpecDataSource::occupiedArea() const
{
	qreal top, bottom, left, right;
	top = bottom = positions().first().y();
	left = right = positions().first().x();
  foreach(QPointF point,positions()) {
		top    = qMin(top,point.y());
		bottom = qMax(bottom,point.y());
		left   = qMin(left,point.x());
		right  = qMax(right,point.x());
		}
	return QRectF(left,top,right-left,bottom-top);
}

QRectF NVBSpecDataSource::boundingRect() const 
{
  QRectF rect;
  foreach(QwtData * d, getData()) {
    rect = rect.united(d->boundingRect());
    }
  return rect.normalized();
}

