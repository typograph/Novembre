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
#ifndef NVB_MAINWINDOW_H
#define NVB_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include "NVBFileInfo.h"

class QAction;
class QMenu;
class QSettings;
class QToolBar;
class NVBFileFactory;
class NVBLogWidgetDock;

class QDragEnterEvent;
class QDropEvent;

class NVBMainWindow: public QMainWindow {
Q_OBJECT
private:		
protected:
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

	QSettings * conf;

	NVBMainWindow();
	virtual ~NVBMainWindow();

public slots:
	void showInfo();

	void openDataSet(NVBDataSet*);
	void openFile(QString);
	void openFile(const NVBAssociatedFilesInfo &);

	void callFileOpenDialog();	
};

#endif
