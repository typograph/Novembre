//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBDELEGATEPROVIDER_H
#define NVBDELEGATEPROVIDER_H

#include "NVBGeneralDelegate.h"
#include "NVBViewController.h"
#include "NVBPageToolbar.h"
#include <QString>
#include <QtPlugin>

class NVBDelegateProvider {
	public :
		virtual ~NVBDelegateProvider() {;}

		virtual quint16 id() = 0;
		virtual bool hasDelegate(quint16 DID) = 0;
		virtual QString cathegory() = 0;
		virtual bool hasSupportForViewType(NVB::ViewType) { return false;}
		/**
		  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
		  */
//  virtual QList<QAction*> actionsForViewType(NVB::ViewType vtype) = 0;
		virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar) = 0;

		virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd) = 0;
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
	};

Q_DECLARE_INTERFACE(NVBDelegateProvider, "com.novembre.delegateProvider/0.0");

#endif
