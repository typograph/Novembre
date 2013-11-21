//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
