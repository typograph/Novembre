//
// C++ Interface: NVBPageInfoView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBPAGEINFOVIEW_H
#define NVBPAGEINFOVIEW_H

#include "NVBPageInfoWidget.h"
#include <QtCore/QModelIndex>

#define PageRole Qt::UserRole

class NVBPageInfoView : public NVBPageInfoWidget {
Q_OBJECT
public:
  NVBPageInfoView(QWidget * parent):NVBPageInfoWidget(0,parent) {;}
  virtual ~NVBPageInfoView() {;}
public slots:
  void clearView() { clear(); }
  void showPage(const QModelIndex & index) {
		refresh(index.data(PageRole).value<NVBDataSet*>());
		}
};

#endif
