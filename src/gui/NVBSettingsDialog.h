//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBSETTINGSDIALOG_H
#define NVBSETTINGSDIALOG_H

#include <QtGui/QDialog>

class QStackedWidget;
class QPushButton;
class QListWidget;
class QSettings;

class NVBSettingsDialog : public QDialog {
		Q_OBJECT

	private:

		QSettings * conf;
		QListWidget * sections;
		QStackedWidget * view;
		QPushButton * applyButton;
		QPushButton * resetButton;

		static NVBSettingsDialog * getGlobalDialog();

	public:

		NVBSettingsDialog();
		~NVBSettingsDialog() {;}

	public slots:
		void switchToPage(int page);

		void tryAccept();

		void writeSettings();
		void reinitSettings();

	private slots:
		void pageSwitch();
		void dataOutOfSync();
		void dataInSync();

	public:

		static int showGeneralSettings();
		static int showBrowserSettings();
// 	static int showFileSettings();
// 	static int showPluginSettings();
		void addPage(QWidget * widget);

	};

#endif
