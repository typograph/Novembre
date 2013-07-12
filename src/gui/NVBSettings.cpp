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
#include "../core/NVBLogger.h"
#include "NVBSettings.h"
#include <QGridLayout>
#include <QString>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMetaType>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

NVBSettings::NVBSettings(): QDialog() {
	conf = qApp->property("NVBSettings").value<QSettings*>();

	if (!conf)
		NVBCriticalError("Configuration missing");

	QGridLayout * l = new QGridLayout(this);

	setLayout(l);

#ifndef NVB_STATIC
	l->addWidget(new QLabel("The plugin path defines where Novembre is looking for plugins. Novembre will not work without plugins, so this is a very important field.", this), 0, 0, 1, -1);
	l->addWidget(new QLabel("Plugin path.", this), 1, 0);
	l->addWidget(plgPath = new QLineEdit(conf->value("PluginPath", QString()).toString(), this), 1, 1);
#endif

	l->addWidget(new QLabel("If you want to keep record of application messages, select a location for the log file.", this), 2, 0, 1, -1);
	l->addWidget(new QLabel("Logfile.", this), 3, 0);
	l->addWidget(logFile = new QLineEdit(conf->value("LogFile", QString()).toString(), this)
	, 3, 1);

	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	l->addWidget(buttonBox, 4, 0, 1, -1, Qt::AlignRight);
	}

void NVBSettings::reject() {
	QDialog::reject();
	}

void NVBSettings::accept() {
#ifndef NVB_STATIC

	if (!QFile::exists(plgPath->text())) {
		QMessageBox::critical(0, "Settings", "Specified plugin path does not exist");
		return;
		}

#endif

	if (!logFile->text().isEmpty()) {
		QFileInfo f(logFile->text());

		if (f.exists()) {
			if (!f.isFile()) {
				QMessageBox::critical(0, "Settings", "Specified log file is not a file");
				return;
				}

			if (!f.isWritable()) {
				QMessageBox::critical(0, "Settings", "You do not have permissions to write to the specified log file");
				return;
				}
			}
		else {
			QFileInfo p(f.dir().path());

			if (p.exists() && !p.isWritable()) {
				QMessageBox::critical(0, "Settings", "You do not have permissions to create the specified log file");
				return;
				}
			}
		}

	if (conf) {
#ifndef NVB_STATIC
		conf->setValue("PluginPath", plgPath->text());
#endif

		if (logFile->text().isEmpty())
			conf->remove("LogFile");
		else
			conf->setValue("LogFile", logFile->text());

		conf->sync();
		}

	QDialog::accept();
	}

int NVBSettings::showGeneralSettings() {
	NVBSettings d;
	return d.exec();
	}
