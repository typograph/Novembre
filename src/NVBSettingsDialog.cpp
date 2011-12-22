//
// C++ Implementation: NVBSettingsDialog
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBLogger.h"
#include "NVBSettingsDialog.h"
#include "NVBSettingsWidget.h"

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

#include <QtGui/QGridLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>

NVBSettingsDialog::NVBSettingsDialog()
: QDialog()
{
	conf = qApp->property("NVBSettings").value<QSettings*>();
	if (!conf)
		NVBCriticalError("Configuration missing");

	QGridLayout * l = new QGridLayout(this);

	setLayout(l);
	
	l->addWidget(sections = new QListWidget(this),0,0);
	l->addWidget(view = new QStackedWidget(this),0,1);
	connect(sections,SIGNAL(currentRowChanged(int)),view,SLOT(setCurrentIndex(int)));
	connect(view,SIGNAL(currentChanged(int)),this,SLOT(pageSwitch()));

	QPushButton * closeButton = new QPushButton("Close",this);
	connect(closeButton,SIGNAL(clicked()),this,SLOT(accept()));
	l->addWidget(closeButton,1,0);

/*
#ifndef NVB_STATIC
	l->addWidget(new QLabel("The plugin path defines where Novembre is looking for plugins. Novembre will not work without plugins, so this is a very important field.",this),0,0,1,-1);
	l->addWidget(new QLabel("Plugin path.",this),1,0);
	l->addWidget(plgPath = new QLineEdit(conf->value("PluginPath",QString()).toString(),this),1,1);
#endif

	l->addWidget(new QLabel("If you want to keep record of application messages, select a location for the log file.",this),2,0,1,-1);
	l->addWidget(new QLabel("Logfile.",this),3,0);
	l->addWidget(logFile = new QLineEdit(conf->value("LogFile",QString()).toString(),this)
,3,1);
*/

	QDialogButtonBox * buttonBox = new QDialogButtonBox();
	applyButton = buttonBox->addButton("Apply",QDialogButtonBox::AcceptRole);
	resetButton = buttonBox->addButton("Revert",QDialogButtonBox::RejectRole);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(writeSettings()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reinitSettings()));

	l->addWidget(buttonBox,1,1,Qt::AlignRight);

	addPage(new NVBGeneralSettingsWidget());
	addPage(new NVBBrowserSettingsWidget());
// 	addPage(new NVBFileSettingsWidget());
// 	addPage(new NVBToolsSettingsWidget());

	view->setCurrentIndex(0);
}

void NVBSettingsDialog::addPage(QWidget* widget)
{
	QListWidgetItem * i = new QListWidgetItem(widget->windowIcon(),widget->windowIconText(),sections);
	view->addWidget(widget);
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(widget);
	if (w) {
		connect(w,SIGNAL(dataChanged()),this,SLOT(dataOutOfSync()));
		connect(w,SIGNAL(dataSynced()),this,SLOT(dataInSync()));
		}
}

void NVBSettingsDialog::tryAccept()
{
	if (applyButton->isEnabled())
		reinitSettings();
	
	if (userMadeChanges)
		accept();
	else
		reject();
}

void NVBSettingsDialog::dataInSync()
{
	applyButton->setEnabled(false);
	resetButton->setEnabled(false);
}

void NVBSettingsDialog::dataOutOfSync()
{
	applyButton->setEnabled(true);
	resetButton->setEnabled(true);
}


void NVBSettingsDialog::reinitSettings()
{
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());
	if (w) w->init(conf);
}

void NVBSettingsDialog::writeSettings()
{
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());
	if (w) w->write(conf);
}

void NVBSettingsDialog::switchToPage(int page)
{
	if (applyButton->isEnabled())
		reinitSettings();
	
	view->setCurrentIndex(page);
}

void NVBSettingsDialog::pageSwitch()
{
// 	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());
}


int NVBSettingsDialog::showGeneralSettings()
{
	NVBSettingsDialog * d = getGlobalDialog();
	d->switchToPage(0);
	return d->exec();
}

int NVBSettingsDialog::showBrowserSettings()
{
	NVBSettingsDialog * d = getGlobalDialog();
	d->switchToPage(1);
	return d->exec();
}

/*
int NVBSettingsDialog::showFileSettings()
{
	NVBSettingsDialog * d = getGlobalDialog();
	d->switchToPage(2);
	return d->exec();
}

int NVBSettingsDialog::showPluginSettings()
{
	NVBSettingsDialog * d = getGlobalDialog();
	d->switchToPage(3);
	return d->exec();
}
*/

NVBSettingsDialog* NVBSettingsDialog::getGlobalDialog()
{
	static NVBSettingsDialog * d = new NVBSettingsDialog();
	return d;
}

/*
class NVBSettingsCheckLineWidget : public NVBSettingsWidget {
public:
	explicit NVBSettingsCheckLineWidget(QString entry, QString text, QString tooltip, QWidget* parent = 0) {
		}

	virtual void init(QSettings * settings) {
		}
};
*/


class NVBGeneralSettingsWidget : public NVBSettingsWidget {
public:
	NVBGeneralSettingsWidget(QWidget* parent = 0) : NVBSettingsWidget(parent) {
		setGroup("General");
// 		addCheckBox("ShowBrowserOnStart","Open browser on start");
#ifndef NVB_STATIC
		addLineEdit("PluginPath","Plugin path","The plugin path defines where Novembre is looking for plugins. Novembre will not work without plugins, so this is a very important field.");
#endif
		addLineEdit("LogFile","Logfile","If you want to keep record of application messages, select a location for the log file.");
		}
	
	virtual void write(QSettings * settings) {
		
		
#ifndef NVB_STATIC
		QLineEdit * plgPath = items.first().lineEdit;
		if (!QFile::exists(plgPath->text())) {
			QMessageBox::critical(0,"Settings","Specified plugin path does not exist");
			return;
			}
#endif

		QLineEdit * logFile = items.last().lineEdit;

		if (!logFile->text().isEmpty()) {
			QFileInfo f(logFile->text());

			if (f.exists()) {
				if (!f.isFile()) {
					QMessageBox::critical(0,"Settings","Specified log file is not a file");
					return;
					}
				if (!f.isWritable()) {
					QMessageBox::critical(0,"Settings","You do not have permissions to write to the specified log file");
					return;
					}
				}
			else {
				QFileInfo p(f.dir().path());
				if (p.exists() && !p.isWritable()) {
					QMessageBox::critical(0,"Settings","You do not have permissions to create the specified log file");
	
				return;
					}
				}
			}
			
		NVBGeneralSettingsWidget::write(settings);
	}
};

class NVBBrowserSettingsWidget : public NVBSettingsWidget {
public:
	NVBBrowserSettingsWidget(QWidget* parent = 0) : NVBSettingsWidget(parent) {
		setGroup("General");
		addCheckBox("ShowOnStart","Show browser on start");
		addCheckBox("ShowMaximized","Show maximized");
		addComboBox("IconSize","Default icon size",QStringList() << "64x64" << "128x128" << "256x256" << "512x512");
		}
	
/*	virtual void write(QSettings * settings) {			
		NVBSettingsWidget::write(settings);
	}*/
};