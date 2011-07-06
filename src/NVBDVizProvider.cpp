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

#include "NVBDVizProvider.h"

#ifdef WITH_2DVIEW
  #include "NVB2DVizDelegates.h"
#endif

#ifdef WITH_3DVIEW
  #include "NVB3DVizDelegates.h"
#endif

#ifdef WITH_GRAPHVIEW
  #include "NVBGraphVizDelegates.h"
#endif

#include "NVBIconVizDelegates.h"
#include "NVBTopoCurver.h"

bool NVBDVizProvider::hasDelegate(quint16 DID)
{
  switch(DID) {
#ifdef WITH_2DVIEW
    case 0x3254 : // '2T'
    case 0x3253 : // '2S'
#endif
#ifdef WITH_3DVIEW
    case 0x3354 : // '3T'
//     case 0x3353 : // '3S'
#endif
#ifdef WITH_GRAPHVIEW
    case 0x4754 : // 'GT'
    case 0x4753 : // 'GS'
#endif
    case 0x4954 : // 'IT'
    case 0x4953 : // 'IS'
      return true;
    default : return false;
    }
}

void NVBDVizProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd)
{
   switch(delegateID) {
#ifdef WITH_2DVIEW
    case 0x3254 : { // '2T'
      if (source->type() == NVB::TopoPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::TopoPage,new NVB2DMapVizDelegate(source)));
        }
      break;
      }
    case 0x3253 : { // '2S'
      if (source->type() == NVB::SpecPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::SpecPage,new NVB2DPtsVizDelegate(source)));
        }
      break;
      }
#endif
#ifdef WITH_3DVIEW
    case 0x3354 : { // '3T'
      if (source->type() == NVB::TopoPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::TopoPage,new NVB3DSurfaceVizDelegate(source)));
        }
      break;
      }
#endif
#ifdef WITH_GRAPHVIEW
    case 0x4754 : { // 'GT'
      if (source->type() == NVB::TopoPage) {
        NVBTopoCurver * tc = new NVBTopoCurver((NVB3DDataSource*)source);
        wnd->setSource(tc,NVBVizUnion(NVB::SpecPage,new NVBCurveVizDelegate(tc)));
        wnd->addControlWidget(tc->widget());
        }
      break;
      }
    case 0x4753 : { // 'GS'
      if (source->type() == NVB::SpecPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::SpecPage,new NVBCurveVizDelegate(source)));
        }
      break;
      }
#endif
    case 0x4954 : { // 'IT'
      if (source->type() == NVB::TopoPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::TopoPage,new QIcon( new NVBTopoIconDelegate(source))));
        }
      break;
      }
    case 0x4953 : { // 'IS'
      if (source->type() == NVB::SpecPage) {
        wnd->setVisualizer(NVBVizUnion(NVB::SpecPage,new QIcon( new NVBSpecIconDelegate(source))));
        }
      break;
      }
    default : return;
    }
}

Q_EXPORT_PLUGIN2(nvbdviz, NVBDVizProvider)
