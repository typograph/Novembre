//
// C++ Implementation: NVBLogger
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBLogger.h"

#ifdef NVB_ENABLE_LOG

#include <QtGui/QApplication>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QRegExp>

void NVBLogger::outputMessage(NVB::LogEntryType type, QString issuer, QString text) {
	static QRegExp fnct = QRegExp(" ([^ ]*)\\(");
	fnct.indexIn(issuer);
	emit message(type,fnct.cap(1),text,QTime::currentTime());
  }

void NVBOutputMessage(NVB::LogEntryType type,QString issuer, QString text){
  qApp->property("Logger").value<NVBLogger*>()->outputMessage(type,issuer,text);
}

void NVBOutputFileErrorMessage(QString Issuer, const QFile * file) {
	NVBOutputMessage(NVB::ErrorEntry, Issuer, QString("Opening %1 failed with error : %2").arg(file->fileName()).arg(file->errorString()));
}

#endif
