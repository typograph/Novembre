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
#include "NVBMainWindow.h"
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include "NVBFileFactory.h"
#include "NVBMainDocks.h"
#include "NVBMimeData.h"
#include "NVBSettings.h"

#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>

#include <QtCore/QSettings>

#ifndef newIcon
#define newIcon(var,name) \
	QIcon var; \
	var.addPixmap(name ## _16); \
	var.addPixmap(name ## _24); \
	var.addPixmap(name ## _32);
#endif

#include "../icons/mainwindow.xpm"
#include "../icons/novembre_v1.xpm"


NVBMainWindow::NVBMainWindow() : QMainWindow()
{

	setAcceptDrops(true);

	setWindowIcon(QIcon(_main_novembre));
//  newIcon(browsicon,_main_browse);

	// Initialisation

	conf = qApp->property("NVBSettings").value<QSettings*>();
	if (!conf)
		NVBCriticalError("Configuration missing");

#ifdef NVB_ENABLE_LOG
	log = new NVBLogWidgetDock(new NVBLogWidget("Novembre log",this),this);
	addDockWidget(Qt::TopDockWidgetArea,log);
#endif

	files = qApp->property("filesFactory").value<NVBFileFactory*>();
	if (!files)
		NVBCriticalError("File factory not properly initialised");

	resize(conf->value("Size", QSize(800, 600)).toSize());

	fileOpenDir.setPath(QDir::homePath());
	fileOpenDir.setFilter(QDir::Readable & QDir::AllDirs & QDir::Files & QDir::Dirs & QDir::Drives);

	createMenus();
	show();
}

NVBMainWindow::~NVBMainWindow( )
{
	conf->setValue("Size", size());
}

void NVBMainWindow::callFileOpenDialog( )
{
	QFileDialog dialog(this,"Open STM File");
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(files->getDialogFilter());
	dialog.setDirectory(fileOpenDir);

	if (dialog.exec() == QDialog::Accepted) {
		openFile(dialog.selectedFiles().value(0));
	}
}

void NVBMainWindow::createMenus( )
{
	// Menus and toolbars

	menuBar()->clear();

	newIcon(openicon,_main_open);

	fileOpenAction = new QAction(openicon, "&Open", this);
	connect( fileOpenAction, SIGNAL( triggered() ), this, SLOT( callFileOpenDialog() ) );

	fileExitAction = new QAction(/*QIcon(_ICON_fish),*/ "E&xit", this);
	connect( fileExitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

	fileMenu = menuBar()->addMenu(QString("&File"));
	fileMenu->addAction( fileOpenAction );
	fileMenu->addSeparator();
	fileMenu->addAction( fileExitAction );

/*	
	fileTools = addToolBar( QString("File Operations") );
	fileTools->addAction( fileOpenAction );
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

}

void NVBMainWindow::showInfo()
{
	QMessageBox::about(this,"About Novembre","Novembre v."+qApp->property("applicationVersion").toString()+"\n(c) Timofey");
}

void NVBMainWindow::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType()))
		event->acceptProposedAction();
	else if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
	else {
		QMainWindow::dragEnterEvent(event);
		}
}

void NVBMainWindow::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType())) {
		event->acceptProposedAction();
		openDataSet(((NVBDataSourceMimeData*)event->mimeData())->getDataSet());
		}
	else if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
		foreach(QUrl url, event->mimeData()->urls()) {
			openFile(url.toLocalFile());
			}
		}
}

void NVBMainWindow::openDataSet(NVBDataSet * /*dataset*/)
{
//  new NVBFileWindow(this, NVBToolsFactory::hardlinkDataSource(source));
}

void NVBMainWindow::openFile(QString filename)
{
	if (filename.isEmpty()) {
		NVBOutputError("No filename supplied");
		return;
		}
	
	NVBFile * nFile = files->getFile(filename);
	if (!nFile) return;

	// TODO let out a signal or something... The browser should open it
	
	nFile->release();
	
}

void NVBMainWindow::openFile(const NVBAssociatedFilesInfo & info)
{
	NVBFile * nFile = files->getFile(info);
	if (!nFile) return;

	// TODO let out a signal or something... The browser should open it
	
	nFile->release();
}
