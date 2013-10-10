//
// Copyright 2012 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBGENERATORSSETTINGSWIDGET_H
#define NVBGENERATORSSETTINGSWIDGET_H

class QTableView;
class QPushButton;
class NVBFileFactory;
class NVBFilePluginModel;
class QStackedWidget;

#include "NVBSettingsWidget.h"
#include <QtCore/QModelIndex>

/**
 * \class NVBGeneratorsSettingsWidget
 *
 * An NVBSettingsWidget for NVBFilesFactory.
 *
 * Here, the user can set properties for individual plugins,
 * such as disable loading of files with these plugins,
 * or change plugin-specific settings.
 *
 */

class NVBGeneratorsSettingsWidget : public NVBSettingsWidget {
	Q_OBJECT
	private:
		/// A table showing the list of plugins
		QTableView * pluginList;
		/// The model supplying data for the table
		NVBFilePluginModel * pluginModel;
		/// The widget showing the configuration widgets of plugins
    QStackedWidget* pluginSettingsView;

	private slots:
		/// Show the appropriate configuration widget
		void selectGenerator( QModelIndex index );
		/// Activate/deactivate plugings
		void updateGeneratorSettings(QModelIndex, QModelIndex);

	public:
		/// Create the widget
		NVBGeneratorsSettingsWidget(NVBSettings settings);
		virtual ~NVBGeneratorsSettingsWidget() {;}
		
		/// Set a model with generators
		void setModel(NVBFilePluginModel * model);
		
		/// Copy widget values from config file
		virtual void init();

		/// Write out all changes to config
		virtual bool write();

	};

#endif // NVBGENERATORSSETTINGSWIDGET_H
