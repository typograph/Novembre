/***************************************************************************
 *   Copyright (C) 2011 by Timofey Balashov                                *
 *   Timofey.Balashov@pi.uka.de                                            *
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

#include "NVBCoreApplication.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QMessageBox>
#include <QtCore/QVariant>

NVBCoreApplication::NVBCoreApplication( int & argc, char ** argv )
: QApplication(argc,argv)
{
	setApplicationVersion(NVB_VERSION);
	setQuitOnLastWindowClosed(true);
	
#ifdef NVB_ENABLE_LOG
// For threads, we have to do that
	qRegisterMetaType<NVB::LogEntryType>("NVB::LogEntryType");
	NVBLogger * l = new NVBLogger(this);
	setProperty("Logger",QVariant::fromValue(l));
 	connect(l,SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)),this,SLOT(message(NVB::LogEntryType, QString, QString)));
#endif

}

bool NVBCoreApplication::notify( QObject * receiver, QEvent * event )
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
}

#ifdef NVB_ENABLE_LOG
void NVBCoreApplication::message(NVB::LogEntryType type, QString issuer, QString text)
{
  if (type == NVB::CriticalErrorEntry)
    QMessageBox::critical(0,issuer,text);
#ifdef NVB_DEBUG
	else
#else
  else if (type == NVB::DebugEntry)
    qDebug() << issuer << " :: " << text;
  else if (type == NVB::ErrorEntry)
#endif
    qDebug() << issuer << " -> " << text;
}
#endif

NVBCoreApplication::~ NVBCoreApplication()
{
#ifdef NVB_ENABLE_LOG
  delete property("Logger").value<NVBLogger*>();
#endif
}
