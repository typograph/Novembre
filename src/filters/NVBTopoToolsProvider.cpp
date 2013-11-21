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
#include "NVBTopoToolsProvider.h"
#include "NVBTopoLeveler.h"
#ifdef WITH_2DVIEW
#include "NVBCircAverager.h"
#endif

bool NVBTopoToolsProvider::hasDelegate(quint16 DID) {
	switch (DID) {
		case 0x4C76 : // 'Lv' -- leveling
//    case 0x4078 : // 'Bx' -- boundary extraction
#ifdef WITH_2DVIEW
		case 0x4161 : // 'Ca' -- Circular averaging
#endif
			return true;

		default :
			return false;
		}
	}

void NVBTopoToolsProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd) {
	switch (delegateID) {
		case 0x4C76 : { // 'Lv'
			if (source->type() == NVB::TopoPage) {
				NVBTopoLeveler * filtered;
				filtered = new NVBTopoLeveler((NVB3DDataSource*)source, wnd);
				wnd->setSource(filtered);
				wnd->addControlWidget(filtered->widget());
				}

			break;
			}

#ifdef WITH_2DVIEW

		case 0x4161 : { // 'Ca'
			if (source->type() == NVB::TopoPage) {
				wnd->setActiveVisualizer((new NVBCircAverager((NVB3DDataSource*)source, wnd))->bullseye());
				}

			break;
			}

#endif

		default :
			return;
		}
	}

void NVBTopoToolsProvider::populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar) {
	Q_UNUSED(vtype);
	QAction * a = NVBTopoLeveler::action();
	a->setData(((int)id() << 16) + 0x4C76);
	toolbar->addTopoPageAction(a);

#ifdef WITH_2DVIEW
	a = NVBCircAverager::action();
	a->setData(((int)id() << 16) + 0x4161);
	toolbar->addActionWithType(a, NVB::TopoPage, NVB::TwoDView);
#endif
	}

Q_EXPORT_PLUGIN2(nvbtopo, NVBTopoToolsProvider)
