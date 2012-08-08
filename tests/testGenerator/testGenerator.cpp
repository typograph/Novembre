#include "testGenerator.h"

#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QTimer>

#include "NVBFileGenerator.h"
#include "NVBFile.h"
#include "NVBforeach.h"

int main(int argc, char** argv) {

	NVBTestGenApplication app(argc,argv);
	
	if (argc > 1)
		app.filename = argv[1];

	QTimer::singleShot(1,&app,SLOT(openFile()));

	return app.exec();

}

NVBTestGenApplication::NVBTestGenApplication( int & argc, char ** argv )
: NVBCoreApplication(argc,argv)
{
}

NVBTestGenApplication::~ NVBTestGenApplication()
{
}

void NVBTestGenApplication::openFile() {
	printf("%s ",filename.toLatin1().constData());
	int r = openFile(filename);
	switch(r) {
		case status_SUCCESS:
			printf("Success (%d/%d pages)\n",pages_loaded_file,pages_loaded_fileinfo);
			break;
		case status_UNLOADABLE_FILEINFO:
			printf("FileInfo loading failed\n");
			break;
		case status_UNLOADABLE_FILE:
			printf("File loading failed (%d pages)\n", pages_loaded_fileinfo);
			break;
		case status_INVALID:
			printf("Invalid parameters\n");
			break;
		case status_TECHFAILURE:
			printf("Technical problems\n");
			break;
		default:
			printf("Unknown error\n");
			break;
		}

	exit(r);
}

int NVBTestGenApplication::openFile(QString name) {

	if (name.isEmpty())
		return status_INVALID;

//	if (QDir(name).exists()) {
//		openFolder(name);
//		return;
//		}

	TESTGENERATOR * generator = new TESTGENERATOR();
	
	if (!generator) return status_TECHFAILURE;

	NVBAssociatedFilesInfo inf = generator->associatedFiles(name);
	
	NVBFileInfo * fi = inf.loadFileInfo();
	if (!fi)
		return status_UNLOADABLE_FILEINFO;
	pages_loaded_fileinfo = fi->count();
	delete fi;
	
	NVBFile * fl = generator->loadFile(inf);
	if (!fl)
		return status_UNLOADABLE_FILE;
	pages_loaded_file = fl->isEmpty() ? 0 : fl->first()->dataSets().count();
	delete fl;

	delete generator;

	return status_SUCCESS;
}
