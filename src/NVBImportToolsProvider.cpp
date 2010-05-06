//
// C++ Implementation: NVBImportToolsProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBImportToolsProvider.h"
// #include "NVBMultiSpecImportView.h"

#include "../icons/import.xpm"

bool NVBImportToolsProvider::hasDelegate(quint16 DID)
{
  switch(DID) {
    case 0x5364 : // 'Si' -- spectra import
      return true;
    default : return false;
    }
}

void NVBImportToolsProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd)
{
  switch(delegateID) {
    case 0x5364 : { // 'Si'
      if (!specimpwnd)
        specimpwnd = new NVBMultiSpecImportView(wnd->area());

      specimpwnd->raise();
      specimpwnd->show();
      break;
      }
    default : return;
  }
}

void NVBImportToolsProvider::populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar)
{
  Q_UNUSED(vtype);

  QAction * a = new QAction(QIcon(_imp_spec),"Import multiple spectroscopy",0);
  a->setData(((int)id() << 16) + 0x5364);
//  a->setEnabled(false);
  toolbar->addActionWithType(a,NVB::UndefinedPage,NVB::AllWindows);
}

Q_EXPORT_PLUGIN2(nvbimport, NVBImportToolsProvider)
