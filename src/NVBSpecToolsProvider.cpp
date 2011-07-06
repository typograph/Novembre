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

#include "NVBSpecToolsProvider.h"
#include "NVBSpecAverager.h"
// #include "NVBSpecSlicer.h"
#include "NVBSpecSubstractor.h"
#include "NVBSpecExcluder.h"
#include "NVBSpecMath.h"
#include "NVBSpecShift.h"
#include "NVBSpecSmooth.h"

bool NVBSpecToolsProvider::hasDelegate(quint16 DID)
{
  switch(DID) {
    case 0x4176 : // 'Av' -- averager
//    case 0x536C : // 'Sl' -- slicing
    case 0x4273 : // 'Bs' -- background substract
    case 0x4365 : // 'Ce' -- Curve exclusion
    case 0x436D : // 'Cm' -- Curve mathematics
    case 0x4373 : // 'Cs' -- Curve shift
//    case 0x506D : return true; // 'Pm' -- peak matching
    case 0x1111 : // 'Sm' -- Smoothing
      return true;
    default : return false;
    }
}

void NVBSpecToolsProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd)
{
   switch(delegateID) {
    case 0x4176 : { // 'Av'
      if (source->type() == NVB::SpecPage) {
        NVBSpecAverager * filtered = new NVBSpecAverager((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
    case 0x436D : { // 'Cm'
      if (source->type() == NVB::SpecPage) {
        NVBSpecMath * filtered = new NVBSpecMath((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
    case 0x4373 : { // 'Cs'
      if (source->type() == NVB::SpecPage) {
        NVBSpecShift * filtered = new NVBSpecShift((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
      
/*    case 0x536C : { // 'Sl'
      if (source->type() != NVB::SpecPage) break;
      NVBSpecDataSource * page = (NVBSpecDataSource*)source;
      if (wnd->viewType() == NVB::TwoDView) {
        NVBSpecSlicerWidget * topowidget  = new NVBSpecSlicerWidget(page);
        NVBSpecSlicerDelegate * newpage = new NVBSpecSlicerDelegate(page);
        connect(topowidget,SIGNAL(posChanged(double)),newpage,SLOT(setPos(double)));
        wnd->setSource(newpage);
        wnd->addControlWidget(topowidget);
        }
      else if (wnd->viewType() == NVB::GraphView) {
        NVBSpecSlicerWidget * specwidget  = new NVBSpecSlicerWidget(page);
        NVBSpecSlicerPosTracker * postracker = new NVBSpecSlicerPosTracker(page);
        connect(specwidget,SIGNAL(posChanged(double)),postracker,SLOT(setPos(double)));
        connect(postracker,SIGNAL(posChanged(double)),specwidget,SLOT(setPos(double)));
        NVBSpecSlicerWidget * topowidget  = new NVBSpecSlicerWidget(page);
        NVBSpecSlicerDelegate * newpage = new NVBSpecSlicerDelegate(page);
        connect(specwidget,SIGNAL(posChanged(double)),topowidget,SLOT(setPos(double)));
        connect(topowidget,SIGNAL(posChanged(double)),specwidget,SLOT(setPos(double)));
        connect(topowidget,SIGNAL(posChanged(double)),newpage,SLOT(setPos(double)));
        wnd->addControlWidget(specwidget);
        wnd->setActiveVisualizer(NVBVizUnion(NVB::SpecPage,postracker));
        wnd->openInNewWindow(newpage)->addControlWidget(topowidget);
        }
      break;
      }*/
    case 0x4273 : { // 'Bs' -- background substract
      if (source->type() == NVB::SpecPage) {
        NVBSpecSubstractor * filtered = new NVBSpecSubstractor((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
    case 0x4365 : {
      if (source->type() == NVB::SpecPage) {
        NVBSpecExcluder * filtered = new NVBSpecExcluder((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
    case 0x1111 : {
      if (source->type() == NVB::SpecPage) {
        NVBSpecSmoother * filtered = new NVBSpecSmoother((NVBSpecDataSource*)source);
        wnd->setSource(filtered);
        wnd->addControlWidget(filtered->widget());
        }
      break;
      }
    default : return;
    }
}

void NVBSpecToolsProvider::populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar)
{
  Q_UNUSED(vtype);
  QAction * a = NVBSpecAverager::action();
  a->setData(0x4E534176);
  toolbar->addSpecPageAction(a);

  a = NVBSpecMath::action();
  a->setData(0x4E53436D);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

  a = NVBSpecShift::action();
  a->setData(0x4E534373);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

  a = NVBSpecSubstractor::action();
  a->setData(0x4E534273);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

  a = NVBSpecExcluder::action();
  a->setData(0x4E534365);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

  a = NVBSpecSmoother::action();
  a->setData(0x4E531111);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

}

Q_EXPORT_PLUGIN2(nvbspec, NVBSpecToolsProvider)
