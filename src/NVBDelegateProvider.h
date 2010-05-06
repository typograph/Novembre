//
// C++ Interface: NVBDelegateProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBDELEGATEPROVIDER_H
#define NVBDELEGATEPROVIDER_H

#include "NVBGeneralDelegate.h"
#include "NVBViewController.h"
#include "NVBPageToolbar.h"
#include <QString>
#include <QtPlugin>

class NVBDelegateProvider {
public :
  virtual ~NVBDelegateProvider() {;}

  virtual quint16 id() = 0;
  virtual bool hasDelegate(quint16 DID) = 0;
  virtual QString cathegory() = 0;
  virtual bool hasSupportForViewType(NVB::ViewType) { return false;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
//  virtual QList<QAction*> actionsForViewType(NVB::ViewType vtype) = 0;
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar) = 0;

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd) = 0;
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
};

Q_DECLARE_INTERFACE(NVBDelegateProvider,"com.novembre.delegateProvider/0.0");

#endif
