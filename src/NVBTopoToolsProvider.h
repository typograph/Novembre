//
// C++ Interface: NVBTopoToolsProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBTOPOTOOLSPROVIDER_H
#define NVBTOPOTOOLSPROVIDER_H

#include "NVBDelegateProvider.h"
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBTopoToolsProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

public :
  NVBTopoToolsProvider() {;}
  virtual ~NVBTopoToolsProvider() {;} // TODO check about deleting

  virtual quint16 id() {return 0x4E54;} // 'NT'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Data filters");}
  virtual bool hasSupportForViewType(NVB::ViewType) { return true;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
//   virtual QList<QAction*> actionsForViewType(NVB::ViewType vtype);
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
};

#endif
