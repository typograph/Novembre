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

#include <QWidget>
#include <QMultiHash>
#include <QMetaType>
#include <QDir>
#include <QPluginLoader>
#include <QAction>
#include "NVBSettings.h"

/**
 *  Aggregates and provides plugins to modify the data (or do whatever you want)
 *  The plugin's capabilities are not limited to modifying data, but include
 *  creating and deleting windows, setting visualizers, adding control vidgets etc. 
 *  \sa NVBDelegateProvider
 */
class NVBToolsFactory : public QObject {
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

	bool isPIDValid( quint32 pluginID );

	/// Create a stable link to a \a source, that will not be affected by masking the source through filters
	static NVBDataSource * hardlinkDataSource(NVBDataSource * source);

	// QUICKFIX
	QIcon getDefaultIcon(NVBDataSource * source);

public slots:
	void activateDelegate(quint32 pluginID, NVBDataSource * source, NVBViewController * wnd);
	void activateDefaultVisualizer(NVBDataSource * source, NVBViewController * wnd);

};

Q_DECLARE_METATYPE(NVBToolsFactory*);

#endif
