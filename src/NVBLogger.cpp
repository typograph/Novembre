//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include "NVBLogger.h"

#ifdef NVB_ENABLE_LOG

#include <QtGui/QApplication>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QRegExp>

void NVBLogger::outputMessage(NVB::LogEntryType type, QString issuer, QString text) {
	static QRegExp fnct = QRegExp(" ([^ ]*::[^ ]*)\\(");
	QRegExp fnct_copy(fnct); // Thread safety, sort of
	fnct_copy.indexIn(issuer);
	emit message(type, fnct_copy.cap(1), text, QTime::currentTime());
	}

void NVBOutputMessage(NVB::LogEntryType type, QString issuer, QString text) {
	qApp->property("Logger").value<NVBLogger*>()->outputMessage(type, issuer, text);
	}

void NVBOutputFileErrorMessage(QString Issuer, const QFile * file) {
	NVBOutputMessage(NVB::ErrorEntry, Issuer, QString("Opening %1 failed with error : %2").arg(file->fileName()).arg(file->errorString()));
	}

#endif
