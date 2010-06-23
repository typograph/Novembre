//
// C++ Implementation: NVBQtiExportView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBQtiExportView.h"
#include "NVBFile.h"

#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QVBoxLayout>

NVBQtiExportView::NVBQtiExportView( NVBWorkingArea * area, QString filename )
#if QT_VERSION >= 0x040300
  : QMdiSubWindow(area->newWindowParentWidget())
#else
  : QFrame(area->newWindowParentWidget())
#endif
  , NVBViewController(area)
  , qti_file(new QFile(filename,this))
{

  setWindowTitle(QString("QtiPlot project %1").arg(filename));

  list = new QListWidget(this);
//   c_line = new QLineEdit(this);
//   c_line->setReadOnly(true);

  QVBoxLayout * vlayout = new QVBoxLayout(this);
  vlayout->addWidget(list);
//   vlayout->addWidget(c_line);

/*
if (qti_file->exists) {
  qti_file->open(QIODevice::ReadWrite);
  qti_file->seek();
  }
else {*/
  qti_file->open(QIODevice::WriteOnly);
  QTextStream qti_stream(qti_file);
  
  qti_stream << "QtiPlot 0.9.0 project file\n";
  qti_stream << "<scripting-lang>\tmuParser\n";
  qti_stream << "<windows>\t20\n";


//  }

}


NVBQtiExportView::~NVBQtiExportView()
{
  QTextStream qti_stream(qti_file);
  qti_stream << "<log>\n</log>";
  qti_file->close();
  delete qti_file;
}

void NVBQtiExportView::addSource( NVBDataSource * page, NVBVizUnion /* viz */ )
{
	QFileInfo finfo(page->owner()->name());

  QString comment = QInputDialog::getText(this,"Enter page comment",QString());
  
  list->addItem(QString("%1 (%2) [%3]").arg(finfo.fileName(),page->name(),comment));
  
  QTextStream qti_stream(qti_file);

  if (page->type() == NVB::SpecPage) {
  
    NVBSpecDataSource * source = (NVBSpecDataSource*)page;
    QList<QwtData*> data = source->getData();
    QList<QColor> colors = source->colors();
  
  
    int nsame = list->findItems(QString("%1 *").arg(finfo.fileName()),Qt::MatchWildcard).size();
  
    qti_stream << "<table>" << endl << ( (nsame < 2) ? finfo.fileName() :
#if QT_VERSION >= 0x040400
     QString("%1.%2").arg(finfo.fileName()).arg(nsame) ) << "\t" << source->datasize().width() << "\t" << source->datasize().height() + 1 << "\t" << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
#else
     QString("%1.%2").arg(finfo.fileName()).arg(nsame) ) << "\t" << source->datasize().width() << "\t" << source->datasize().height() + 1 << "\t" << QDateTime::currentDateTime().toString(Qt::SystemLocaleDate);
#endif
    qti_stream << endl;
    qti_stream << "geometry\tminimized\t";
    qti_stream << endl;
    
    qti_stream << "header\tBias[X]"; // TODO It has to be X Axis Label...
    for (int i = 0; i < data.size(); i++) {
      qti_stream << "\t" << colors.at(i).name() << "[Y]";
      }
    qti_stream << endl;
    
    qti_stream << "ColWidth\t";
    for (int i = 0; i <= data.size(); i++)
      qti_stream << "100\t";
    qti_stream << endl;

    qti_stream << "<com>\n</com>\n";
    
    qti_stream << "ColType";
    for (int i = 0; i <= data.size(); i++)
      qti_stream << "\t0;0/6";
    qti_stream << endl;
  
    qti_stream << "Comments\t";
    qti_stream << source->tDim().toStr() << "\t";
    for (int i = 0; i <= data.size(); i++)
      qti_stream << source->zDim().toStr() << "\t";
    qti_stream << endl;
    
    qti_stream << "WindowLabel\t" << (comment.isEmpty() ? page->name() : QString("%1 [%2]").arg(comment).arg(page->name())) << "\t2\n";
    qti_stream << "<data>\n";
    
    for (int j = 0; j < source->datasize().width(); j++) {

      qti_stream << j << "\t";
      qti_stream << data.first()->x(j);
      for (int i = 0; i < data.size(); i++)
        qti_stream << "\t" << data.at(i)->y(j);
      qti_stream << endl;
      
      }
    
    qti_stream << "</data>\n</table>";
    qti_stream << endl;

  }
  
  else
    list->item(list->model()->rowCount())->setBackground(QBrush(QColor(0x00ffcccc)));
  
}


