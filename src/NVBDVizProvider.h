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
		virtual bool hasSupportForViewType(NVB::ViewType) { return true;}
		/**
		  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
		  */
		virtual void populateToolbar(NVB::ViewType, NVBPageToolbar *) {;}

		virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
	};

#endif
