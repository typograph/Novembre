//
// C++ Interface: NVBPageInfoWidget
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBPAGEINFOWIDGET_H
#define NVBPAGEINFOWIDGET_H

#include "NVBDataSource.h"
#include <QGridLayout>
#include <QWidget>

class NVBPageInfoWidget : public QWidget {
Q_OBJECT
private:
  QGridLayout * layout;
  void addRow(QString property, QString value);
protected:
  void clear();
public:
  NVBPageInfoWidget(NVBDataSource * page = 0, QWidget * parent = 0);
  ~NVBPageInfoWidget() {;}
public slots:
  void refresh(NVBDataSource * page = 0);
};

#endif
