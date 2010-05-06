//
// C++ Interface: NVBImportToolsProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBIMPORTTOOLSPROVIDER_H
#define NVBIMPORTTOOLSPROVIDER_H

#include "NVBDelegateProvider.h"
#include "NVBMultiSpecImportView.h"

#include <QPointer>
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBImportToolsProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

private:
  QPointer<NVBMultiSpecImportView> specimpwnd;

public :
  NVBImportToolsProvider() {;}
  virtual ~NVBImportToolsProvider() {;} // TODO check about deleting

  virtual quint16 id() {return 0x4E49;} // 'NI'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Data import");}
  virtual bool hasSupportForViewType(NVB::ViewType) { return true;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
};

#endif
