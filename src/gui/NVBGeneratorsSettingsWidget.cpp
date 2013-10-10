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

#include "NVBGeneratorsSettingsWidget.h"

#include <QtCore/QAbstractListModel>
#include <QtGui/QTableView>
#include <QtCore/QCoreApplication>
#include <QtGui/QLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QStackedWidget>
#include "NVBFilePluginModel.h"
#include "NVBFileGenerator.h"

#include "../../icons/generators.xpm"

/**
 * The widget is created without a model. The table is empty.
 * (This is needed because the dialog with this widget is created before
 * NVBFileFactory)
 *
 * NVBGeneratorsSettingsWidget does not allow reverting changes in activation status
 * of the plugins. Those are immediately saved.
 */
NVBGeneratorsSettingsWidget::NVBGeneratorsSettingsWidget(NVBSettings conf)
	: NVBSettingsWidget(conf)
	, pluginList(0)
	, pluginModel(0)
	{

	pluginList = new QTableView(this);
	if (!pluginList) return;
	appendWidgetToLayout(pluginList);

	pluginSettingsView = new QStackedWidget(this);
	pluginSettingsView->hide();
	appendWidgetToLayout(pluginSettingsView);

	pluginList->verticalHeader()->hide();
	pluginList->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	setWindowIcon(QIcon(_generators));
	setWindowIconText("File formats");
	}

/**
 * Set the NVBFilePluginModel with generators.
 * This function is used by the NVBFileFactory upon generator loading.
 */
void NVBGeneratorsSettingsWidget::setModel(NVBFilePluginModel* model) {
	if (pluginList) {
		while(pluginSettingsView->count()) {
			QWidget * w = pluginSettingsView->widget(0);
			pluginSettingsView->removeWidget(w);
			delete w;
			}
		pluginList->setModel(model);
		pluginList->setSelectionBehavior(QAbstractItemView::SelectRows);
		pluginModel = model;
		connect(pluginList->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(selectGenerator(QModelIndex)));
		connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(updateGeneratorSettings(QModelIndex, QModelIndex)));

		for (int gi = 0; gi < pluginModel->rowCount(); gi++) {
			NVBSettingsWidget * generatorWidget = pluginModel->availableGenerators().at(gi)->configurationPage();
			if (generatorWidget) {
				addSetting(generatorWidget, false);
				pluginSettingsView->addWidget(generatorWidget);
				}
			else
				pluginSettingsView->addWidget(new QLabel("No user-controllable settings available."));
			}

		init();
	}
}

/**
 * Activates/deactivates generators based on the settings in the configuration file.
 * Initialization of the settings of individual plugins handled by parent class
 */
void NVBGeneratorsSettingsWidget::init() {
	if (pluginModel) {
		for (int gi = 0; gi < pluginModel->rowCount(); gi++) {
			const NVBFileGenerator * generator = pluginModel->availableGenerators().at(gi);
			pluginModel->setGeneratorActive(gi, settings.value(generator->moduleName()).toBool());
			}
		}
	NVBSettingsWidget::init();
}

/**
 * Write settings to the configuration file.
 * Saving the settings of individual plugins handled by parent class
 */
bool NVBGeneratorsSettingsWidget::write() {
	if (pluginModel) {
		for (int gi = 0; gi < pluginModel->rowCount(); gi++)
			settings.setValue(pluginModel->availableGenerators().at(gi)->moduleName(), pluginModel->isGeneratorActive(gi));
		}
	NVBSettingsWidget::write();
}

/**
 * Slot called on user selection in the table.
 * Show the correct plugin widget
 */
void NVBGeneratorsSettingsWidget::selectGenerator(QModelIndex index) {
	pluginSettingsView->setCurrentIndex(index.row());
	pluginSettingsView->show();
}

/**
 * Slot called on plugin checking/unchecking.
 * Autosaving the data.
 */
void NVBGeneratorsSettingsWidget::updateGeneratorSettings(QModelIndex start, QModelIndex end) {
	if (start.column() == 0) {
		for (int gi = start.row(); gi <= end.row(); gi++)
			settings.setValue(pluginModel->availableGenerators().at(gi)->moduleName(), pluginModel->isGeneratorActive(gi));
		}	
}
