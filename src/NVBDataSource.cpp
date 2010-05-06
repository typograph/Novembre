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
  QRectF rect;
  QPointF shift(1,1);
  foreach(QPointF point,positions()) {
#if QT_VERSION >= 0x040300  
    rect |= QRectF(point,point+shift);
#else
    rect |= QRectF(point,QSizeF(shift.x(),shift.y()));
#endif
    }
  rect.setBottomRight(rect.bottomRight()-shift);
  return rect;
}

QRectF NVBSpecDataSource::boundingRect() const 
{
  QRectF rect;
  foreach(QwtData * d, getData()) {
    rect = rect.united(d->boundingRect());
    }
  return rect.normalized();
}

