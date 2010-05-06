//
// C++ Interface: NVBPageToolbar
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBPAGETOOLBAR_H
#define NVBPAGETOOLBAR_H

#include <QList>
#include <QAction>
#include <QWidget>
#include <QToolBar>
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

struct ActionLimits {
  ActionLimits(NVB::PageType p = NVB::UndefinedPage,
               NVB::ViewType v = NVB::AnyView)
              : pageType(p) , viewType(v) {;}
  NVB::PageType pageType;
  NVB::ViewType viewType;
};

class NVBPageToolbar : public QToolBar {
Q_OBJECT
private:
  QList<ActionLimits> types;
  void fillList();
public:
  inline NVBPageToolbar ( const QString & title, QWidget * parent = 0 ):QToolBar(title,parent) {;}
  inline NVBPageToolbar ( QWidget * parent = 0 ):QToolBar(parent) {;}
  virtual inline ~NVBPageToolbar() {;}
  inline QAction * addTopoPageAction(QAction * action) {
    return addActionWithType(action, NVB::TopoPage); }
  inline QAction * addSpecPageAction(QAction * action) {
    return addActionWithType(action, NVB::SpecPage); }
  QAction * addActionWithType(QAction * action, NVB::PageType ptype = NVB::UndefinedPage, NVB::ViewType vtype = NVB::AnyView );

public slots:
  void switchPageType(NVB::PageType);
  void switchViewType(NVB::ViewType);
  void switchControlledWindow();

};


#endif
