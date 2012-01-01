#include "NVBSettings.h"
#include <QtGui/QApplication>

QSettings* NVBSettings::getGlobalSettings()
{
	return qApp->property("NVBSettings").value<QSettings*>();
}

QString NVBSettings::pluginGroup()
{
	static QString group("Plugins");
	return group;
}
