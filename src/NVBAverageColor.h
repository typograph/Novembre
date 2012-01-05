//
// C++ Interface: NVBAverageColor
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBAVERAGECOLOR_H
#define NVBAVERAGECOLOR_H

class NVBAverageColor {
private:
  QList<QColor> colors;
  mutable QColor avcolor;
  mutable bool averaged;

  void average() const {
    if (colors.size() <= 0) return;
    uint r = 0, g = 0, b = 0; // TODO think about HSV
    foreach(QColor c, colors) {
      r += c.red();
      g += c.green();
      b += c.blue();
      }
    r /= colors.size();
    g /= colors.size();
    b /= colors.size();
    avcolor = QColor::fromRgb(r,g,b);
    averaged = true;
    }
public:
  NVBAverageColor():averaged(false) {;}

  inline QColor color() const {
    if (!averaged) average();
    return avcolor;
    }

  inline NVBAverageColor & operator<<( const QColor & color) {
    colors << color;
    return *this;
    }
};

#endif
