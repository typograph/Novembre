//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "NvBrowserApp.h"
#include "NVBSettingsDialog.h"
#include "NVBSettings.h"
#include "NVBBrowser.h"
#include "NVBFileFactory.h"
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include "NVBMainWindow.h"
#include "NVBLogUtils.h"
#include "NVBColorMaps.h" // TODO maybe move default map creation into browser

int main(int argc, char *argv[]) {
	NVBBrowserApplication * app = new NVBBrowserApplication(argc, argv);

	NVBMainWindow * main = new NVBMainWindow(app->mainSettings());
	main->setCentralWidget(new NVBBrowser(app->browserSettings()));
	main->setWindowTitle( QString("Novembre Browser") );
	main->show();

	// If we leave deleting everything to g++,
	// there are segfaults on program exit...

	int res = app->exec();
	delete main;
	delete app;
	return res;
	}

NVBBrowserApplication::NVBBrowserApplication( int & argc, char ** argv )
	: NVBCoreApplication(argc, argv)
	,	config(QString("%1/.NVB.0.1").arg(QDir::homePath())){

	// Loading objects

	// Configuration

	// TODO system dependent config file

	bool firstrun = config.contains("Browser");

//	setProperty("DefaultGradient",QVariant::fromValue((NVBColorMap*)(new NVBRGBRampColorMap(0xFFFF30AD,0xFF15FF33))));
	setProperty("DefaultGradient", QVariant::fromValue((NVBColorMap*)
	            new NVBRGBMixColorMap(
	              new NVBGrayStepColorMap(
	                QList<double>() << 0 << 0.12549 << 0.360784 << 0.596078 << 0.737255 << 1,
	                QList<double>() << 0 << 0.466667 << 0.776471 << 0.933333 << 0.976471 << 0.976471
	              ),
	              new NVBGrayStepColorMap(
	                QList<double>() << 0 << 0.0666667 << 0.184314 << 0.415686 << 0.713725 << 0.909804 << 1,
	                QList<double>() << 0 << 0 << 0.0823529 << 0.376471 << 0.815686 << 0.972549 << 0.976471
	              ),
	              new NVBGrayStepColorMap(
	                QList<double>() << 0 << 0.333333 << 0.627451 << 0.862745 << 1,
	                QList<double>() << 0 << 0 << 0.298039 << 0.835294 << 0.984314
	              )
	            )
	                                                  ));
	// Start logging

#ifdef NVB_ENABLE_LOG

	if (config.contains("LogFile") && !new NVBLogFile(config.value("LogFile").toString(), this)) {
		QMessageBox::critical(0, "Log error", "Cannot access the logfile. Please check the settings");
		NVBSettingsDialog::showGeneralSettings();

		if (config.contains("LogFile") && !new NVBLogFile(config.value("LogFile").toString(), this)) {
			NVBOutputError("Cannot access the logfile. Disabling file logging.");
			config.remove("LogFile"); // This part should not be reachable
			}
		}

#endif

#ifdef NVB_STATIC

	if (firstrun)
		NVBSettingsDialog::showGeneralSettings();

#else

	if (firstrun || !config.contains("PluginPath"))
		config.setValue("PluginPath", NVB_PLUGINS);

	if (!QFile::exists(config.value("PluginPath").toString())) {
		NVBSettingsDialog::showGeneralSettings();

		if (!QFile::exists(config.value("PluginPath").toString()))	throw;
		}

#endif

	NVBSettingsDialog::initGlobalDialog(config);

//	while (true) {
#ifndef NVB_STATIC
	setLibraryPaths(QStringList(config.value("PluginPath").toString()));
#endif
	qApp->setProperty("filesFactory", QVariant::fromValue(new NVBFileFactory(config.group("Plugins"))));

	}

NVBBrowserApplication::~ NVBBrowserApplication() {
	delete property("filesFactory").value<NVBFileFactory*>();
	delete property("DefaultGradient").value<NVBColorMap*>();
	}

NVBSettings NVBBrowserApplication::mainSettings() {
	return NVBSettings(config, "SDI");
}

NVBSettings NVBBrowserApplication::browserSettings() {
	return NVBSettings(config,"Browser");
}
