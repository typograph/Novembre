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

#include <QApplication>
#include <QVariant>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

void NVBLogger::outputMessage(NVB::LogEntryType type, QString issuer, QString text) {
  emit message(type,issuer,text,QTime::currentTime());
  }

void NVBOutputMessage(NVB::LogEntryType type,QString issuer, QString text){
  qApp->property("Logger").value<NVBLogger*>()->outputMessage(type,issuer,text);
}

void NVBvOutputMessage(NVB::LogEntryType type, const char *Issuer, const char * Message, va_list ap) {

  char str[1024];
  if (vsnprintf(str,1024,Message,ap) >= 1020) {
    str[1021] = '.';
    str[1022] = '.';
    str[1023] = '.';
    }
  NVBOutputMessage(type, QString(Issuer), QString(str));
}

void NVBCriticalError(const char * Issuer, const char * Message, ...)
{
  va_list ap;
  va_start(ap,Message);
  NVBvOutputMessage(NVB::CriticalErrorEntry,Issuer,Message,ap);
  va_end(ap);
  throw nvberr_no_sense;
}

void NVBCriticalError(QString Issuer, QString Message)
{
  NVBOutputMessage(NVB::CriticalErrorEntry, Issuer, Message);
  throw nvberr_no_sense;
}

void NVBOutputError(const char * Issuer, const char * Message, ...)
{
  va_list ap;
  va_start(ap,Message);
  NVBvOutputMessage(NVB::ErrorEntry,Issuer,Message,ap);
  va_end(ap);
}

void NVBOutputError(QString Issuer, QString Message)
{
  NVBOutputMessage(NVB::ErrorEntry, Issuer, Message);
}

void NVBOutputPMsg(const char * Issuer, const char * Message, ...)
{
  va_list ap;
  va_start(ap,Message);
  NVBvOutputMessage(NVB::PMsgEntry,Issuer,Message,ap);
  va_end(ap);
}

void NVBOutputPMsg(QString Issuer, QString Message)
{
  NVBOutputMessage(NVB::PMsgEntry, Issuer, Message);
}

#ifdef NVB_VERBOSE_LOG
void NVBOutputVPMsg(const char * Issuer, const char * Message, ...)
{
  va_list ap;
  va_start(ap,Message);
  NVBvOutputMessage(NVB::VerboseEntry,Issuer,Message,ap);
  va_end(ap);
}

void NVBOutputVPMsg(QString Issuer, QString Message)
{
  NVBOutputMessage(NVB::VerboseEntry, Issuer, Message);
}
#endif

#ifdef NVB_DEBUG
void NVBOutputDMsg( QString Message )
{
  NVBOutputMessage(NVB::DebugEntry, QString(), Message);
}
#endif

void NVBOutputFileError(QString Issuer, QString filename)
{
  QString Message = QString("File %1 opening failed: ").arg(filename.isEmpty() ? "-unknown-" : filename);

  switch (errno) {
    case EACCES: { Message.append("Permission denied");break;}
    case EBADF: { Message.append("Bad file descriptor");break;}
    case EBUSY: { Message.append("Resourse busy");break;}
    case EIO: { Message.append("I/O error");break;}
    case ENOENT: { Message.append("No such file or directory");break;}
    default: { Message.append("Error #%1").arg(errno);break;}
    }
  NVBOutputMessage(NVB::ErrorEntry, Issuer, Message);
}

#endif
