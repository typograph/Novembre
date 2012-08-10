#include "NVBGeneratorsSettingsWidget.h"

#include <QtCore/QAbstractListModel>
#include <QtGui/QListView>
#include <QtCore/QCoreApplication>
#include "NVBFilePluginModel.h"

NVBGeneratorsSettingsWidget::NVBGeneratorsSettingsWidget()
: NVBSettingsWidget()
, pluginList(0)
, ffactory(0)
{
	pluginList = new QListView(this);
	if (!pluginList) return;
	pluginList->setModel(new NVBFilePluginModel());
	if (!pluginList->model()) return;

	connect(pluginList->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectGenerator()));

}

void NVBGeneratorsSettingsWidget::init(QSettings *settings) {

}

bool NVBGeneratorsSettingsWidget::write(QSettings *settings) {

}
