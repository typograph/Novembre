//
// C++ Implementation: mystmfiletoolsfactory
//
// Description:
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBToolsFactory.h"
#include "NVBLogger.h"
#include <QApplication>
#include "NVBFilterDelegate.h"

// Default DPIDs. Module 'ND', plugins '2,3,G,I' + 'T,S'

#ifdef WITH_2DVIEW
  #define DDPID_2T 0x4E443254
  #define DDPID_2S 0x4E443253
#endif
#ifdef WITH_3DVIEW
  #define DDPID_3T 0x4E443354
  #define DDPID_3S 0x4E443353
#endif
#ifdef WITH_GRAPHVIEW
  #define DDPID_GT 0x4E444754
  #define DDPID_GS 0x4E444753
#endif

#define DDPID_IT 0x4E444954
#define DDPID_IS 0x4E444953

Q_IMPORT_PLUGIN(nvbdviz);
// Q_IMPORT_PLUGIN(nvbimport);
#ifdef NVB_STATIC
Q_IMPORT_PLUGIN(nvbtopo);
Q_IMPORT_PLUGIN(nvbspec);
Q_IMPORT_PLUGIN(nvbexport);
Q_IMPORT_PLUGIN(nvbcolor);
#endif

NVBToolsFactory::NVBToolsFactory()
{
  conf = qApp->property("NVBSettings").value<QSettings*>();

  foreach (QObject *plugin, QPluginLoader::staticInstances()) {
    NVBDelegateProvider *dprovider = qobject_cast<NVBDelegateProvider*>(plugin);
    if (dprovider) {
			NVBOutputPMsg("Static plugin loaded");
      plugins.insert(dprovider->id(),dprovider);
      }
    }

#ifndef NVB_STATIC
#ifdef Q_OS_MAC
  QDir dir = QDir( qApp->libraryPaths().at(0), "*.dylib" );
#elif defined Q_WS_WIN
  QDir dir = QDir( qApp->libraryPaths().at(0), "*.dll" );
#else
  QDir dir = QDir( qApp->libraryPaths().at(0), "*.so" );
#endif
  if (!dir.cd ("tools"))
		NVBOutputError(QString("Tool plugins directory %1/tools does not exist").arg(dir.absolutePath()));
  else foreach (QString fileName, dir.entryList(QDir::Files)) {
    QPluginLoader loader(dir.absoluteFilePath(fileName));
		NVBOutputPMsg(QString("Loading plugin %1").arg(fileName));
    NVBDelegateProvider *plugin = qobject_cast<NVBDelegateProvider*>(loader.instance());
    if (plugin) plugins.insert(plugin->id(),plugin);
		else NVBOutputError(loader.errorString());
    }
#endif

  if (!plugins.size())
		NVBCriticalError(QString("No valid plugins found"));

  conf->beginGroup("Tools");

#ifdef WITH_2DVIEW
  if (!conf->contains("Default2DTopoViz")) conf->setValue("Default2DTopoViz",DDPID_2T);
  if (!conf->contains("Default2DSpecViz")) conf->setValue("Default2DSpecViz",DDPID_2S);
  DPID_2T = conf->value("Default2DTopoViz").toInt();
  if (!isPIDValid(DPID_2T)) DPID_2T = DDPID_2T;
  DPID_2S = conf->value("Default2DSpecViz").toInt();
  if (!isPIDValid(DPID_2S)) DPID_2S = DDPID_2S;
#endif

#ifdef WITH_3DVIEW
  if (!conf->contains("Default3DTopoViz")) conf->setValue("Default3DTopoViz",DDPID_3T);
  if (!conf->contains("Default3DSpecViz")) conf->setValue("Default3DSpecViz",DDPID_3S);
  DPID_3T = conf->value("Default3DTopoViz").toInt();
  if (!isPIDValid(DPID_3T)) DPID_3T = DDPID_3T;
  DPID_3S = conf->value("Default3DSpecViz").toInt();
  if (!isPIDValid(DPID_3S)) DPID_3S = DDPID_3S;
#endif

#ifdef WITH_GRAPHVIEW
  if (!conf->contains("DefaultGraphTopoViz")) conf->setValue("DefaultGraphTopoViz",DDPID_GT);
  if (!conf->contains("DefaultGraphSpecViz")) conf->setValue("DefaultGraphSpecViz",DDPID_GS);
  DPID_GT = conf->value("DefaultGraphTopoViz").toInt();
  if (!isPIDValid(DPID_GT)) DPID_GT = DDPID_GT;
  DPID_GS = conf->value("DefaultGraphSpecViz").toInt();
  if (!isPIDValid(DPID_GS)) DPID_GS = DDPID_GS;
#endif 

  if (!conf->contains("DefaultIconTopoViz")) conf->setValue("DefaultIconTopoViz",DDPID_IT);
  if (!conf->contains("DefaultIconSpecViz")) conf->setValue("DefaultIconSpecViz",DDPID_IS);
  DPID_IT = conf->value("DefaultIconTopoViz").toInt();
  if (!isPIDValid(DPID_IT)) DPID_IT = DDPID_IT;
  DPID_IS = conf->value("DefaultIconSpecViz").toInt();
  if (!isPIDValid(DPID_IS)) DPID_IS = DDPID_IS;

  conf->endGroup();
}

NVBToolsFactory::~NVBToolsFactory()
{
  foreach (NVBDelegateProvider * provider, plugins) {
    delete provider;
    }
}

QList<NVBPageToolbar*> NVBToolsFactory::generateToolbars(NVB::ViewType vtype)
{
  QMap<QString,NVBPageToolbar*> tBars;

  NVBPageToolbar * tBar;
  bool addSeparator;
  foreach(NVBDelegateProvider* provider, plugins) {
    addSeparator = false;
    if (provider->hasSupportForViewType(vtype)) {
      if (!tBars.contains(provider->cathegory()))
        tBars.insert(provider->cathegory(),new NVBPageToolbar(provider->cathegory()));
      else addSeparator = true;
      tBar = tBars.value(provider->cathegory());
      if (addSeparator) tBar->addSeparator();
      provider->populateToolbar(vtype,tBar);
      }
    }

  foreach(QString key, tBars.keys()) {
    if (tBars.value(key)->actions().size() == 0)
      tBars.remove(key);
    }

  return tBars.values();
}

void NVBToolsFactory::activateDelegate(quint32 pluginID, NVBDataSource * source, NVBViewController* wnd)
{
  plugins.value((quint16)(pluginID >> 16))->activateDelegate((quint16)pluginID,source,wnd);
}

void NVBToolsFactory::activateDefaultVisualizer(NVBDataSource * source, NVBViewController* wnd)
{
  quint32 DPID = 0;

  switch(source->type()) {
    case NVB::TopoPage : {
      switch (wnd->viewType()) {
#ifdef WITH_2DVIEW
        case NVB::TwoDView : {
          DPID = DPID_2T;
          break;
          }
#endif
#ifdef WITH_3DVIEW
        case NVB::ThreeDView : {
          DPID = DPID_3T;
          break;
          }
#endif
#ifdef WITH_GRAPHVIEW
        case NVB::GraphView : {
          DPID = DPID_GT;
          break;
          }
#endif
        case NVB::IconView : {
          DPID = DPID_IT;
          break;
          }
        default: break;
        }
      break;
      }
    case NVB::SpecPage : {
      switch (wnd->viewType()) {
#ifdef WITH_2DVIEW
        case NVB::TwoDView : {
          DPID = DPID_2S;
          break;
          }
#endif
#ifdef WITH_3DVIEW
        case NVB::ThreeDView : {
          DPID = DPID_3S;
          break;
          }
#endif
#ifdef WITH_GRAPHVIEW
        case NVB::GraphView : {
          DPID = DPID_GS;
          break;
          }
#endif
        case NVB::IconView : {
          DPID = DPID_IS;
          break;
          }
        default: break;
        }
      break;
      }
    default : break;
    }

  if (DPID)
    plugins.value((quint16)(DPID >> 16))->activateDelegate((quint16)DPID,source,wnd);
}

NVB::ViewType NVBToolsFactory::getDefaultPageViewType( NVB::PageType ptype )
{
  switch(ptype) {
    case NVB::TopoPage : 
#ifdef WITH_2DVIEW
      return NVB::TwoDView;
#elif defined WITH_3DVIEW
      return NVB::ThreeDView;
#elif defined WITH_GRAPHVIEW
      return NVB::GraphView;
#else
      return NVB::NoView;
#endif
    case NVB::SpecPage :
#ifdef WITH_GRAPHVIEW
      return NVB::GraphView;
#elif defined WITH_2DVIEW
      return NVB::TwoDView;
#elif defined WITH_3DVIEW
      return NVB::ThreeDView;
#else
      return NVB::NoView;
#endif
    case NVB::InvalidPage :
    default : return NVB::NoView;
    }
}

bool NVBToolsFactory::isPIDValid(quint32 pluginID)
{
  quint16 pID, dID;
  pID = (quint16)(pluginID >> 16);
  dID = (quint16) pluginID;
  if (!plugins.contains(pID)) return false;
  return plugins.value(pID)->hasDelegate(dID);
}


NVBDataSource * NVBToolsFactory::hardlinkDataSource(NVBDataSource * source)
{
  switch (source->type()) {
    case NVB::TopoPage : return new NVB3DHardlinkDelegate((NVB3DDataSource*)source);
    case NVB::SpecPage : return new NVBSpecHardlinkDelegate((NVBSpecDataSource*)source);
    default : return 0;
    }
}
