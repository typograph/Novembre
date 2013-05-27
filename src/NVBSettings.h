#ifndef NVBSETTINGS_H
#define NVBSETTINGS_H

#include <QtCore/QSettings>

Q_DECLARE_METATYPE(QSettings*)

namespace NVBSettings {

/**
	* @brief Returns application-wide QSettings
	*
	* @return QSettings*
	**/
QSettings * getGlobalSettings();

QString pluginGroup();

}

#endif
