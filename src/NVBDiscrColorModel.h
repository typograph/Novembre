//
// C++ Interface: NVBDiscrColorModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBDISCRCOLORMODEL_H
#define NVBDISCRCOLORMODEL_H

#include <QObject>
#include <QList>
#include <QColor>

namespace NVBColoring {

class NVBDiscrColorModel : public QObject {
Q_OBJECT
public:
    NVBDiscrColorModel() {;}
    virtual ~NVBDiscrColorModel() {;}
  
    virtual QColor colorize(int) const = 0;
    virtual QList<QColor> colorize(int start, int end) const {
      if (start > end) return QList<QColor>();
      QList<QColor> list;
      for(int i = start; i<=end; i++)
        list.append(colorize(i));
      return list;
      }

//    virtual QString colorName(uint index) const = 0;
signals:
    void adjusted();
};

}

#endif
