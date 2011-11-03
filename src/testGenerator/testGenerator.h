#ifndef NVB_TESTGENERATOR_H
#define NVB_TESTGENERATOR_H

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include "../NVBLogger.h"
#include "../NVBCoreApplication.h"

class QTreeWidgetItem;
class NVBFileGenerator;
class QLineEdit;
class QTreeWidget;
class QListView;
class NVBSingleView;
class NVBFile;
class NVBFileInfo;

class NVBTestGenApplication : public NVBCoreApplication {
Q_OBJECT
public:
	NVBTestGenApplication ( int & argc, char ** argv );
	virtual ~NVBTestGenApplication();

	QLineEdit * files;
	NVBSingleView * pview;
	QTreeWidget * tree;
	QListView * view;

	void addFileInfoToTree(QTreeWidgetItem * parent, NVBFileInfo * info);
	void addFileToTree(QTreeWidgetItem * parent, NVBFile * file);
	
public slots:
	void openFile(QString = QString());
	void openFolder(QString);
	void showPage(const QModelIndex & newi);
};


#endif
