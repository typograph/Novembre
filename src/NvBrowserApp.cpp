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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "NvBrowserApp.h"
#include "NVBSettings.h"
#include "NVBBrowser.h"
#include "NVBFileFactory.h"
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include "NVBMainWindow.h"
#include "NVBLogUtils.h"

int main(int argc, char *argv[])
{
	NVBBrowserApplication app(argc, argv);

	NVBMainWindow main;
	main.setCentralWidget(new NVBBrowser());
	main.setWindowTitle( QString("Novembre Browser") );
	main.show();

	return app.exec();
}

NVBBrowserApplication::NVBBrowserApplication( int & argc, char ** argv )
: NVBCoreApplication(argc,argv)
, confile(QString("%1/.NVB.0.1").arg(QDir::homePath()))
{ 

	// Loading objects

	// Configuration

	// TODO system dependent config file

	bool firstrun = !QFile::exists(confile);

	QSettings * conf = new QSettings(confile,QSettings::IniFormat,this);
	setProperty("NVBSettings",QVariant::fromValue(conf));

	// Start logging

#ifdef NVB_ENABLE_LOG
	if (conf->contains("LogFile"))
		while (true) {
			NVBLogFile * lf =	new NVBLogFile(conf->value("LogFile").toString(),this);
			if (!lf) {
				QMessageBox::critical(0,"Log error","Cannot access the logfile. Please check the settings");
				if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
				}
			else
				break;
			}
#endif

#ifdef NVB_STATIC
	if (firstrun)
		NVBSettings::showGeneralSettings();
#else
	if (firstrun || !conf->contains("PluginPath"))
		conf->setValue("PluginPath",NVB_PLUGINS);
	if (!QFile::exists(conf->value("PluginPath").toString()))
		if (NVBSettings::showGeneralSettings() == QDialog::Rejected)
			throw;
#endif

//	while (true) {
#ifndef NVB_STATIC
		setLibraryPaths(QStringList(conf->value("PluginPath").toString()));
#endif
		qApp->setProperty("filesFactory",QVariant::fromValue(new NVBFileFactory()));
//		break;
//		}
//	QMessageBox::critical(0,"Plugin error","Errors occured when loading plugins. Re-check plugin path");
//	if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);

}

/*
void NVBBrowserApplication::setMainWindow(NVBBrowser * widget) {
	mainWindow = widget;
	NVBMainWindow * mw = new QMainWindow(this);
	
	}
*/
NVBBrowserApplication::~ NVBBrowserApplication()
{
	QSettings * conf = property("NVBSettings").value<QSettings*>();
	conf->sync();
	delete conf;
	delete property("filesFactory").value<NVBFileFactory*>();
}
