//
// C++ Implementation: NVBPages
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBPages.h"

//------------- NVB3DPage ---------------------

NVB3DPage::~ NVB3DPage( )
{
  if (colors)
    delete colors;
  if (data)
    free(data);
}

bool NVB3DPage::setColorModel( NVBContColorModel * colorModel )
{
  if (colorModel) {
    emit colorsAboutToBeChanged();
    if (colors) delete colors;
    colors = colorModel;
    connect(colors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));
    emit colorsChanged();
    }
  return (bool)colorModel;
}

bool NVB3DPage::setData( double * newdata )
{
  if (newdata) {
    emit dataAboutToBeChanged();
    if (data)
      free(data);
    data = newdata;
    getMinMax();
    emit dataChanged();
    }
  return (bool)newdata;
}

void NVB3DPage::getMinMax()
{
  if (data)
    getMemMinMax<double>(data, _resolution.width()*_resolution.height() , zMin, zMax);
}

//------------- NVBSpecPage -------------------

NVBSpecPage::~ NVBSpecPage( )
{
   while (!_data.isEmpty())
     delete _data.takeFirst();
   if (colors) delete colors;
}


bool NVBSpecPage::setColorModel(NVBDiscrColorModel * colorModel)
{
  if (colorModel) {
    emit colorsAboutToBeChanged();
    if (colors) delete colors;
    colors = colorModel;
    connect(colors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));
    emit colorsChanged();
    }
  return (bool)colorModel;
}

bool NVBSpecPage::setData(QList< QwtData * > data)
{
  if (data.isEmpty()) return false;
  emit dataAboutToBeChanged();
  while(!_data.isEmpty())
    delete _data.takeFirst();
  _data = data;
  getMinMax();
  emit dataChanged();
  return true;
}


bool NVBSpecPage::setData(int y, QwtData * data)
{
  if (data) {
    emit dataAboutToBeChanged();
    _data.replace(y,data);
    getMinMax();
    emit dataChanged();
    }
  return (bool)data;
}

void NVBSpecPage::getMinMax( )
{
  zMin = 1e+120;
  zMax = -1e+120;
  for (int i = _datasize.height()-1;i>=0;i--) {
    QwtDoubleRect r = _data.at(i)->boundingRect().normalized();
    if (zMin > r.top()) zMin = r.top();
    if (zMax < r.bottom()) zMax = r.bottom();
    }
}


