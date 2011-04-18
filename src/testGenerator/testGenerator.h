#ifndef NVB_TESTGENERATOR_H
#define NVB_TESTGENERATOR_H

#include <QtGui/QApplication>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include "../NVBLogger.h"

class NVBTestGenApplication : public QApplication {
Q_OBJECT
private:
public:
  NVBTestGenApplication ( int & argc, char ** argv );
  virtual ~NVBTestGenApplication();
  virtual bool notify ( QObject * receiver, QEvent * event ) ;

#ifdef NVB_ENABLE_LOG
private slots:
  void message(NVB::LogEntryType type, QString issuer, QString text);
#endif
};


#endif
