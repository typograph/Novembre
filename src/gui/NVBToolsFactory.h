//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef NVBTOOLSFACTORY_H
#define NVBTOOLSFACTORY_H

#include <QApplication>
#include "../core/NVBDelegateProvider.h"

#include <QWidget>
#include <QMultiHash>
#include <QMetaType>
#include <QDir>
#include <QPluginLoader>
#include <QAction>
#include "NVBSettings.h"
#include "../core/NVBIconProvider.h"

/**
 *  Aggregates and provides plugins to modify the data (or do whatever you want)
 *  The plugin's capabilities are not limited to modifying data, but include
 *  creating and deleting windows, setting visualizers, adding control vidgets etc.
 *  \sa NVBDelegateProvider
 */
class NVBToolsFactory : public NVBIconProvider {
		Q_OBJECT

		QHash<quint16, NVBDelegateProvider*> plugins;

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
		NVB::ViewType getDefaultPageViewType(NVB::PageType ptype);

		bool isPIDValid(quint32 pluginID);

		// QUICKFIX
		virtual QIcon getDefaultIcon(NVBDataSource * source);

	public slots:
		void activateDelegate(quint32 pluginID, NVBDataSource * source, NVBViewController * wnd);
		void activateDefaultVisualizer(NVBDataSource * source, NVBViewController * wnd);

	};

Q_DECLARE_METATYPE(NVBToolsFactory*);

#endif
