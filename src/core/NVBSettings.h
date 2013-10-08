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

#ifndef NVBSETTINGS_H
#define NVBSETTINGS_H

#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QExplicitlySharedDataPointer>

// Q_DECLARE_METATYPE(QSettings*)

class NVBSettingsContainer;

/**
 * \brief Manage settings of the application
 * 
 * NVBSettings is similar to (and built around) QSettings, and allows keeping
 * QVariant parameters in a tree-like structure in a file. Any subtree can also
 * be represented as NVBSettings, allowing passing settings to classes
 * that needn't be aware of the whole tree.
 * 
 * In contrast to QSettings, NVBSettings does not allow writing arrays.
 *  
 */

class NVBSettings {
private:
	NVBSettings(NVBSettingsContainer * c);
public:
	NVBSettings(QString filename);
// 	NVBSettings(QSettings);
	NVBSettings(const NVBSettings & other, QString group = QString());
	NVBSettings( const NVBSettings* other, QString group = QString() );
	~NVBSettings();
	
	// Service functions
	
/**
	* @brief Returns application-wide QSettings
	*
	* @return QSettings*
	**/
 	static NVBSettings * getGlobalSettings();

	QString pluginGroup();
	
	NVBSettings group(QString path) { return NVBSettings(this,path); }
	
	// "Inherited" functions
	
	QString fileName () const;
	bool contains ( const QString & key ) const;
	QVariant value ( const QString & key, const QVariant & defaultValue = QVariant() ) const;
	void remove ( const QString & key );
	void setValue ( const QString & key, const QVariant & value );
	
	void beginGroup ( const QString & prefix );
	void endGroup ();
	
	QString node() { return level; }
	
private:
	QExplicitlySharedDataPointer<NVBSettingsContainer> c;
	QString level;
	QStringList groups;
	QString c_group;
	void updateGroup();
};

#endif
