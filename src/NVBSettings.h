#ifndef NVBSETTINGS_H
#define NVBSETTINGS_H

#include <QtCore/QSettings>

Q_DECLARE_METATYPE(QSettings*);

QSettings * getGlobalSettings();

#endif
