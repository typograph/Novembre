#ifndef NVB_TESTGENERATOR_H
#define NVB_TESTGENERATOR_H

#include <QtGui/QApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>
#include "../NVBLogger.h"

class QLineEdit;
class QTreeWidget;
class QListView;

class NVBTestGenApplication : public QApplication {
Q_OBJECT
private:
public:
	NVBTestGenApplication ( int & argc, char ** argv );
	virtual ~NVBTestGenApplication();
	virtual bool notify ( QObject * receiver, QEvent * event ) ;

	QLineEdit * files;
	QTreeWidget * tree;
	QListView * view;

public slots:
	void openFile(QString = QString());
#ifdef NVB_ENABLE_LOG
private slots:
	void message(NVB::LogEntryType type, QString issuer, QString text);
#endif
};


#endif
