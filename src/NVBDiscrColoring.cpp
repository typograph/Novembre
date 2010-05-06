//
// C++ Implementation: NVBDiscrColoring
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBDiscrColoring.h"
#include "NVBLogger.h"

NVBColoring::NVBRandomDiscrColorModel::NVBRandomDiscrColorModel(int ncolors):NVBDiscrColorModel(),nc(ncolors)
{
  if (nc < 0) nc = 0;
  qsrand(nc);
  for(int i = 0; i < nc; i++)
    colors.append(newcolor());
}

QColor NVBColoring::NVBRandomDiscrColorModel::colorize(int index) const
{
  if (index < 0 ) {
    NVBOutputPMsg("NVBRandomDiscrColorModel::colorize","Negative color queried for.");
    return Qt::black;
    }
  else if ( index >= nc) {
    NVBOutputPMsg("NVBRandomDiscrColorModel::colorize","Expanding color base");
    for( int i = nc; i <= index; i++)
      colors.append(newcolor());
    }
  return colors.at(index);
}

QColor NVBColoring::NVBRandomDiscrColorModel::newcolor() const
{
  return QColor::fromRgb(qrand()%255,qrand()%255,qrand()%255);
}
