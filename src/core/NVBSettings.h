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
