//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBLOGGER_H
#define NVBLOGGER_H

#ifdef NVB_DEBUG
#include <QtCore/QDebug>
#endif

#ifdef NVB_ENABLE_LOG

#include <QtCore/QMetaType>
#include <QtCore/QTime>
#include <QtCore/QString>

// The following piece of code was taken directly from boost/current_function.hpp

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || (defined(__DMC__) && (__DMC__ >= 0x810))

# define NVB_CURRENT_FUNCTION __PRETTY_FUNCTION__

#elif defined(__FUNCSIG__)

# define NVB_CURRENT_FUNCTION __FUNCSIG__

#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))

# define NVB_CURRENT_FUNCTION __FUNCTION__

#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)

# define NVB_CURRENT_FUNCTION __FUNC__

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)

# define NVB_CURRENT_FUNCTION __func__

#else

# define NVB_CURRENT_FUNCTION "(unknown)"

#endif

//--- end boost

#define NVB_ASSERT(cond,what) Q_ASSERT_X(cond,NVB_CURRENT_FUNCTION,what)

class QFile;

namespace NVB {

enum LogEntryType { NoEntry, CriticalErrorEntry, ErrorEntry, PMsgEntry, VerboseEntry, DebugEntry };

}

Q_DECLARE_METATYPE(NVB::LogEntryType);

class NVBLogger : public QObject {
		Q_OBJECT
	public:
		NVBLogger(QObject * parent = 0): QObject(parent) {;}
		virtual ~NVBLogger() {;}
	public slots:
		void outputMessage(NVB::LogEntryType type, QString issuer, QString text);
	signals:
		void message(NVB::LogEntryType type, QString issuer, QString text, QTime time);
	};

Q_DECLARE_METATYPE(NVBLogger*);

void NVBOutputMessage(NVB::LogEntryType type, QString issuer, QString text);
void NVBOutputFileErrorMessage(QString Issuer, const QFile * file);

#define NVBCriticalError(M) do { \
					NVBOutputMessage(NVB::CriticalErrorEntry, NVB_CURRENT_FUNCTION, M); \
					qApp->exit(1); \
					} while(0)

#define NVBOutputError(M) NVBOutputMessage(NVB::ErrorEntry, NVB_CURRENT_FUNCTION, M)
#define NVBOutputPMsg(M)  NVBOutputMessage(NVB::PMsgEntry, NVB_CURRENT_FUNCTION, M)

#ifdef NVB_VERBOSE_LOG
#define NVBOutputVPMsg(M) NVBOutputMessage(NVB::VerboseEntry, NVB_CURRENT_FUNCTION, M)
#else
#define NVBOutputVPMsg(...) (void)0
#endif

#ifdef NVB_DEBUG
#define NVBOutputDMsg(M)  NVBOutputMessage(NVB::DebugEntry, NVB_CURRENT_FUNCTION, M)
#else
#define NVBOutputDMsg(...) (void)0
#endif

#define NVBOutputFileError(F) NVBOutputFileErrorMessage(NVB_CURRENT_FUNCTION, F)

#else

#define NVB_ASSERT(cond,what) Q_ASSERT(cond)
#define NVBCriticalError(...) qApp->exit(1)
#define NVBOutputError(...) (void)0
#define NVBOutputDMsg(...) (void)0
#define NVBOutputPMsg(...) (void)0
#define NVBOutputVPMsg(...) (void)0
#define NVBOutputFileError(...) (void)0

#endif

/*
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
*/
#endif
