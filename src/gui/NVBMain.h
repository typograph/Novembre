//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
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
/*!
 * Persistent MdiSubWindow that closes instead of hiding.
 * This class is used by NVBMain to implement addPersistentWindow.
 */
class NVBPersistentMdiSubWindow : public QMdiSubWindow {
		Q_OBJECT
	public:
		/*!
		 * Create a persistent window containing @p widget in @p parent.
		 */
		NVBPersistentMdiSubWindow(QWidget * widget, QWidget * parent = 0): QMdiSubWindow(parent) {
			setWidget(widget);
			widget->show();
			show();
			connect(widget, SIGNAL(shown()), SLOT(show()));
			//    connect(this,SIGNAL(hidden()),widget,SLOT(hide()));
			}
		/// Destroy the persistent window
		virtual ~NVBPersistentMdiSubWindow() {;}
	public slots:
		/// Instead of closing this window will hide
		virtual void closeEvent(QCloseEvent * event) {
			widget()->close();

			if (widget()->isHidden()) {
				event->accept();
				hide();
				emit hidden();
				}
			}
	signals:
		/// Emitted when the closeEvent lead to successful hiding.
		void hidden();
	};
#endif

/*!
 * Main window in Novembre. This window contains all menus and toolbars.
 */
class NVBMain: public QMainWindow, public NVBWorkingArea {
		Q_OBJECT
	private:
		/// Browser was closed by user
		bool browserForceClosed;
		// actions

		QAction * fileBrowseAction;
		QAction * fileOpenAction;
		QAction * fileExitAction;

		QAction * helpAboutAction;
#ifdef NVB_ENABLE_LOG
		QAction * helpLogAction;
#endif

		// interface
		/// The browser object
		NVBBrowser * fileBrowser;
		/// Workspace, containg windows. Class depends on Qt version
#if QT_VERSION >= 0x040300
		QMdiArea * workspace;
#else
		QWorkspace * workspace;
#endif
		QMenu *fileMenu, *editMenu, *topoMenu, *specMenu, *helpMenu, *winMenu;
		QToolBar *fileTools, *editTools, *topoTools, *specTools;

		/// File factory to load files
		NVBFileFactory * files;
#ifdef NVB_ENABLE_LOG
		/// Widget with log events
		NVBLogWidgetDock * log;
#endif

#ifdef NVB_NO_FWDOCKS
		NVBAutoDock<false> * pageDock;
		NVBAutoDock<true> * toolsDock;
#endif
		/// Directory from which the last directly opened file came from.
		QDir fileOpenDir;

		void createMenus();
	protected:

		virtual void dragEnterEvent(QDragEnterEvent * event);
		virtual void dropEvent(QDropEvent * event);

	public:

		QSettings * conf;

		NVBMain();
		~NVBMain();

		void addPersistentWindow(QWidget * window);
		void addWindow(QWidget * window);

		virtual QWidget * newWindowParentWidget() const { return workspace; }

	public slots:
		/// Show QMessageBox with Novembre icon and version information
		void showInfo();

		/// Open @p page in new NVBFileWindow
		void openPage(NVBDataSource* page);
		/// Open page number @p index from file @p filename in new NVBFileWindow
		void openPage(QString filename, int index);
		/// Open page number @p index from data defined by @p info in new NVBFileWindow
		void openPage(const NVBAssociatedFilesInfo & info , int index);
		/// Open a list of pages at @p indexes from file @p filename. By default opens all pages
		void openFile(QString filename, QList<int> indexes = QList<int>());
		/// Open a list of pages at @p indexes from data defined by @p info. By default opens all pages
		void openFile(const NVBAssociatedFilesInfo & info, QList<int> indexes = QList<int>());

		/// Open a file open dialog
		void callFileOpenDialog();
		/// Open browser
		/**
		 * If the browser was not opened yet, it gets created. Otherwise,
		 * we just show it if it's hidden, or raise it if it is not the top window.
		 */
		void callBrowser();
		/*!
		 * Close browser window. The browser is really closed and destroyed.
		 */
		void closeBrowser();

	private slots:
		/// Guard against actual closing of the browser window
		void browserDestroyed();
		/// Called when the user tries to close the browser
		void browserCloseRequest();

		/// When an action is trigged by user, this method redirects the request to the active window
		void redirectAction(QAction *);
		/// When a different window is activated, this method notifies the actions that the page type changed
		void actualizeCurrentPage(NVBDataSource *);

		/// When the list of windows is requested, this method updates the list
		void updateWindowMenu();
		/// Raises the window to the top, activates it and checks if it is visible
		void activateSubWindow();

		/// Gets called when user switched between window, and emits viewTypeChanged.
#if QT_VERSION >= 0x040300
		virtual void actualize(QMdiSubWindow* = 0);
#else
		virtual void actualize(QWidget* = 0);
#endif

	signals:
		/// Emitted when the view type of active window changes
		void viewTypeChanged(NVB::ViewType);
		/// Emitted when the page type in the active window changes (including changes due to switching between windows)
		void pageTypeChanged(NVB::PageType);

	};

#endif
