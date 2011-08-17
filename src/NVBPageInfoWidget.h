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

#include <QtGui/QWidget>
#include <QtCore/QString>
#include "NVBDataSource.h"

class QGridLayout;

class NVBPageInfoWidget : public QWidget {
Q_OBJECT
private:
  QGridLayout * layout;
  void addRow(QString property, QString value);
	void addComments(NVBDataComments c);
protected:
  void clear();
public:
  NVBPageInfoWidget(NVBDataSet * page = 0, QWidget * parent = 0);
  ~NVBPageInfoWidget() {;}
public slots:
  void refresh(NVBDataSet * page = 0);
};

#endif
