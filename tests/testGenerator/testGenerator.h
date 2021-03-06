#ifndef NVB_TESTGENERATOR_H
#define NVB_TESTGENERATOR_H

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>
#include "NVBLogger.h"
#include "NVBCoreApplication.h"

#define status_SUCCESS 0
#define status_UNLOADABLE_FILEINFO 1
#define status_UNLOADABLE_FILE 2
#define status_INVALID 3
#define status_TECHFAILURE 4

class NVBFileGenerator;
class NVBFile;
class NVBFileInfo;

class NVBTestGenApplication : public NVBCoreApplication {
Q_OBJECT
public:

	QString filename;
	int pages_loaded_file;
	int pages_loaded_fileinfo;

	NVBTestGenApplication ( int & argc, char ** argv );
	virtual ~NVBTestGenApplication();

public slots:
	void openFile();
	int openFile(QString);
};


#endif
