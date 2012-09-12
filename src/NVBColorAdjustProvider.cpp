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

#include "NVBColorAdjustProvider.h"
#include "NVBContColorScaler.h"
#include "NVBCurveClassifier.h"
#ifdef WITH_2DVIEW
#include "NVBDiscrColorPainter.h"
#endif
#include "NVBSlicePainter.h"

bool NVBColorAdjustProvider::hasDelegate(quint16 DID)
{
  switch(DID) {
    case 0x5441 : // 'TA' /Topography autoscale
#ifdef WITH_2DVIEW
    case 0x5350 : // 'SP' /Spectroscopy paint
#endif
		case 0x5343 : // 'SC' /Spectroscopy classifier
//    case 0x5353 : // 'SS' /Spectroscopy slice
      return true; 
    default : return false;
    }
}

void NVBColorAdjustProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd)
{
   switch(delegateID) {
    case 0x5441 : { // 'TA'
      if (source->type() == NVB::TopoPage) {
        NVBContColorScaler * cscaler = new NVBContColorScaler((NVB3DDataSource*)source);
        wnd->setSource(cscaler->filter());
        wnd->addControlWidget(cscaler);
        }
      break;
      }
#ifdef WITH_2DVIEW
    case 0x5350 : { // 'SP'
      if (source->type() == NVB::SpecPage) {
        NVBDiscrColorPainter * cpainter = new NVBDiscrColorPainter((NVBSpecDataSource*)source,wnd);
        wnd->setSource(cpainter->filter());
        wnd->addControlWidget(cpainter);
        }
      break;
      }
#endif
		 case 0x5343 : { // 'SC'
			 if (source->type() == NVB::SpecPage) {
				 NVBCurveClassifier * classifier = new NVBCurveClassifier((NVBSpecDataSource*)source);
				 wnd->setSource(classifier);
				 wnd->addControlWidget(classifier->widget());
				 }
			 break;
			 }
//    case 0x5353 : { // 'SS'
//      if (source->type() == NVB::SpecPage) {
//        NVBSlicePainter * spainter = new NVBSlicePainter((NVBSpecDataSource*)source,new NVBGrayRampContColorModel(0,1,0,1));
//        wnd->setSource(spainter);
//        wnd->addControlWidget(spainter->widget());
//        }
//      break;
//      }
		default : return;
		}
}

void NVBColorAdjustProvider::populateToolbar(NVB::ViewType /*vtype*/, NVBPageToolbar * toolbar)
{
  QAction * a = NVBContColorScaler::action();
  a->setData(((int)id() << 16) + 0x5441);
  toolbar->addActionWithType(a,NVB::TopoPage,NVB::AnyView);

#ifdef WITH_2DVIEW
  a = NVBDiscrColorPainter::action();
  a->setData(((int)id() << 16) + 0x5350);
  toolbar->addActionWithType(a,NVB::SpecPage,NVB::TwoDView);
#endif

	a = NVBCurveClassifier::action();
	a->setData(((int)id() << 16) + 0x5343);
	toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

//  a = NVBSlicePainter::action();
//  a->setData(((int)id() << 16) + 0x5353);
//  toolbar->addActionWithType(a,NVB::SpecPage,NVB::AnyView);

}

Q_EXPORT_PLUGIN2(nvbcolor, NVBColorAdjustProvider)

