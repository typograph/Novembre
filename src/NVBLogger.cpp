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
#include <QtCore/QMetaObject>

void NVBLogger::outputMessage(NVB::LogEntryType type, QString issuer, QString text) {
//	static QRegExp fnct = QRegExp("^([^ ]* )?([^ ]*)\\(");
	static QRegExp fnct = QRegExp("([^ ]*)\\(");
	QRegExp fnct_copy(fnct); // Thread safety, sort of
	if (fnct_copy.indexIn(issuer) >= 0)
		emit message(type,fnct_copy.cap(1),text,QTime::currentTime());
	else {
		NVBOutputError(QString("Regexp of function failed : investigate '%1'").arg(issuer));
		emit message(type,issuer,text,QTime::currentTime());		
		}
  }

void NVBOutputMessage(NVB::LogEntryType type,QString issuer, QString text){
	NVBLogger * logger = qApp->property("Logger").value<NVBLogger*>();
	if (logger)
	QMetaObject::invokeMethod(
		logger,
		"outputMessage",
		Q_ARG(NVB::LogEntryType,type),
		Q_ARG(QString,issuer),
		Q_ARG(QString,text)
		);
//  ->outputMessage(type,issuer,text);
}

void NVBOutputFileErrorMessage(QString Issuer, const QFile * file) {
	NVBOutputMessage(NVB::ErrorEntry, Issuer, QString("Opening %1 failed with error : %2").arg(file->fileName()).arg(file->errorString()));
}

#endif
