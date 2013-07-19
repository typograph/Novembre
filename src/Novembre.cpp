//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#include "Novembre.h"
#include "gui/NVBMain.h"
#include "core/NVBSettings.h"
#include <QtCore/QDir>

int main(int argc, char *argv[]) {
	NVBApplication app(argc, argv);

	if (app.otherInstanceIsRunning()) {
		app.passParamsToOtherInstance();
		return 0;
		}

	app.createFactories();

	NVBMain widget;
	widget.setWindowTitle( QString("Novembre") );
	widget.show();

	app.setMainWindow(&widget);

	return app.exec();
	}

NVBApplication::NVBApplication( int & argc, char ** argv )
	: NVBCoreApplication(argc, argv)
	, confile(QString("%1/.NVB").arg(QDir::homePath()))
	, conf(0)
	, socketBusy(false)
	, firstrun(false) {
// Loading objects
	parseArguments();

	// Configuration

	// TODO system dependent config file

	firstrun = !QFile::exists(confile);

	conf = new QSettings(confile, QSettings::IniFormat, this);
	setProperty("NVBSettings", QVariant::fromValue(conf));

	// Start logging

#ifdef NVB_ENABLE_LOG

	if (conf->contains("LogFile"))
		while (true) {
			NVBLogFile * f = new NVBLogFile(conf->value("LogFile").toString(), this);

			if (!f) {
				QMessageBox::critical(0, "Log error", "Cannot access the logfile. Please check the settings");

				if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
				}
			else break;
			}

#endif

#ifndef NVB_BROWSER_ONLY
	// Check if we are the only instance running

	if (!conf->contains("UDPPort") && !conf->contains("KeepSingle")) {
		conf->setValue("KeepSingle", true);
		int i;

		for (i = 20001; i < 40000; i++)
			if (msgSocket.bind(QHostAddress::LocalHost, i, QUdpSocket::DontShareAddress)) {
				msgSocket.disconnectFromHost();
				break;
				}

		conf->setValue("UDPPort", i);
		}

	socketBusy = !msgSocket.bind(QHostAddress::LocalHost, conf->value("UDPPort").toInt(), QUdpSocket::DontShareAddress);

	if (socketBusy) // We are not the only instance
		NVBOutputPMsg("UDP Port busy. Novembre must be running");
	else {
		NVBOutputPMsg("UDP Port free. Novembre instance starting...");
		connect(&msgSocket, SIGNAL(readyRead()), this, SLOT(openFileFrocketData()));
		}

#endif

	}

bool NVBApplication::otherInstanceIsRunning() {
	return socketBusy;
	}

void NVBApplication::passParamsToOtherInstance() {
	foreach (QString filename, filesSupplied)
	msgSocket.writeDatagram(filename.toLatin1(), QHostAddress::LocalHost, conf->value("UDPPort").toInt());
	// quit(); // OOps, this doesn't work, as we're not in the event loop yet
	}

void NVBApplication::createFactories() {

#ifdef NVB_STATIC

	if (firstrun)
		NVBSettings::showGeneralSettings();

#else

	if (firstrun || !conf->contains("PluginPath"))
		conf->setValue("PluginPath", NVB_PLUGINS);

	if (!QFile::exists(conf->value("PluginPath").toString()))
		if (NVBSettings::showGeneralSettings() == QDialog::Rejected)
			NVBCriticalError("Cannot find plugins - cannot continue");

#endif

	while (true) {
#ifndef NVB_STATIC
		setLibraryPaths(QStringList(conf->value("PluginPath").toString()));
#endif
		NVBFileFactory * ff = new NVBFileFactory();

		if (!ff) NVBCriticalError("Filefactory failed to initialize.");

		qApp->setProperty("filesFactory", QVariant::fromValue(ff));

		NVBToolsFactory * tf = new NVBToolsFactory;

		if (!tf) NVBCriticalError("Tools factory failed to initialize");

		qApp->setProperty("toolsFactory", QVariant::fromValue(tf));

		break;
//	QMessageBox::critical(0,"Plugin error","Errors occured when loading plugins. Re-check plugin path");
//	if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
		}
	}

void NVBApplication::openFileFrocketData() {
	QByteArray datagram;
	datagram.resize(msgSocket.pendingDatagramSize());
	msgSocket.readDatagram(datagram.data(), datagram.size());
	mainWindow->openFile(datagram);
	}

void NVBApplication::setMainWindow(NVBMain * widget) {
	mainWindow = widget;

	if (widget) {
		foreach (QString filename, filesSupplied)
		mainWindow->openFile(filename);
		}
	}

void NVBApplication::parseArguments() {
	QStringList args = arguments();
	args.removeFirst();
	filesSupplied = args;
	}

NVBApplication::~ NVBApplication() {
	msgSocket.disconnectFromHost();
	delete property("filesFactory").value<NVBFileFactory*>();
	delete property("toolsFactory").value<NVBToolsFactory*>();
	QSettings * conf = property("NVBSettings").value<QSettings*>();
	conf->sync();
	delete conf;
	}
