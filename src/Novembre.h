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
#ifndef NVB_H
#define NVB_H

#ifdef NVB_DEBUG
#define NVB_VERSION "0.0.5g [DEBUG]"
#else
#define NVB_VERSION "0.0.5g"
#endif

#include "NVBMain.h"
#include "NVBLogger.h"

#include <QApplication>
#include <QUdpSocket>
#include <QStringList>

// using namespace NVBErrorCodes;

class NVBApplication : public QApplication {
		Q_OBJECT
	private:
		QString confile;
		QStringList filesSupplied;
		QUdpSocket msgSocket;
		NVBMain * mainWindow;
		QSettings * conf;

		bool socketBusy;
		bool firstrun;

		void parseArguments();
	public:
		NVBApplication(int & argc, char ** argv);
		virtual ~NVBApplication();
		virtual bool notify(QObject * receiver, QEvent * event) ;
#if QT_VERSION < 0x040400
		QString applicationVersion() { return NVB_VERSION; }
		Q_PROPERTY(QString applicationVersion READ applicationVersion);
#endif

		bool otherInstanceIsRunning();
		void passParamsToOtherInstance();
		void createFactories();

		void setMainWindow(NVBMain * widget);

#ifdef NVB_ENABLE_LOG
	private slots:
		void message(NVB::LogEntryType type, QString issuer, QString text);
		void openFileFrocketData();
#endif
	};

#endif
