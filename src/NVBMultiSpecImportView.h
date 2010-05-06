//
// C++ Interface: NVBMultiSpecImportView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBMULTISPECEXPORTVIEW_H
#define NVBMULTISPECEXPORTVIEW_H

#include "NVBViewController.h"
#include <QFile>
#if QT_VERSION >= 0x040300
#include <QMdiSubWindow>
#endif

class QCloseEvent;
class QListWidget;
class QListWidgetItem;
class QListView;

/**
	@author Timofey <timoty@pi-balashov>
*/
#if QT_VERSION >= 0x040300
class NVBMultiSpecImportView : public QMdiSubWindow, public NVBViewController {
#else
class NVBMultiSpecImportView : public QWidget, public NVBViewController {
#endif
Q_OBJECT
private:
  QListWidget * fileList;
  QListView * pageList;

protected:
  virtual void closeEvent ( QCloseEvent * event );

public:
  NVBMultiSpecImportView( NVBWorkingArea * area );
  ~NVBMultiSpecImportView();

  virtual NVB::ViewType viewType() { return NVB::NoView; }

  virtual void setSource(NVBDataSource * , NVBVizUnion = NVBVizUnion()) {;}
  virtual void addSource(NVBDataSource * , NVBVizUnion  = NVBVizUnion()) {;}

  virtual void setVisualizer(NVBVizUnion ) {;}
  virtual void addControlWidget(QWidget * ) {;}
  virtual void setActiveVisualizer(NVBVizUnion ) {;};

  virtual NVBViewController * openInNewWindow(NVBDataSource * , NVBVizUnion  = NVBVizUnion(), NVB::ViewType  = NVB::DefaultView) { return 0;}

private slots:
  void addFile();
  void showItem( QListWidgetItem * item );

};

#endif
