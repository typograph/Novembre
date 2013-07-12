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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "Novembre.h"
#include "gui/NVBSettings.h"
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[]) {
	NVBApplication app(argc, argv);

	if (app.otherInstanceIsRunning()) {
		app.passParamsToOtherInstance();
		return 0;
		}

	app.createFactories();

	NVBMain widget;
	widget.setWindowTitle(QString("Novembre"));
	widget.show();

	app.setMainWindow(&widget);

	return app.exec();
	}

/*
So, the working order is as follows:

	1. Main window is loaded. Configuration options are loaded.
	2. If browser window was not closed last time or the user presses the button:
		2a. Main window loads the browser window, directory list and files are loaded
	3. When the user chooses the file, page tree is loaded together with the miniatures.
	3x. if the user chooses another file, the tree is killed and then rebuilt.
	4. When the user chooses the data page the window receives the pointer to the corresponding tree leaf.
	6. When the data window is closed, the modified pages get freed, however the tree doesn't, even after all the data windows are closed. Up to 5 file trees stay in memory.
	7. When the browser is closed, the changed directory tree is written back to the configuration file.
	8. When the main window is closed, all the trees are freed.

*/

NVBApplication::NVBApplication(int & argc, char ** argv)
	: QApplication(argc, argv), confile(QString("%1/.NVB").arg(QDir::homePath()))
	, conf(0)
	, socketBusy(false)
	, firstrun(false) {
#if QT_VERSION >= 0x040400
	setApplicationVersion(NVB_VERSION);
#endif

	setQuitOnLastWindowClosed(true);

#ifdef NVB_ENABLE_LOG
// For threads, we have to do that
	qRegisterMetaType<NVB::LogEntryType>("NVB::LogEntryType");
	NVBLogger * l = new NVBLogger(this);
	setProperty("Logger", QVariant::fromValue(l));
	connect(l, SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)), this, SLOT(message(NVB::LogEntryType, QString, QString)));
#endif
//  connect(l,SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)),qobject_cast<NVBApplication*>(this),SLOT(message(NVB::LogEntryType, QString, QString)));

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

	}

bool NVBApplication::otherInstanceIsRunning() {
	return socketBusy;
	}

void NVBApplication::passParamsToOtherInstance() {
	foreach(QString filename, filesSupplied)
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
		NVBToolsFactory * tf = new NVBToolsFactory();

		if (!tf) NVBCriticalError("Tools factory failed to initialize");

		qApp->setProperty("toolsFactory", QVariant::fromValue(tf));

		NVBFileFactory * ff = new NVBFileFactory();

		if (!ff) NVBCriticalError("Filefactory failed to initialize.");
 
		qApp->setProperty("filesFactory", QVariant::fromValue(ff));

		break;
//	QMessageBox::critical(0,"Plugin error","Errors occured when loading plugins. Re-check plugin path");
//	if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
		}
	}

bool NVBApplication::notify(QObject * receiver, QEvent * event) {
	return QApplication::notify(receiver, event);
	}

#ifdef NVB_ENABLE_LOG
void NVBApplication::message(NVB::LogEntryType type, QString issuer, QString text) {
	if (type == NVB::CriticalErrorEntry)
		QMessageBox::critical(0, issuer, text);
	else if (type == NVB::ErrorEntry)
		qDebug() << issuer << "->" << text;
	else if (type == NVB::DebugEntry)
		qDebug() << issuer << ":" << text;
	}
#endif

void NVBApplication::openFileFrocketData() {
	QByteArray datagram;
	datagram.resize(msgSocket.pendingDatagramSize());
	msgSocket.readDatagram(datagram.data(), datagram.size());
	mainWindow->openFile(datagram);
	}

void NVBApplication::setMainWindow(NVBMain * widget) {
	mainWindow = widget;

	if (widget) {
		foreach(QString filename, filesSupplied)
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
#ifdef NVB_ENABLE_LOG
	delete property("Logger").value<NVBLogger*>();
#endif
	}
