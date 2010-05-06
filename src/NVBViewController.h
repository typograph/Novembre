//
// C++ Interface: NVBViewController
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBVIEWCONTROLLER_H
#define NVBVIEWCONTROLLER_H

#include <QModelIndex>
#include <QWidget>
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

class NVBWorkingArea {
public:
  NVBWorkingArea() {;}
  virtual ~NVBWorkingArea() {;}
  virtual void addPersistentWindow(QWidget * window) = 0;
  virtual void addWindow(QWidget * window) = 0;
  virtual QWidget * newWindowParentWidget() = 0;
};

class NVBViewController {
private:
public:
  NVBViewController( NVBWorkingArea * area ):parent_area(area) {;}
  virtual ~NVBViewController() {;}

  virtual NVB::ViewType viewType() =0;

  virtual void setSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion()) =0;
  virtual void addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion()) =0;

  virtual void setVisualizer(NVBVizUnion visualizer) =0;
  virtual void addControlWidget(QWidget * controlWidget) =0;
  virtual void setActiveVisualizer(NVBVizUnion visualizer) = 0;

/*
  virtual void addControlWidget(QWidget * widget) =0;
  virtual void setControlWidget(QWidget * widget) =0;
*/

  virtual NVBViewController * openInNewWindow(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion(), NVB::ViewType vtype = NVB::DefaultView) =0;

  NVBWorkingArea * area() { return parent_area; }

protected:
  NVBWorkingArea * parent_area;

};

#endif
