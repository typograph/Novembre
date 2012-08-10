#ifndef NVBGENERATORSSETTINGSWIDGET_H
#define NVBGENERATORSSETTINGSWIDGET_H

class QListView;
class QPushButton;
class NVBFileFactory;

#include "NVBSettingsWidget.h"

/**
 * A widget for controlling the usage of file generators and their properties
 *
 *
 */

class NVBGeneratorsSettingsWidget : public NVBSettingsWidget
{
private:
	QListView * pluginList;

	NVBFileFactory * ffactory;

	NVBGeneratorsSettingsWidget * pluginWidget;

private slots:
	void selectGenerator();

public:
	NVBGeneratorsSettingsWidget();
	virtual ~NVBGeneratorsSettingsWidget() {;}

	/// Copy widget values from an existing QSettings
	virtual void init(QSettings * settings);
	/// Write out all changes into QSettings
	virtual bool write(QSettings * settings);


};

#endif // NVBGENERATORSSETTINGSWIDGET_H
