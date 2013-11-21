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


#include "NVBPageToolbar.h"

void NVBPageToolbar::fillList() {
	int i = actions().size();

	for (; i > types.size();)
		types << ActionLimits();
	}

void NVBPageToolbar::adapt() {
	fillList();
// 	if (currentViewType == NVB::NoView) {
// 		setVisible(false);
// 		return;
// 		}

//	bool show_toolbar = false;
	QList<QAction*> tb_actions = actions();

	for (int i = 0; i < types.size(); i++)
		if ((
		      types.at(i).viewType == NVB::AllWindows ||	(
		        currentViewType != NVB::NoView && (
		          types.at(i).viewType == NVB::AnyView ||
		          types.at(i).viewType == currentViewType
		        )
		      )
		    ) && (
		      types.at(i).pageType == NVB::UndefinedPage ||
		      types.at(i).pageType == currentPageType
		    )
		   ) {
			tb_actions[i]->setEnabled(true);
//			tb_actions[i]->setVisible(true);
//			show_toolbar = true;
			}
		else {
			tb_actions[i]->setEnabled(false);
//			tb_actions[i]->setVisible(false);
			}

//	setVisible(show_toolbar);
	}

QAction * NVBPageToolbar::addActionWithType(QAction * action, NVB::PageType ptype, NVB::ViewType vtype) {
	fillList();
	types << ActionLimits(ptype, vtype);
	QToolBar::addAction(action);
	return action;
	}

void NVBPageToolbar::switchControlledWindow() {
	}
