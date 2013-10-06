//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBSETTINGSWIDGET_H
#define NVBSETTINGSWIDGET_H

#include <QtGui/QFrame>
#include <QtCore/QStringList>

class QSettings;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QVBoxLayout;

/**
 * NVBSettingsWidget is a widget that can save and init values of objects in its layout
 * from a given QSettings object.
 *
 * NVBSettingsWidget children are laid out with a QVBoxLayout, in order in which
 * the add*** functions are called.
 *
 * In case this simple scheme doesn't work for you, subclass NVBSettingsWidget,
 * and replace the layout and the init() and write() functions.
 * Do not forget to use the group in your init() function.
 */

class NVBSettingsWidget;

struct NVBSettingsWidgetEntry {
	enum Type { External, ComboBox, CheckBox, LineEdit } type;
	QString key;
	union {
		NVBSettingsWidget * settingsWidget;
		QComboBox * comboBox;
		QCheckBox * checkBox;
		QLineEdit * lineEdit;
		};

	NVBSettingsWidgetEntry(QString entry, QComboBox * combo) : type(ComboBox), key(entry), comboBox(combo) {;}
	NVBSettingsWidgetEntry(QString entry, QCheckBox * check) : type(CheckBox), key(entry), checkBox(check) {;}
	NVBSettingsWidgetEntry(QString entry, QLineEdit * line ) : type(LineEdit), key(entry), lineEdit(line) {;}
	NVBSettingsWidgetEntry(NVBSettingsWidget * widget ) : type(External), settingsWidget(widget) {;}
	};

class NVBSettingsWidget : public QFrame {
		Q_OBJECT

	protected:
		bool uncommitted;

		QString groupname;
		NVBSettingsWidget * parentSettings;

		QVBoxLayout * vlayout;
		QList<NVBSettingsWidgetEntry> entries;

	public:
		explicit NVBSettingsWidget(QWidget* parent = 0);

		/// Set a group for all the children widgets to use in QSettings
		void setGroup(QString groupName) { groupname = groupName; }
		/// Return the group in use
		QString group() {
			return groupname;
			}

		QString fullGroup() {
			return (parentSettings ? QString("%1/%2").arg(parentSettings->fullGroup(), groupname) : groupname );
			}

		/// Add an NVBSettingsWidget to the layout (last position)
		void addSetting(NVBSettingsWidget *);
		/// Add a checkbox to the layout, with caption \a text and the corresponding QSettings key \a entry
		void addCheckBox(QString entry, QString text, QString tooltip = QString());
		/// Add an existing \a checkbox to the layout, with the corresponding QSettings key \a entry.
		void addCheckBox(QString entry, QCheckBox * checkBox);
		/// Add a combobox to the layout, with caption \a label, \a items and the corresponding QSettings key \a entry
		void addComboBox(QString entry, QString label, QStringList items, QString tooltip = QString());
		/// Add an existing \a combobox to the layout, with caption \a label and the corresponding QSettings key \a entry
		void addComboBox(QString entry, QString label, QComboBox * combobox);
		/// Add a lineedit to the layout, with caption \a label and the corresponding QSettings key \a entry
		void addLineEdit(QString entry, QString label, QString tooltip = QString());
		/// Add an existing \a lineedit to the layout, with caption \a label and the corresponding QSettings key \a entry
		void addLineEdit(QString entry, QString label, QLineEdit * lineedit);

		/// Copy widget values from an existing QSettings
		virtual void init(QSettings * settings);
		/// Write out all changes into QSettings
		virtual bool write(QSettings * settings);

		/// This function will be called when data is written.
		virtual void onWrite() {;}

	signals:

		void dataSynced();
		void dataChanged();

	private slots:
		void synced() { uncommitted = false; }
		void edited() { uncommitted = true;  }
	};

#endif // NVBSETTINGSWIDGET_H
