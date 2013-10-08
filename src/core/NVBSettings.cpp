//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBSettings.h"
#include <QtGui/QApplication>
#include <QtCore/QSharedData>
// #include <qvarlengtharray.h>

class NVBSettingsContainer: public QSharedData {
public:
	NVBSettingsContainer(QString filename): settings(filename, QSettings::IniFormat) {;}
	~NVBSettingsContainer() { settings.sync(); }
	
	QSettings settings;
};


NVBSettings::NVBSettings(NVBSettingsContainer * container) : c (container) {

}

NVBSettings::NVBSettings(QString filename) {
	c = new NVBSettingsContainer(filename);
}

NVBSettings::NVBSettings(const NVBSettings& other, QString group)
	: c (other.c)
	, level(other.level)
{
	if (!group.isEmpty())
		level += group + '/';
	updateGroup();
}

NVBSettings::NVBSettings(const NVBSettings* other, QString group) 
	: c (other->c)
	, level(other->level)
{
	if (!group.isEmpty())
		level += group + '/';
	updateGroup();
}

NVBSettings::~NVBSettings() {
	c->settings.sync();
// 	delete c;
}


// QSettings* NVBSettings::getGlobalSettings() {
// 	return qApp->property("NVBSettings").value<QSettings*>();
// 	}
// 
// QString NVBSettings::pluginGroup() {
// 	static QString group("Plugins");
// 	return group;
// 	}

void NVBSettings::updateGroup() {
	if (groups.isEmpty())
		c_group = level;
	else
		c_group = QString("%1%2/").arg(level, groups.join(QChar('/')));
}
	
void NVBSettings::beginGroup(const QString& prefix) {
	groups.append(prefix);
	updateGroup();
}

void NVBSettings::endGroup() {
	groups.removeLast();
	updateGroup();
}

bool NVBSettings::contains(const QString& key) const {
	if (key.isEmpty())
		return false;
	else
		return c->settings.contains(c_group + key);
}

QString NVBSettings::fileName() const {
	return c->settings.fileName();
}
void NVBSettings::remove(const QString& key) {
	if (!key.isEmpty())
		c->settings.remove(c_group + key);
}

void NVBSettings::setValue(const QString& key, const QVariant& value) {
	if (!key.isEmpty())
		c->settings.setValue(c_group + key,value);
}

QVariant NVBSettings::value(const QString& key, const QVariant& defaultValue) const {
	if (key.isEmpty())
		return defaultValue;
	else
		return c->settings.value(c_group + key,defaultValue);
}
