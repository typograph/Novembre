#ifndef NVB_TESTGENERATOR_H
#define NVB_TESTGENERATOR_H

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include "core/NVBLogger.h"
#include <QCoreApplication>
#include <QtCore/QSettings>

#define status_SUCCESS 0
#define status_UNLOADABLE_FILEINFO 1
#define status_UNLOADABLE_FILE 2
#define status_INVALID 3
#define status_TECHFAILURE 4

class NVBFileGenerator;
class NVBFile;
class NVBFileInfo;

Q_DECLARE_METATYPE(QSettings*)

class NVBTestGenApplication : public QCoreApplication {
Q_OBJECT
public:

	QStringList filenames;
	int pages_loaded_file;
	int pages_loaded_fileinfo;

	NVBTestGenApplication ( int & argc, char ** argv );
	virtual ~NVBTestGenApplication();

public slots:
	void openFile();
	int openFile(QString, TESTGENERATOR *);

#ifdef NVB_ENABLE_LOG
private slots:
	void message(NVB::LogEntryType type, QString issuer, QString text);
#endif
};


#endif
