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

#ifndef NVBCOLORADJUSTPROVIDER_H
#define NVBCOLORADJUSTPROVIDER_H

#include "NVBDelegateProvider.h"
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBColorAdjustProvider : public QObject, public NVBDelegateProvider {
Q_OBJECT
Q_INTERFACES(NVBDelegateProvider);

public :
  NVBColorAdjustProvider() {;}
  virtual ~NVBColorAdjustProvider() {;}

  virtual quint16 id() {return 0x4E43;} // 'NC'
  virtual bool hasDelegate(quint16 DID);
  virtual QString cathegory() { return QString("Color control"); }
  virtual bool hasSupportForViewType(NVB::ViewType vtype) {
#ifdef WITH_2DVIEW
    return (vtype == NVB::TwoDView || vtype == NVB::DefaultView);
#else
    return (vtype == NVB::DefaultView);
#endif
    }
/**
  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
  */
  virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);


  virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
};

#endif
