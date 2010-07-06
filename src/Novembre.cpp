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

#include "Novembre.h"
#include "NVBSettings.h"
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
  try {
    NVBApplication app(argc, argv);
    
    NVBMain widget;
    widget.setWindowTitle( QString("Novembre") );
    widget.show();
    
    app.setMainWindow(&widget);
    
    return app.exec();
    }
  catch (...) {
    return 1;
    }
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

NVBApplication::NVBApplication( int & argc, char ** argv ):QApplication(argc,argv),confile(QString("%1/.NVB").arg(QDir::homePath()))
{ 
#if QT_VERSION >= 0x040400
  setApplicationVersion(NVB_VERSION);
#endif

  setQuitOnLastWindowClosed(true);

#ifdef NVB_ENABLE_LOG
// For threads, we have to do that
  qRegisterMetaType<NVB::LogEntryType>("NVB::LogEntryType");
  NVBLogger * l = new NVBLogger(this);
  setProperty("Logger",QVariant::fromValue(l));
  connect(l,SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)),this,SLOT(message(NVB::LogEntryType, QString, QString)));
#endif
//  connect(l,SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)),qobject_cast<NVBApplication*>(this),SLOT(message(NVB::LogEntryType, QString, QString)));

  // Loading objects

  parseArguments();

  // Configuration

  // TODO system dependent config file

	bool firstrun = !QFile::exists(confile);

  QSettings * conf = new QSettings(confile,QSettings::IniFormat,this);
  setProperty("NVBSettings",QVariant::fromValue(conf));

  // Start logging

#ifdef NVB_ENABLE_LOG
  if (conf->contains("LogFile"))
    while (true) {
      try {
        new NVBLogFile(conf->value("LogFile").toString(),this);
        break;
        }
      catch (int err) {
				QMessageBox::critical(0,"Log error","Cannot access the logfile. Please check the settings");
				if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
        }
      }
#endif

  // Check if we are the only instance running

  if (!conf->contains("UDPPort") && !conf->contains("KeepSingle")) {
    conf->setValue("KeepSingle",true);
    int i;
    for (i=20001;i<40000;i++)
      if (msgSocket.bind(QHostAddress::LocalHost,i,QUdpSocket::DontShareAddress)) {
        msgSocket.disconnectFromHost();
        break;
      }
    conf->setValue("UDPPort",i);
  }

  if (!msgSocket.bind(QHostAddress::LocalHost,conf->value("UDPPort").toInt(),QUdpSocket::DontShareAddress)) {
    // We are not the only instance
		NVBOutputPMsg("UDP Port busy. Novembre must be running");
    foreach (QString filename, filesSupplied)
      msgSocket.writeDatagram(filename.toLatin1(),QHostAddress::LocalHost,conf->value("UDPPort").toInt());
    quit();
    return;
  }

	NVBOutputPMsg("UDP Port free. Novembre instance starting...");

  connect(&msgSocket,SIGNAL(readyRead()),this,SLOT(openFileFrocketData()));

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

  while (true) {
    try {
#ifndef NVB_STATIC
      setLibraryPaths(QStringList(conf->value("PluginPath").toString()));
#endif
      qApp->setProperty("filesFactory",QVariant::fromValue(new NVBFileFactory()));
      qApp->setProperty("toolsFactory",QVariant::fromValue(new NVBToolsFactory()));
      break;
      }
    catch (int err) {
			QMessageBox::critical(0,"Plugin error","Errors occured when loading plugins. Re-check plugin path");
			if (NVBSettings::showGeneralSettings() == QDialog::Rejected) exit(1);
      }
    }

}

bool NVBApplication::notify( QObject * receiver, QEvent * event )
{
  try {
    return QApplication::notify(receiver,event);
    }
  catch (int err) {
		NVBOutputError(QString("Uncaught error #%1").arg(err));
    return false;
    }
  catch (...) {
		NVBOutputError("Fatal error");
    return false;
    }
/*  catch (...) {
    qDebug() << "NVBApplication::notify" << "->" << "Fatal error. Logger malfunctioning.";
    return false;
    }*/
}

#ifdef NVB_ENABLE_LOG
void NVBApplication::message(NVB::LogEntryType type, QString issuer, QString text)
{
  if (type == NVB::CriticalErrorEntry)
    QMessageBox::critical(0,issuer,text);
  else if (type == NVB::ErrorEntry)
    qDebug() << issuer << "->" << text;
  else if (type == NVB::DebugEntry)
    qDebug() << text;
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
    foreach (QString filename, filesSupplied)
      mainWindow->openFile(filename);
    }
  }

void NVBApplication::parseArguments() {
  QStringList args = arguments();
  args.removeFirst();
  filesSupplied = args;
}

NVBApplication::~ NVBApplication()
{
  msgSocket.disconnectFromHost();
  QSettings * conf = property("NVBSettings").value<QSettings*>();
  conf->sync();
  delete conf;
  delete property("filesFactory").value<NVBFileFactory*>();
  delete property("toolsFactory").value<NVBToolsFactory*>();
#ifdef NVB_ENABLE_LOG
  delete property("Logger").value<NVBLogger*>();
#endif
}
