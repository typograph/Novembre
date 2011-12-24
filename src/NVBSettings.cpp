#include "NVBSettings.h"
#include <QtGui/QApplication>

QSettings* getGlobalSettings()
{
	return qApp->property("NVBSettings").value<QSettings*>();
}
