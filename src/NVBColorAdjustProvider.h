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
#ifndef NVBCOLORADJUSTPROVIDER_H
#define NVBCOLORADJUSTPROVIDER_H

#include "NVBDelegateProvider.h"
#include <QAction>
#include <QString>
#include <QToolBar>

class NVBColorAdjustProvider : public QObject, public NVBDelegateProvider {
		Q_OBJECT
		Q_INTERFACES(NVBDelegateProvider);

	public :
		NVBColorAdjustProvider() {;}
		virtual ~NVBColorAdjustProvider() {;}

		virtual quint16 id() {return 0x4E43;} // 'NC'
		virtual bool hasDelegate(quint16 DID);
		virtual QString cathegory() { return QString("Color control"); }
		virtual bool hasSupportForViewType(NVB::ViewType vtype) {
#ifdef WITH_2DVIEW
			return (vtype == NVB::TwoDView || vtype == NVB::DefaultView);
#else
			return (vtype == NVB::DefaultView);
#endif
			}
		/**
		  * Caution: the function must set action->data() to (providerID << 16 + delegateID)!
		  */
		virtual void populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar);


		virtual void activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd);
//  virtual NVBGeneralDelegate * getPlugin(QString name, NVBWindow * parent) = 0;
//  virtual NVBGeneralDelegate * getPlugin(int i) = 0;
	};

#endif
