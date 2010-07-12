//
// C++ Implementation: NVBPageToolbar
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBPageToolbar.h"

void NVBPageToolbar::fillList()
{
  int i = actions().size();
	for (;i > types.size();)
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

	for(int i = 0; i < types.size(); i++)
		if ( (
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

QAction * NVBPageToolbar::addActionWithType(QAction * action, NVB::PageType ptype, NVB::ViewType vtype )
{
  fillList();
  types << ActionLimits( ptype, vtype );
  QToolBar::addAction(action);
  return action;
}

void NVBPageToolbar::switchControlledWindow( )
{
}
