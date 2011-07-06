//
// C++ Interface: NVBQtiExportView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBQTIEXPORTVIEW_H
#define NVBQTIEXPORTVIEW_H

#include "NVBViewController.h"
#include <QListWidget>
#include <QFile>
#if QT_VERSION >= 0x040300
  #include <QMdiSubWindow>
#endif

class QString;

/**
	@author Timofey <timoty@pi-balashov>
*/
#if QT_VERSION >= 0x040300
class NVBQtiExportView : public QMdiSubWindow, public NVBViewController
#else
class NVBQtiExportView : public QWidget, public NVBViewController
#endif
{
private:
  QFile * qti_file;
  QListWidget * list;
//   QLineEdit * c_line;

public:
  NVBQtiExportView( NVBWorkingArea * area, QString filename );
  ~NVBQtiExportView();

  virtual NVB::ViewType viewType() { return NVB::ListView; }

  virtual void setSource(NVBDataSource * page, NVBVizUnion = NVBVizUnion()) { addSource(page); }
  virtual void addSource(NVBDataSource * , NVBVizUnion  = NVBVizUnion());

  virtual void setVisualizer(NVBVizUnion ) {;}
  virtual void addControlWidget(QWidget * ) {;}
  virtual void setActiveVisualizer(NVBVizUnion ) {;};

/*
  virtual void addControlWidget(QWidget * widget) =0;
  virtual void setControlWidget(QWidget * widget) =0;
*/

  virtual NVBViewController * openInNewWindow(NVBDataSource * , NVBVizUnion  = NVBVizUnion(), NVB::ViewType  = NVB::DefaultView) { return 0;}

};

#endif
