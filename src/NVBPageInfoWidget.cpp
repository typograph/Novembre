//
// C++ Implementation: NVBPageInfoWidget
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBPageInfoWidget.h"
#include <QLabel>

NVBPageInfoWidget::NVBPageInfoWidget(NVBDataSource * page, QWidget * parent):QWidget(parent)
{
  layout = new QGridLayout(this);
  layout->setColumnStretch(0,1);
  layout->setColumnStretch(1,-1);
  layout->setColumnStretch(2,100);
  setLayout(layout);
  refresh(page);
}

void NVBPageInfoWidget::refresh(NVBDataSource * page)
{
  clear();
  if (page) {
   setWindowTitle(page->name()); // Not really needed
    switch (page->type()) {
      case NVB::TopoPage : {
//        NVB3DDataSource * tPage = qobject_cast<NVB3DDataSource *>(page);
        NVB3DDataSource * tPage = (NVB3DDataSource *)(page);
        if (!tPage) break;
        addRow("Page type","Topography");
        addRow("Data points",QString("%1 x %2").arg(tPage->resolution().width()).arg(tPage->resolution().height()));
// TODO NVBDimension
        addRow("Size",QString("%1 x %2").arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().width()))).toString(0,2,3)).arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().height()))).toString(0,2,3)));
        break;
        }
      case NVB::SpecPage : {
//        NVBSpecDataSource * sPage = qobject_cast<NVBSpecDataSource *>(page);
        NVBSpecDataSource * sPage = (NVBSpecDataSource *)(page);
        if (!sPage) break;
        addRow("Page type","Spectroscopy");
        addRow("Number of curves",QString("%1").arg(sPage->datasize().height()));
        addRow("Points per curve",QString("%1").arg(sPage->datasize().width()));
// TODO NVBDimension
        addRow("X axis span",NVBPhysValue(sPage->boundingRect().width(),sPage->xDim()).toString(0,2,3));
        break;
        }
      default : {
        addRow("Page type","Unknown");
        break;
        }
      }
    foreach(QString key, page->getAllComments().keys()) {
      addRow(key,page->getComment(key));
      }
    }
}

void NVBPageInfoWidget::addRow(QString property, QString value)
{
  int row = layout->count();
  QLabel * l = new QLabel(property);
  l->setAlignment(l->alignment() | Qt::AlignRight);
  layout->addWidget(l,row,0);
  layout->addWidget(new QLabel(":"),row,1);
  QLabel * lv = new QLabel(value);
  lv->setWordWrap(true);
  layout->addWidget(lv,row,2);
  layout->setRowStretch(row,1);
}

void NVBPageInfoWidget::clear()
{
  while (layout->count()) {
    QLayoutItem * i = layout->takeAt(0);
    if (i) {
      if (i->widget()) delete i->widget();
      delete i;
      }
    }
}
