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

#ifndef NVBSPECTOOLSPROVIDER_H
#define NVBSPECTOOLSPROVIDER_H

#include "NVBDelegateProvider.h"
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBSpecToolsProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

public :
  NVBSpecToolsProvider() {;}
  virtual ~NVBSpecToolsProvider() {;} // TODO check about deleting

  virtual quint16 id() {return 0x4E53;} // 'NS'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Data filters");}
  virtual bool hasSupportForViewType(NVB::ViewType) { return true;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);
//  virtual QList<QAction*> actionsForViewType(NVB::ViewType vtype);

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
};

#endif
