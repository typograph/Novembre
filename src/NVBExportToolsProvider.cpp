//
// C++ Implementation: NVBExportToolsProvider
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBExportToolsProvider.h"
#include "NVBQtiExportView.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include "NVBFile.h"

#include <QMessageBox>

#include "../icons/export.xpm"
#include "../icons/info.xpm"

bool NVBExportToolsProvider::hasDelegate(quint16 DID)
{
  switch(DID) {
    case 0x5064 : // 'Pi' -- page info
    case 0x5078 : // 'Px' -- page ASCII export
    case 0x5171 : // 'Qp' -- QtiPlot export
      return true;
    default : return false;
    }
}

void NVBExportToolsProvider::activateDelegate(quint16 delegateID, NVBDataSource * source, NVBViewController * wnd)
{
  static QString exportPath = QDir::currentPath();

  switch(delegateID) {
    case 0x5078 : { // 'Px'
			QString filename = source->owner->sources().name() + "." + source->name() + ".dat";
			filename = QFileDialog::getSaveFileName ( 0, "Export to ASCII file", exportPath +filename, "Data files (*.dat);;Text files (*.txt);;All files (*)");
      if (filename.isEmpty()) return;
      exportPath = filename.left(filename.lastIndexOf('/')+1);
      QFile f(filename);
      if (f.open(QIODevice::WriteOnly) ) {
        QTextStream t(&f);
				t << "# Page " << source->name() << " from " << source->owner->sources().first() << "\n";
        t << "# First column in units of " << source->xDim().toStr() << ", other columns in units of " << source->zDim().toStr() << "\n";
        if (source->type() == NVB::SpecPage) {
          NVBSpecDataSource * page = (NVBSpecDataSource*)source;
          QList<QwtData*> pagedata = page->getData();
          for(int i = 0; i< page->datasize().width(); i++) {
            t << pagedata.at(0)->x(i);
            foreach( QwtData * d, page->getData()) {
              t << " " << d->y(i);
            }
            t << "\n";
            }
          }
        else if (source->type() == NVB::TopoPage) {
          NVB3DDataSource * page = (NVB3DDataSource*)source;
          t << "# Distance between slices is " << NVBPhysValue(page->position().height()/page->resolution().height(),page->yDim()).toString() << "\n";
          for(int i = 0; i< page->resolution().width(); i++) {
            t << i * page->position().width()/page->resolution().width();
            for(int j = 0; j< page->resolution().height(); j++)
              t << " " << page->getData(j,i);
            t << "\n";
            }
          }
        else
          t << "Unknown page format\n";
        f.close();
      }
      break;
      }
    case 0x5171 : { // 'Qp'
      if (!qtiwnd) {     
				QString filename = source->owner->sources().name() + ".qti";
        filename = QFileDialog::getSaveFileName ( 0, "QtiPlot Project", exportPath +filename, "Qtiplot files (*.qti);;All files (*)");
        if (filename.isEmpty()) return;
        exportPath = filename.left(filename.lastIndexOf('/')+1);

        qtiwnd = new NVBQtiExportView(wnd->area(),filename);
        }

      qtiwnd->addSource(source);
      qtiwnd->raise();
      break;
      }
    case 0x5064 : {
      switch (source->type()) {
        case NVB::TopoPage : {
          NVB3DDataSource * tPage = (NVB3DDataSource *)(source);
          if (!tPage) break;
          QMessageBox::information(0,"Page info",QString("Page name : %1\nPage type : Topography\nData points : %2 x %3\nSize : %4 x %5").arg(tPage->name()).arg(tPage->resolution().width()).arg(tPage->resolution().height()).arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().width()))).toString(0,2,3)).arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().height()))).toString(0,2,3)));
          break;
          }
        case NVB::SpecPage : {
          NVBSpecDataSource * sPage = (NVBSpecDataSource *)(source);
          if (!sPage) break;
          QMessageBox::information(0,"Page info",QString("Page name : %1\nPage type : Spectroscopy\nNumber of curves : %2\nPoints per curve : %3\nX axis span : %4").arg(sPage->name()).arg(sPage->datasize().height()).arg(sPage->datasize().width()).arg(NVBPhysValue(sPage->boundingRect().width(),sPage->xDim()).toString(0,2,3)));
          break;
          }
        default: {
          QMessageBox::information(0,"Page info","Unknown page type");
          break;
          }
        }
      break;
      }
    default : return;
    }
}

void NVBExportToolsProvider::populateToolbar(NVB::ViewType vtype, NVBPageToolbar * toolbar)
{
  Q_UNUSED(vtype);

  QAction * a = new QAction(QIcon(_info_pageinfo),"Page info",0);
  a->setData(((int)id() << 16) + 0x5064);
  toolbar->addActionWithType(a);

  a = new QAction(QIcon(_exp_dat),"Export as ASCII",0);
  a->setData(((int)id() << 16) + 0x5078);
  toolbar->addActionWithType(a);

  a = new QAction(QIcon(_exp_qti),"Export to QtiPlot",0);
  a->setData(((int)id() << 16) + 0x5171);
//  a->setEnabled(false);
  toolbar->addSpecPageAction(a);
}

Q_EXPORT_PLUGIN2(nvbexport, NVBExportToolsProvider)
