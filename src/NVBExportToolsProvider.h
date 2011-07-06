//
// C++ Interface: NVBExportToolsProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBEXPORTTOOLSPROVIDER_H
#define NVBEXPORTTOOLSPROVIDER_H

#include "NVBDelegateProvider.h"
#include "NVBQtiExportView.h"

#include <QPointer>
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBExportToolsProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

private:
  QPointer<NVBQtiExportView> qtiwnd;

public :
  NVBExportToolsProvider() {;}
  virtual ~NVBExportToolsProvider() {;} // TODO check about deleting

  virtual quint16 id() {return 0x4E45;} // 'NE'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Data export");}
  virtual bool hasSupportForViewType(NVB::ViewType) { return true;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
};

#endif
