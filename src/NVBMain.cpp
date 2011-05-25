/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov                                *
 *                         Timofey.Balashov@pi.uka.de                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "NVBMain.h"

#ifndef newIcon
#define newIcon(var,name) \
  QIcon var; \
  var.addPixmap(name ## _16); \
  var.addPixmap(name ## _24); \
  var.addPixmap(name ## _32);
#endif

#include "../icons/mainwindow.xpm"
//#include "../icons/file_open.xpm"
//#include "../icons/file_save.xpm"
//#include "../icons/fish.xpm"
#include "../icons/novembre_v0.xpm"

class NVBDummyViewController : public NVBViewController {
private:
public:
  NVBDummyViewController( NVBWorkingArea * area ):NVBViewController(area) {;}
//   virtual ~NVBViewController() {;}

  virtual NVB::ViewType viewType() { return NVB::NoView; }

  virtual void setSource(NVBDataSource * , NVBVizUnion ) {;}
  virtual void addSource(NVBDataSource * , NVBVizUnion ) {;}

  virtual void setVisualizer(NVBVizUnion ) {;}
  virtual void addControlWidget(QWidget * ) {;}
  virtual void setActiveVisualizer(NVBVizUnion ) {;}
  virtual NVBViewController * openInNewWindow(NVBDataSource * , NVBVizUnion , NVB::ViewType ) { return 0; }

};


NVBMain::NVBMain():QMainWindow()
{

  setAcceptDrops(true);

  fileBrowser = NULL;
  setWindowIcon(QIcon(_main_novembre));

  // main area
#if QT_VERSION >= 0x040300
  workspace = new QMdiArea(this);
  connect(workspace, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(actualize(QMdiSubWindow*)),Qt::QueuedConnection);
#else
  workspace = new QWorkspace(this);
  connect(workspace, SIGNAL(windowActivated(QWidget*)), this, SLOT(actualize(QWidget*)),Qt::QueuedConnection);
#endif
  workspace->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);  
  workspace->setFocusPolicy(Qt::ClickFocus);

  setCentralWidget(workspace);

  // Initialisation

  conf = qApp->property("NVBSettings").value<QSettings*>();
  if (!conf)
		NVBCriticalError("Configuration missing");

#ifdef NVB_ENABLE_LOG
  log = new NVBLogWidgetDock(new NVBLogWidget("Novembre log",this),this);
  addDockWidget(Qt::TopDockWidgetArea,log);
#endif

#ifdef NVB_NO_FWDOCKS
  pageDock = new NVBAutoDock<false>("Pages",this);
  addDockWidget(Qt::LeftDockWidgetArea,pageDock);
  pageDock->setAllowedAreas(Qt::LeftDockWidgetArea & Qt::RightDockWidgetArea);
  pageDock->hide();

  toolsDock = new NVBAutoDock<true>("Tools",this);
  addDockWidget(Qt::RightDockWidgetArea,toolsDock);
  toolsDock->setAllowedAreas(Qt::LeftDockWidgetArea & Qt::RightDockWidgetArea);
  toolsDock->hide();
#endif

  files = qApp->property("filesFactory").value<NVBFileFactory*>();
  if (!files)
		NVBCriticalError("File factory not properly initialised");

  resize(conf->value("Size", QSize(800, 600)).toSize());

  if (conf->contains("ShowBrowserOnStart")) {
    if (conf->value("ShowBrowserOnStart").toBool())
      callBrowser();
    }
  else {
    conf->setValue("ShowBrowserOnStart",QVariant(false));
    }

	fileOpenDir.setPath(QDir::homePath());
	fileOpenDir.setFilter(QDir::Readable & QDir::AllDirs & QDir::Files & QDir::Dirs & QDir::Drives);

  createMenus();
  show();
	actualize();

/*

  QStatusBar * statusbar = new QStatusBar(this,"status");

*/

}


void NVBMain::callBrowser()
{
  browserForceClosed = false;
  
  if (fileBrowser) {
    fileBrowser->setWindowState(fileBrowser->windowState() & ~Qt::WindowMinimized);
#if QT_VERSION >= 0x040300
    workspace->setActiveSubWindow(fileBrowser);
#else
    workspace->setActiveWindow(fileBrowser);
#endif
    }
  else {
    if (!(fileBrowser = new NVBBrowser(workspace)))
			NVBCriticalError("Browser creation failed");
#if QT_VERSION < 0x040300
    addWindow(fileBrowser);
#endif
    connect(fileBrowser,SIGNAL(closeRequest()),this,SLOT(browserCloseRequest()));
    connect(fileBrowser,SIGNAL(destroyed(QObject*)),this,SLOT(browserDestroyed()));
		connect(fileBrowser,SIGNAL(pageRequest(const NVBAssociatedFilesInfo&, int)),
			this,SLOT(openPage(const NVBAssociatedFilesInfo&, int)));
//    conf->setValue("ShowBrowserOnStart",QVariant(true));
    fileBrowser->resize(conf->value("Browser/Size", QSize(400, 300)).toSize());
		newIcon(browsicon,_main_browse)
		fileBrowser->setWindowIcon(browsicon);
    }
    
  fileBrowser->show();
}

#if QT_VERSION >= 0x040300
void NVBMain::actualize( QMdiSubWindow * window)
#else
void NVBMain::actualize( QWidget * window)
#endif
{
  static QWidget * lastwindow = 0;

	NVBOutputVPMsg(QString("Activated window \"%2\" (%1)").arg((int)window).arg(window ? window->windowTitle() : "No window"));
  if (lastwindow == window) return;

  lastwindow = window;

  NVBViewController * c = dynamic_cast<NVBViewController*>(window);
  if (c)
    emit viewTypeChanged(c->viewType());
  else // This might be a browser
    emit viewTypeChanged(NVB::NoView);

#ifdef NVB_NO_FWDOCKS
  NVBFileWindow * w = qobject_cast<NVBFileWindow*>(window);
  if (w) {
    pageDock->setWidget(w->pageView());
    toolsDock->setWidget(w->toolsView());
    }
  else {
    pageDock->setWidget(0);
    toolsDock->setWidget(0);
    }
#endif
}

void NVBMain::closeBrowser( )
{
  if (fileBrowser) {
    delete fileBrowser;
    browserForceClosed = true;
    }
}

void NVBMain::browserCloseRequest() {
  browserForceClosed = true;
}

void NVBMain::browserDestroyed() {
  fileBrowser = NULL;
  fileBrowseAction->setEnabled(true);
  if (browserForceClosed) 
    conf->setValue("ShowBrowserOnStart",QVariant(false));
}

NVBMain::~NVBMain( )
{
  conf->setValue("Size", size());
  conf->setValue("ShowBrowserOnStart",QVariant(!browserForceClosed));

//  closeBrowser(); // If one does it, the browser doesn't load automatically afterwards
  if (fileBrowser)
    delete fileBrowser;
  delete workspace;
}

void NVBMain::callFileOpenDialog( )
{
	QFileDialog dialog(this,"Open STM File");
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(files->getDialogFilter());
	dialog.setDirectory(fileOpenDir);

	if (dialog.exec() == QDialog::Accepted) {
		openFile(dialog.selectedFiles().value(0));
	}
}

void NVBMain::fileSave( )
{
/*
  NVB_cwindow * c_wnd;
  c_wnd = (NVB_cwindow*)(workspace->activeWindow());
  c_wnd->sync();
  c_wnd->getReferer()->save();
  */
}

void NVBMain::fileExport( )
{
	NVBOutputPMsg("Not implemented yet");
}

void NVBMain::editUndo( )
{
 // ((NVB_cwindow*)(workspace->activeWindow()))->oUndo();
}

void NVBMain::editRedo( )
{
 // ((NVB_cwindow*)(workspace->activeWindow()))->oRedo();
}

void NVBMain::editUndoRaw( )
{
 // ((NVB_cwindow*)(workspace->activeWindow()))->oUndoRaw();
}

void NVBMain::createMenus( )
{
  // Menus and toolbars

  menuBar()->clear();

  // file operations
 
  newIcon(browsicon,_main_browse);

  fileBrowseAction = new QAction(browsicon,"&Browse...", this);
  connect( fileBrowseAction, SIGNAL( triggered() ), this, SLOT( callBrowser() ) );

  newIcon(openicon,_main_open);

  fileOpenAction = new QAction(openicon, "&Open", this);
  connect( fileOpenAction, SIGNAL( triggered() ), this, SLOT( callFileOpenDialog() ) );

/*
  fileSaveAction = new QAction(QIcon(file_save), "&Save", this);
  connect( fileSaveAction, SIGNAL( triggered() ), this, SLOT( fileSave() ) );

  fileExportAction = new QAction(QIcon(_ICON_fish), "&Export", this);
  connect( fileExportAction, SIGNAL( triggered() ), this, SLOT( fileExport() ) );
*/
  fileExitAction = new QAction(/*QIcon(_ICON_fish),*/ "E&xit", this);
  connect( fileExitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

  fileMenu = menuBar()->addMenu(QString("&File"));
  fileMenu->addAction( fileOpenAction );
  fileMenu->addAction( fileBrowseAction );
/*  fileMenu->addSeparator();
  fileMenu->addAction( fileSaveAction );
  fileMenu->addSeparator();
  fileMenu->addAction( fileExportAction ); */
  fileMenu->addSeparator();
  fileMenu->addAction( fileExitAction );

  fileTools = addToolBar( QString("File Operations") );
  fileTools->addAction( fileOpenAction );
  fileTools->addAction( fileBrowseAction );

/*
  fileTools->addSeparator();
  fileTools->addAction( fileExportAction );
  fileTools->addAction( fileSaveAction ); */
//   fileTools->addSeparator();
//   fileTools->addAction( fileExitAction );

  // edit actions

/*
  editUndoRawAction = new QAction(QPixmap(_ICON_fish), "Undo ra&w", this );
  connect( editUndoRawAction, SIGNAL( triggered() ), this, SLOT( editUndoRaw() ) );

  editUndoAction = new QAction(QPixmap(_ICON_fish), "&Undo", this);
  connect( editUndoAction, SIGNAL( triggered() ), this, SLOT( editUndo() ) );

  editRedoAction = new QAction(QPixmap(_ICON_fish), "&Redo", this );
  connect( editRedoAction, SIGNAL( triggered() ), this, SLOT( editRedo() ) );

  editMenu = menuBar()->addMenu( QString("&Edit") );
  editMenu->addAction( editUndoRawAction );
  editMenu->addAction( editUndoAction );
  editMenu->addAction( editRedoAction );

  editTools = addToolBar( QString("Edit") );
  editTools->addAction( editUndoRawAction );
  editTools->addSeparator();
  editTools->addAction( editUndoAction );
  editTools->addAction( editRedoAction );
*/
  // help

#ifdef NVB_ENABLE_LOG
  helpLogAction = new QAction("&Log", this);
  connect( helpLogAction, SIGNAL( triggered() ), log, SLOT( show() ) );
#endif

  helpAboutAction = new QAction("&About Novembre", this);
  connect( helpAboutAction, SIGNAL( triggered() ), this, SLOT( showInfo() ) );

  helpMenu = menuBar()->addMenu( QString("&Help") );
#ifdef NVB_ENABLE_LOG
  helpMenu->addAction( helpLogAction );
  helpMenu->addSeparator();
#endif
  helpMenu->addAction( helpAboutAction );

#ifdef NVB_NO_FWDOCKS
// Dock show

  QToolBar * tBar;
  tBar = addToolBar("List");

  QAction * pageDockAction = tBar->addAction(QIcon(_main_PageList),"Show/hide page list");
  pageDockAction->setCheckable(true);
  connect(pageDockAction,SIGNAL(toggled(bool)),pageDock,SLOT(setVisible(bool)));
  connect(pageDock,SIGNAL(visibilityChanged(bool)),pageDockAction,SLOT(setChecked(bool)));

  QAction * toolsDockAction = tBar->addAction(QIcon(_main_WidgetList),"Show/hide controls list");
  toolsDockAction->setCheckable(true);
  connect(toolsDockAction,SIGNAL(toggled(bool)),toolsDock,SLOT(setVisible(bool)));
  connect(toolsDock,SIGNAL(visibilityChanged(bool)), toolsDockAction,SLOT(setChecked(bool)));
#endif

//  tools

  QList<NVBPageToolbar*> tBars = qApp->property("toolsFactory").value<NVBToolsFactory*>()->generateToolbars(NVB::DefaultView);
  foreach(QToolBar * tBar,tBars) {
    addToolBar(tBar);
    connect(tBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(redirectAction(QAction*)));
    connect(this,SIGNAL(pageTypeChanged(NVB::PageType)),tBar,SLOT(switchPageType(NVB::PageType)));
    connect(this,SIGNAL(viewTypeChanged(NVB::ViewType)),tBar,SLOT(switchViewType(NVB::ViewType)));
    }

}

void NVBMain::showInfo()
{
  QMessageBox::about(this,"About Novembre","Novembre v."+qApp->property("applicationVersion").toString()+"\n(c) Timofey");
}

void NVBMain::addPersistentWindow(QWidget * window)
{
#if QT_VERSION >= 0x040300
  new NVBPersistentMdiSubWindow(window,workspace);
#else
  addWindow(window);
#endif
}

void NVBMain::addWindow(QWidget * window)
{
//  window->setParent(this);
//  window->show();
  NVBFileWindow * w = qobject_cast<NVBFileWindow*>(window);
  if (w) {
    connect(w,SIGNAL(pageSelected(NVB::PageType)),this,SIGNAL(pageTypeChanged(NVB::PageType)));
    }

  if (fileBrowser) fileBrowser->lower();
/*#if QT_VERSION >= 0x040300
  workspace->addSubWindow(window)->raise();
#else
  workspace->addWindow(window)->raise();
#endif*/

  window->show();
  window->raise();
//   window->focus();
}


void NVBMain::dragEnterEvent(QDragEnterEvent * event)
{
  if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType()))
    event->acceptProposedAction();
  else if (event->mimeData()->hasUrls())
    event->acceptProposedAction();
  else {
    QMainWindow::dragEnterEvent(event);
    }
}

void NVBMain::dropEvent(QDropEvent * event)
{
  if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType())) {
    event->acceptProposedAction();
    openPage(((NVBDataSourceMimeData*)event->mimeData())->getPageData());
    }
  else if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
    foreach(QUrl url, event->mimeData()->urls()) {
      openFile(url.toLocalFile());
      }
    }
}

// void NVBMain::dragMoveEvent(QDragMoveEvent * event)
// {
// }
// 
// void NVBMain::dragLeaveEvent(QDragLeaveEvent * event)
// {
// }

void NVBMain::openPage(QString filename, int pagenum)
{
  openFile(filename, QList<int>() << pagenum);
}

void NVBMain::openPage(const NVBAssociatedFilesInfo & info, int pagenum)
{
	openFile(info, QList<int>() << pagenum);
}

void NVBMain::openPage(NVBDataSource * source)
{
  new NVBFileWindow(this, NVBToolsFactory::hardlinkDataSource(source));
}

void NVBMain::openFile(QString filename, QList< int > pages)
{
  if (filename.isEmpty()) {
		NVBOutputError("No filename supplied");
    return;
    }
  
  NVBFile * nFile = files->openFile(filename);
  if (!nFile) return;

  if (pages.isEmpty())
    for(int i=0;i<nFile->rowCount();i++)
      pages << i;

  foreach(int pagenum, pages) {
    NVBFileWindow * nWc = new NVBFileWindow(this, nFile->index(pagenum), nFile);
    if (nWc) addWindow(nWc);
    }
}

void NVBMain::openFile(const NVBAssociatedFilesInfo & info, QList< int > pages)
{
	NVBFile * nFile = files->openFile(info);
	if (!nFile) return;

	if (pages.isEmpty())
		for(int i=0;i<nFile->rowCount();i++)
			pages << i;

	foreach(int pagenum, pages) {
		NVBFileWindow * nWc = new NVBFileWindow(this, nFile->index(pagenum), nFile);
		if (nWc) addWindow(nWc);
		}
}
void NVBMain::redirectAction(QAction * action)
{
#if QT_VERSION >= 0x040300
	NVBFileWindow * w = qobject_cast<NVBFileWindow*>(workspace->currentSubWindow());
#else
	NVBFileWindow * w = qobject_cast<NVBFileWindow*>(workspace->activeWindow());
#endif
	if (w)
		w->installDelegate( action );
	else {
	// We will suppose no plugin can work without a window.
	// One notable exception from before was NVBImport, but it was replaced with NVBAssosciatedFilesInfo
	// Mind you, NVBToolBar disables all actions for NVB::NoView
	/*
    NVBDummyViewController d(this);
    qApp->property("toolsFactory").value<NVBToolsFactory*>()->activateDelegate(action->data().toInt(),0,&d);
	*/
	actualize(0);
		}
}

void NVBMain::actualizeCurrentPage(NVBDataSource * source)
{
  emit pageTypeChanged(source->type());
}

