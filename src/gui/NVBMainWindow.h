//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVB_MAINWINDOW_H
#define NVB_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include "NVBFileInfo.h"
#include "NVBSettings.h"

class QAction;
class QMenu;
class QToolBar;
class NVBFileFactory;
class NVBLogWidgetDock;

class QDragEnterEvent;
class QDropEvent;

class NVBMainWindow: public QMainWindow {
		Q_OBJECT
	private:
	protected:
		NVBSettings conf;

		QAction * fileOpenAction;
		QAction * fileExitAction;

		QAction * helpAboutAction;
#ifdef NVB_ENABLE_LOG
		QAction * helpLogAction;
#endif

		QMenu *fileMenu, *helpMenu;
		QToolBar *fileTools;

		NVBFileFactory * files;
#ifdef NVB_ENABLE_LOG
		NVBLogWidgetDock * log;
#endif

		QDir fileOpenDir;

		virtual void createMenus();

		virtual void dragEnterEvent ( QDragEnterEvent * event );
		virtual void dropEvent ( QDropEvent * event );

	public:

		NVBMainWindow(NVBSettings settings);
		virtual ~NVBMainWindow();

	public slots:
		void showInfo();

		void openDataSet(NVBDataSet*);
		void openFile(QString);
		void openFile(const NVBAssociatedFilesInfo &);

		void callFileOpenDialog();
		void callSettingsDialog();
	};

#endif
