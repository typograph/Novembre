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

#ifndef NVBDVIZPROVIDER_H
#define NVBDVIZPROVIDER_H

#include "NVBDelegateProvider.h"
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBDVizProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

public :
  NVBDVizProvider() {;}
  virtual ~NVBDVizProvider() {;} // TODO check about deleting

  virtual quint16 id() {return 0x4E44;} // 'ND'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Visualizers");}
  virtual bool hasSupportForViewType( NVB::ViewType ) { return true;}
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
  virtual void populateToolbar(NVB::ViewType, NVBPageToolbar *) {;}

  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
};

#endif
