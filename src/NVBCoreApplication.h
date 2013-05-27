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

#ifndef NVBCOREAPPLICATION_H
#define NVBCOREAPPLICATION_H

#ifdef NVB_DEBUG
#define NVB_VERSION "0.1.0- [DEBUG]"
#else
#define NVB_VERSION "0.1.0-"
#endif

#include "NVBLogger.h"
#include <QtGui/QApplication>
#include <QtCore/QStringList>

class NVBCoreApplication : public QApplication {
		Q_OBJECT
	public:
		NVBCoreApplication ( int & argc, char ** argv );
		virtual ~NVBCoreApplication();
		//virtual bool notify ( QObject * receiver, QEvent * event ) ;

	protected:
#ifdef NVB_ENABLE_LOG
		bool quiet;
#endif
		QStringList commandLine;
		virtual void processCommandLine();

#ifdef NVB_ENABLE_LOG
	private slots:
		void message(NVB::LogEntryType type, QString issuer, QString text);
#endif
	};

#endif
