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

#ifndef NVB_H
#define NVB_H

#define NVB_BROWSER_ONLY

#ifdef NVB_DEBUG
#define NVB_VERSION "0.1.0- [DEBUG]"
#else
#define NVB_VERSION "0.1.0-"
#endif

#ifdef NVB_BROWSER_ONLY
#include "NVBBrowser.h"
#else
#include "NVBMain.h"
#include <QtNetwork/QUdpSocket>
#include <QtCore/QStringList>
#endif

#include "NVBCoreApplication.h"

class NVBApplication : public NVBCoreApplication {
		Q_OBJECT
	private:
		QString confile;

		QStringList filesSupplied;
		QUdpSocket msgSocket;
		NVBMain * mainWindow;

		void parseArguments();

		QSettings * conf;

		bool socketBusy;
		bool firstrun;

	public:
		NVBApplication ( int & argc, char ** argv );
		virtual ~NVBApplication();

		bool otherInstanceIsRunning();
		void passParamsToOtherInstance();
		void createFactories();

		void setMainWindow(NVBMain * widget);
		void openFileFrocketData();
	};

#endif
