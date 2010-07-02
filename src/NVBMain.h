/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov   *
 *   Timofey.Balashov@pi.uka.de   *
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
#ifndef NVB_MAIN_H
#define NVB_MAIN_H

#include "NVBBrowser.h"
#include "NVBFileWindow.h"
#include "NVBSettings.h"

#include "NVBFileFactory.h"
#include "NVBToolsFactory.h"
//#include "NVBPageModel.h"
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QSettings>
#if QT_VERSION >= 0x040300
#include <QMdiArea>
#include <QMdiSubWindow>
#else
#include <QWorkspace>
#endif
#include <QFileDialog>
#include <errno.h>

#include <QUrl>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include "NVBMainDocks.h"

//#include "NVBGlobal.h"

// using namespace NVBErrorCodes;

#if QT_VERSION >= 0x040300
class NVBPersistentMdiSubWindow : public QMdiSubWindow {
Q_OBJECT
public:
  NVBPersistentMdiSubWindow(QWidget * widget, QWidget * parent = 0):QMdiSubWindow(parent) {
    setWidget(widget);
    widget->show();
    show();
    connect(widget,SIGNAL(shown()),SLOT(show()));
//    connect(this,SIGNAL(hidden()),widget,SLOT(hide()));
    }
  virtual ~NVBPersistentMdiSubWindow() {;}
public slots:
  virtual void closeEvent ( QCloseEvent * event ) {
    widget()->close();
    if (widget()->isHidden()) {
      event->accept();
      hide();
      emit hidden();
      }
    }
signals:
  void hidden();
};
#endif

class NVBMain: public QMainWindow, public NVBWorkingArea {
Q_OBJECT
private:
    bool browserForceClosed;
    // actions
    
    QAction * fileBrowseAction;
    QAction * fileOpenAction;
    QAction * fileSaveAction;
    QAction * fileExportAction;
    QAction * fileExitAction;
    
    QAction * editUndoRawAction;
    QAction * editUndoAction;
    QAction * editRedoAction;

    QAction * helpAboutAction;
#ifdef NVB_ENABLE_LOG
    QAction * helpLogAction;
#endif
        
    // interface
        
    NVBBrowser * fileBrowser;
#if QT_VERSION >= 0x040300
    QMdiArea * workspace;
#else
    QWorkspace * workspace;
#endif
    QMenu *fileMenu, *editMenu, *topoMenu, *specMenu, *helpMenu;
    QToolBar *fileTools, *editTools, *topoTools, *specTools;

    NVBFileFactory * files;
#ifdef NVB_ENABLE_LOG
    NVBLogWidgetDock * log;
#endif

#ifdef NVB_NO_FWDOCKS
    NVBAutoDock<false> * pageDock;
    NVBAutoDock<true> * toolsDock;
#endif

	QDir fileOpenDir;

    void createMenus();
protected:

  virtual void dragEnterEvent ( QDragEnterEvent * event );
//   virtual void dragLeaveEvent ( QDragLeaveEvent * event );
//   virtual void dragMoveEvent ( QDragMoveEvent * event );
  virtual void dropEvent ( QDropEvent * event );

public:

    QSettings * conf;

    NVBMain();
    ~NVBMain();

    void addPersistentWindow(QWidget * window);
    void addWindow(QWidget * window);

    virtual QWidget * newWindowParentWidget() const { return workspace; }

public slots:
    void showInfo();

    void openPage(NVBDataSource*);
    void openPage(QString , int);
		void openPage(const NVBAssociatedFilesInfo &, int);
		void openFile(QString , QList<int> = QList<int>());
		void openFile(const NVBAssociatedFilesInfo &, QList<int> = QList<int>());
		void callFileOpenDialog();
    void callBrowser();
    void closeBrowser();
    void browserDestroyed();
    void browserCloseRequest();
    
    void fileSave();
    void fileExport();
    void editUndo();
    void editRedo();
    void editUndoRaw();

    void redirectAction( QAction * );
    void actualizeCurrentPage(NVBDataSource * );

//    virtual void level3Pts();
#if QT_VERSION >= 0x040300
    virtual void actualize(QMdiSubWindow*);
#else
    virtual void actualize(QWidget*);
#endif
    
//    NVB_wincontrol* newWindow(QString filename, unsigned int page);
//    void deleteWindow(NVB_cwindow*);

signals:
  void viewTypeChanged(NVB::ViewType);
  void pageTypeChanged(NVB::PageType);

};

#endif
