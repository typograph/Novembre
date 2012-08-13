//
// C++ Interface: NVBSettings
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSETTINGS_H
#define NVBSETTINGS_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>

Q_DECLARE_METATYPE(QSettings*)

class NVBSettings : public QDialog {
Q_OBJECT

private:

  QSettings * conf;

#ifndef NVB_STATIC
  QLineEdit * plgPath;
#endif
  QLineEdit * logFile;

public:

  NVBSettings();
  ~NVBSettings() {;}

public slots:

  virtual void accept();
  virtual void reject();

public:

static int showGeneralSettings();

};

#endif
