//
// C++ Interface: NVBLogger
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBLOGGER_H
#define NVBLOGGER_H

#ifdef NVB_ENABLE_LOG

#include <QMetaType>
#include <QTime>
#include <QString>

namespace NVB {

enum LogEntryType { NoEntry, CriticalErrorEntry, ErrorEntry, PMsgEntry, VerboseEntry, DebugEntry };

}

Q_DECLARE_METATYPE(NVB::LogEntryType);

class NVBLogger : public QObject {
Q_OBJECT
public:
  NVBLogger(QObject * parent = 0):QObject(parent) {;}
  virtual ~NVBLogger() {;}
public slots:
  void outputMessage(NVB::LogEntryType type, QString issuer, QString text);
signals:
  void message(NVB::LogEntryType type, QString issuer, QString text, QTime time);
};

Q_DECLARE_METATYPE(NVBLogger*);

void NVBCriticalError( const char* Issuer, const char* Message, ...);
void NVBCriticalError( QString Issuer, QString Message );

void NVBOutputError( const char* Issuer, const char* Message, ...);
void NVBOutputError( QString Issuer, QString Message );

void NVBOutputPMsg(const char* Issuer, const char* Message, ...);
void NVBOutputPMsg( QString Issuer, QString Message );

#ifdef NVB_VERBOSE_LOG
void NVBOutputVPMsg(const char* Issuer, const char* Message, ...);
void NVBOutputVPMsg( QString Issuer, QString Message );
#else
#define NVBOutputVPMsg(...) (void)0
#endif

#ifdef NVB_DEBUG
void NVBOutputDMsg( QString Message );
#else
#define NVBOutputDMsg(...) (void)0
#endif

void NVBOutputFileError( QString Issuer, QString Message );

#else

#define NVBCriticalError(...) throw nvberr_no_sense;
#define NVBOutputError(...) (void)0
#define NVBOutputDMsg(...) (void)0
#define NVBOutputPMsg(...) (void)0
#define NVBOutputVPMsg(...) (void)0
#define NVBOutputFileError(...) (void)0

#endif


namespace NVBErrorCodes {

// Error by malloc etc -- not enough memory
#define nvberr_not_enough_memory (int)1

// Error by switch cases -- no such known value
#define nvberr_unexpected_value (int)3

// Error by parameters transfer -- this function will not accept that
#define nvberr_invalid_input (int)4

// Error by file opening
#define nvberr_file_error (int)5

// Fatal error : program cannot continue
#define nvberr_no_sense (int)6

// This piece of code cannot be reached!
#define nvberr_confused (int)7

// The parameter exists, but has invalid format
#define nvberr_invalid_format (int)8

// The plugin called returned NULL, but didn't throw
#define nvberr_plugin_failure (int)9

}

#endif
