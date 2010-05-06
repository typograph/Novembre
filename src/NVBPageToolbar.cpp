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
  for (;i > types.size();i--)
    types << ActionLimits();
}

QAction * NVBPageToolbar::addActionWithType(QAction * action, NVB::PageType ptype, NVB::ViewType vtype )
{
  fillList();
  types << ActionLimits( ptype, vtype );
  QToolBar::addAction(action);
  return action;
}

void NVBPageToolbar::switchPageType(NVB::PageType ptype)
{
  fillList();
  QList<QAction*> tb_actions = actions();
  for(int i = 0; i < types.size(); i++)
    if (types.at(i).pageType != NVB::UndefinedPage) {
      if (types.at(i).pageType == ptype)
        tb_actions[i]->setEnabled(true);
//        tb_actions[i]->setVisible(true);
      else
        tb_actions[i]->setEnabled(false);
//         tb_actions[i]->setVisible(false);
      }
}

void NVBPageToolbar::switchViewType( NVB::ViewType vtype)
{
  fillList();
  bool show_toolbar = false;
  QList<QAction*> tb_actions = actions();
  for(int i = 0; i < types.size(); i++)
    if (types.at(i).viewType == NVB::AllWindows || (vtype != NVB::NoView && (types.at(i).viewType == NVB::AnyView || types.at(i).viewType == vtype))) {
      tb_actions[i]->setEnabled(true);
      tb_actions[i]->setVisible(true);
      show_toolbar = true;
      }
    else {
      tb_actions[i]->setEnabled(false);
      tb_actions[i]->setVisible(false);
      }

  setVisible(show_toolbar);
}

void NVBPageToolbar::switchControlledWindow( )
{
}
