//
// C++ Interface: NVBSettingsDialog
//
// This file is part of Novembre GUI
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
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
