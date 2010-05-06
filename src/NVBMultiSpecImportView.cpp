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
#include "NVBMultiSpecImportView.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
// #include <QListWidgetItem>
#include <QListView>
#include <QTextStream>
#include "NVBDiscrColoring.h"

#include "NVBFile.h"

QString commonBeginning(const QString s1, const QString s2) {
  int cmnlen = 0;
  int limlen = qMin(s1.length(),s2.length());
  while (s1.at(cmnlen) == s2.at(cmnlen)) {
    cmnlen += 1;
    if (cmnlen == limlen) break;
    }

  return s1.left(cmnlen);
}

class NVBFileListWidgetItem : public QListWidgetItem {
private:
  NVBFile * _file;
public:
  NVBFileListWidgetItem(NVBFile * file, QListWidget * parent = 0):QListWidgetItem(file->filename(),parent),_file(file) {;}

  ~NVBFileListWidgetItem() {
    if (_file) {
      if (_file->inUse())
        NVBOutputError("~NVBFileListWidgetItem","File still in use!");
      delete _file;
      }
    }

//   virtual QVariant data ( int role ) const {
//     if (role == Qt::UserRole)
//       return QVariant::fromValue(_file);
//     else
//       return QListWidgetItem::data(role);
//     }

  NVBFile * file() { return _file;}
};

class NVBExpandableSpecPage : public NVBSpecPage {
public:
  NVBExpandableSpecPage(QString name, QString taxis ):NVBSpecPage() {
    _datasize = QSize(0,0);
    pagename = name.left(name.lastIndexOf(' '));
    zd = NVBDimension(name.mid(name.lastIndexOf('(')+1,name.lastIndexOf(')')-name.lastIndexOf('(')-1));
    xd = NVBDimension("m");
    yd = NVBDimension("m");
    td = NVBDimension(taxis.mid(taxis.lastIndexOf('(')+1,taxis.lastIndexOf(')')-taxis.lastIndexOf('(')-1));
    setComment("X Axis Label",taxis.left(taxis.lastIndexOf(' ')));
    setColorModel(new NVBRandomDiscrColorModel());
    }

  void addNewSpecPoint(double x, double y, QwtData * pdata) {
    if (! _datasize.isNull() && pdata->size() != _datasize.width()) {
      NVBOutputError("NVBExpandableSpecPage","Wrong size of added data: %d when main size is %d", pdata->size(), _datasize.width() );
      return;
      };
    emit dataAboutToBeChanged();
    _positions << QPointF(x,-y);
    _data << pdata;
    _datasize.rheight() += 1;
    _datasize.rwidth() = pdata->size();
    getMinMax();
    emit dataChanged();
    };
};

class NVBSpecCollection : public NVBFile {

public:
  NVBSpecCollection(const QStringList & spec_files)
    : NVBFile(commonBeginning(spec_files.first(),spec_files.last()))
    {
      QList<NVBExpandableSpecPage*> pages;

      {
        QFile f(spec_files.first());
        f.open(QIODevice::ReadOnly);
        QTextStream first(&f);
        while(first.readLine() != "[DATA]") {;}
        QStringList names = first.readLine().split('\t',QString::SkipEmptyParts);
        for(int i = 1; i < names.count(); i++)
          pages << new NVBExpandableSpecPage(names.at(i),names.first());
      }

      foreach(QString filename, spec_files) {
        QFile f(filename);
        f.open(QIODevice::ReadOnly);
        QTextStream specdata(&f);
        specdata.readLine(); // Experiment \t ....
        specdata.readLine(); // Date ...
        specdata.readLine(); // User ...
        QString s = specdata.readLine();
        double xpos = s.mid(s.indexOf('\t')).toDouble();
        s = specdata.readLine();
        double ypos = s.mid(s.indexOf('\t')).toDouble();
        s = specdata.readLine();
        double zpos = s.mid(s.indexOf('\t')).toDouble();
        while(specdata.readLine() != "[DATA]") {;}
        specdata.readLine(); // Column names
        QList<QVector<double> > cdata;

        for(int i = -1; i < pages.count(); i++)
          cdata << QVector<double>();

        while(!specdata.atEnd()) {
          QStringList vdata = specdata.readLine().split('\t',QString::SkipEmptyParts);
          for (int j = 0; j < vdata.size(); j++)
            cdata[j] << vdata.at(j).toDouble();
          }

        for(int i = 0; i < pages.count(); i++)
          pages[i]->addNewSpecPoint(xpos,ypos,new QwtArrayData(cdata.at(0),cdata.at(i+1)));

        // TODO Do something with z;

        }

        foreach(NVBExpandableSpecPage * p, pages)
          addSource(p);
    }
};


//-------------------------------------------


NVBMultiSpecImportView::NVBMultiSpecImportView( NVBWorkingArea * area )
#if QT_VERSION >= 0x040300
  : QMdiSubWindow(area->newWindowParentWidget())
#else
  : QWidget(area->newWindowParentWidget())
#endif
  , NVBViewController(area)
{

  setWindowTitle("Spectra collections");

  QHBoxLayout * l = new QHBoxLayout();

  qobject_cast<QBoxLayout*>(layout())->addLayout(l);

  fileList = new QListWidget(this);
  l->addWidget(fileList);
  fileList->setContextMenuPolicy(Qt::ActionsContextMenu);

  connect(fileList,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(showItem(QListWidgetItem*)));

  QAction * newFileAction = new QAction("Add files",fileList);
  connect(newFileAction,SIGNAL(triggered()),SLOT(addFile()));
  fileList->insertAction(0,newFileAction);

  pageList = new QListView(this);
  l->addWidget(pageList);

//   pageList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  pageList->setViewMode(QListView::IconMode);
  pageList->setMovement(QListView::Static);
  pageList->setResizeMode(QListView::Adjust);
  pageList->setFlow(QListView::LeftToRight);
  pageList->setWordWrap(true);
  pageList->setEditTriggers(QAbstractItemView::NoEditTriggers);
/*  pageList->setGridSize(QSize(iconSize + 20, iconSize + 40));
  pageList->setIconSize(QSize(iconSize, iconSize));*/
  pageList->setGridSize(QSize(84, 104));
  pageList->setIconSize(QSize(64, 64));
  pageList->setWrapping(true);
  pageList->setSelectionMode(QAbstractItemView::SingleSelection);
  pageList->setSelectionBehavior(QAbstractItemView::SelectItems);

  pageList->setDragDropMode(QAbstractItemView::DragOnly);
//   pageList->setModel();

}

NVBMultiSpecImportView::~NVBMultiSpecImportView()
{
 // Close all files, fire up error messages
}

void NVBMultiSpecImportView::addFile()
{
  static QString specPath = QDir::currentPath();

  QStringList files = QFileDialog::getOpenFileNames( this , "Select spectroscopy files to open", specPath, "Data files (*.dat);;All files (*.*)" );

  if (files.isEmpty()) return;
  specPath = QFileInfo(files.first()).canonicalPath();

  files.sort(); // This should take care of shallow copies, among all other things

  new NVBFileListWidgetItem(new NVBSpecCollection(files),fileList);

}

void NVBMultiSpecImportView::closeEvent(QCloseEvent * event)
{
  for(int i = 0; i < fileList->size(); i++)
    if (((NVBFileListWidgetItem*)(fileList->item(i)))->file()->inUse()) {
      event->ignore();
      return;
      }
  event->accept();
}

void NVBMultiSpecImportView::showItem(QListWidgetItem * wt_item)
{
  NVBFileListWidgetItem * item = dynamic_cast<NVBFileListWidgetItem *>(wt_item);
  if (!item) return;
  pageList->setModel(item->file());
}


