/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "NVBSettingsWidget.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtCore/QSettings>

#include "NVBLogger.h"

NVBSettingsWidget::NVBSettingsWidget(QWidget* parent): QWidget(parent)
{
	setLayout(vlayout = new QVBoxLayout());
}

void NVBSettingsWidget::addSetting(NVBSettingsWidget* widget)
{
	entries << NVBSettingsWidgetEntry(widget);
	vlayout->addWidget(widget);

	connect(widget,SIGNAL(dataChanged()),this,SIGNAL(dataChanged()));
}

void NVBSettingsWidget::addCheckBox(QString entry, QString text, QString tooltip)
{
	QCheckBox * cb = new QCheckBox(text);
	if (cb) cb->setToolTip(tooltip);
	addCheckBox(entry,cb);	
}

void NVBSettingsWidget::addCheckBox(QString entry, QCheckBox* checkBox)
{
	if (!checkBox) return;
	entries << NVBSettingsWidgetEntry(entry,checkBox);
	vlayout->addWidget(checkBox);
	connect(checkBox,SIGNAL(clicked(bool)),this,SIGNAL(dataChanged()));
}

void NVBSettingsWidget::addComboBox(QString entry, QString label, QStringList items, QString tooltip)
{
	QComboBox * c = new QComboBox(this);
	if (c) {
		c->addItems(items);
		c->setToolTip(tooltip);
		}
	addComboBox(entry,label,c);
}

void NVBSettingsWidget::addComboBox(QString entry, QString label, QComboBox* combobox)
{
	if (!combobox) return;
	entries << NVBSettingsWidgetEntry(entry,combobox);
	QHBoxLayout * hl = new QHBoxLayout(this);
	QLabel * lbl = new QLabel(label);
	if (lbl) lbl->setToolTip(combobox->toolTip());
	hl->addWidget(lbl);
	hl->addWidget(combobox);
	vlayout->addLayout(hl);
	connect(combobox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(dataChanged()));
}

void NVBSettingsWidget::addLineEdit(QString entry, QString label, QString tooltip)
{
	QLineEdit * le = new QLineEdit(this);
	le->setToolTip(tooltip);
	addLineEdit(entry,label,le);
}

void NVBSettingsWidget::addLineEdit(QString entry, QString label, QLineEdit* lineedit)
{
	if (!lineedit) return;
	entries << NVBSettingsWidgetEntry(entry,lineedit);
	QHBoxLayout * hl = new QHBoxLayout(this);
	QLabel * lbl = new QLabel(label);
	if (lbl) lbl->setToolTip(lineedit->toolTip());
	hl->addWidget(lbl);
	hl->addWidget(lineedit);
	vlayout->addLayout(hl);
	connect(lineedit,SIGNAL(textChanged(QString)),this,SIGNAL(dataChanged()));
}

void NVBSettingsWidget::init(QSettings* settings)
{
	if (!settings) return;

	if (!groupname.isNull())
		settings->beginGroup(groupname);
	
	foreach(NVBSettingsWidgetEntry e, entries) {
		switch(e.type) {
			case NVBSettingsWidgetEntry::External :
				e.settingsWidget->init(settings);
				break;
			case NVBSettingsWidgetEntry::ComboBox :
				QString value = settings->value(e.key).toString();
				int i = e.comboBox->findText(value);
				if (i >= 0)
					e.comboBox->setCurrentIndex(i);
				else if (e.comboBox->isEditable())
					e.comboBox->setEditText(value);
				else
					NVBOutputError(QString("Unrecognized value %3 in settings at %1/%2").arg(group()).arg(e.key).arg(value));
				break;
			case NVBSettingsWidgetEntry::CheckBox :
				e.checkBox->setChecked(settings->value(e.key).toBool());
				break;
			case NVBSettingsWidgetEntry::LineEdit :
				e.lineEdit->setText( settings->value(e.key).toString() );
				break;
			default:
				NVBOutputError("Unrecognized settings item type");
			}
		}

	if (!groupname.isNull())
		settings->endGroup();
	
	emit dataSynced();
}

void NVBSettingsWidget::write(QSettings* settings)
{
	if (!settings) return;
	
	if (!groupname.isNull())
		settings->beginGroup(groupname);
	
	foreach(NVBSettingsWidgetEntry e, entries) {
		switch(e.type) {
			case NVBSettingsWidgetEntry::External :
				e.settingsWidget->write(settings);
				break;
			case NVBSettingsWidgetEntry::ComboBox :
				settings->setValue(e.key,e.comboBox->currentText());
				break;
			case NVBSettingsWidgetEntry::CheckBox :
				settings->setValue(e.key,e.checkBox->isChecked());
				break;
			case NVBSettingsWidgetEntry::LineEdit :
				if (e.lineEdit->text().isEmpty())
					settings->remove(e.key);
				else
					settings->setValue(e.key,e.lineEdit->text());
				break;
			default:
				NVBOutputError("Unrecognized settings item type");
			}
		}
		
	if (!groupname.isNull())
		settings->endGroup();
	
	emit dataSynced();
}
