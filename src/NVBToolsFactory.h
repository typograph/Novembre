//
// C++ Interface: mystmfiletoolsfactory
//
// Description:
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBTOOLSFACTORY_H
#define NVBTOOLSFACTORY_H

#include <QApplication>
#include "NVBDelegateProvider.h"

//#include "NVBFileWindow.h"
#include <QWidget>
#include <QMultiHash>
#include <QMetaType>
#include <QDir>
#include <QPluginLoader>
#include <QAction>
//#include <QSettings>
#include "NVBSettings.h"

//class NVBFileWindow;

//class NVBDelegateProvider;
//class NVBToolTypes;

/*
struct pluginID {
  NVBWindow * receiver;
  NVBDelegateProvider * provider;
  int index;
};

pluginID mymakePluginID(NVBWindow * r, NVBDelegateProvider * p, int i);

Q_DECLARE_METATYPE(pluginID);
*/
class NVBToolsFactory : public QObject
{
  Q_OBJECT

  QHash<quint16,NVBDelegateProvider*> plugins; 

#ifdef WITH_2DVIEW
  quint32 DPID_2T, DPID_2S;
#endif
#ifdef WITH_3DVIEW
  quint32 DPID_3T, DPID_3S;
#endif
#ifdef WITH_GRAPHVIEW
  quint32 DPID_GT, DPID_GS;
#endif
  quint32 DPID_IT, DPID_IS;

  QSettings * conf;

public:
  NVBToolsFactory();
  ~NVBToolsFactory();

  QList<NVBPageToolbar*> generateToolbars(NVB::ViewType vtype);
  NVB::ViewType getDefaultPageViewType( NVB::PageType ptype);
/*
  NVBGeneralDelegate * getDefaultDelegate( NVB::PageType page, NVBGeneralDelegate::DelegateViewType view);
  NVBGeneralDelegate * getDummyDelegate(NVBGeneralDelegate::DelegateViewType view);
*/
//  getDefaultPainter * ();
//  QGraphicsItem * getDefault2DPainter(NVBDataSource * source);
//  NVB3DVizDelegate * getDefault3DPainter(NVBDataSource * source);
//  QwtPlotItem * getDefaultSpecPainter(NVBDataSource * source);

//  NVBVizUnion getDefaultVizForViewType(NVBDataSource * source, NVB::ViewType vtype);

  bool isPIDValid( quint32 pluginID );

  static NVBDataSource * hardlinkDataSource(NVBDataSource * source);

public slots:
  void activateDelegate(quint32 pluginID, NVBDataSource * source, NVBViewController * wnd);
  void activateDefaultVisualizer(NVBDataSource * source, NVBViewController * wnd);

protected :
//  void addPlugins(NVBDelegateProvider* provider);
};

Q_DECLARE_METATYPE(NVBToolsFactory*);

#endif
