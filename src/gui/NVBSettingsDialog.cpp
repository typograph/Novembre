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

#include "NVBLogger.h"
#include "NVBSettings.h"
#include "NVBSettingsDialog.h"
#include "NVBSettingsWidget.h"
#include "NVBStandardIcons.h"

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QGridLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>

#include <QtGui/QPainter>
#include <QtGui/QTextLayout>

#define LIST_ICON_SIZE 32
#define LIST_ICON_MARGIN 6

/*
class NVBSettingsCheckLineWidget : public NVBSettingsWidget {
public:
	explicit NVBSettingsCheckLineWidget(QString entry, QString text, QString tooltip, QWidget* parent = 0) {
		}

	virtual void init(NVBSettings settings) {
		}
};
*/

NVBSettingsDialog * NVBSettingsDialog::globalInstance = 0;

class NVBGeneralSettingsWidget : public NVBSettingsWidget {
	public:
		NVBGeneralSettingsWidget(NVBSettings conf, QWidget* parent = 0) : NVBSettingsWidget(conf, parent) {
			setWindowIcon(getStandardIcon(NVBStandardIcon::Novembre));
			setWindowIconText("General");
// 		addCheckBox("ShowBrowserOnStart","Open browser on start");
#ifndef NVB_STATIC
			addLineEdit("PluginPath", "Plugin path", "The plugin path defines where Novembre is looking for plugins. Novembre will not work without plugins, so this is a very important field.");
#endif
			addLineEdit("LogFile", "Logfile", "If you want to keep record of application messages, select a location for the log file.");
			}

		virtual bool write() {

#ifndef NVB_STATIC
			QLineEdit * plgPath = entries.first().lineEdit;

			if (!QFile::exists(plgPath->text())) {
				QMessageBox::critical(0, "Settings", "Specified plugin path does not exist");
				return false;
				}

#endif

			QLineEdit * logFile = entries.last().lineEdit;

			if (!logFile->text().isEmpty()) {
				QFileInfo f(logFile->text());

				if (f.exists()) {
					if (!f.isFile()) {
						QMessageBox::critical(0, "Settings", "Specified log file is not a file");
						return false;
						}

					if (!f.isWritable()) {
						QMessageBox::critical(0, "Settings", "You do not have permissions to write to the specified log file");
						return false;
						}
					}
				else {
					QFileInfo p(f.dir().path());

					if (p.exists() && !p.isWritable()) {
						QMessageBox::critical(0, "Settings", "You do not have permissions to create the specified log file");

						return false;
						}
					}
				}

			return NVBSettingsWidget::write();
			}
	};

class NVBBrowserSettingsWidget : public NVBSettingsWidget {
	public:
		NVBBrowserSettingsWidget(NVBSettings conf, QWidget* parent = 0) : NVBSettingsWidget(conf, parent) {
			setWindowIcon(getStandardIcon(NVBStandardIcon::Browser));
			setWindowIconText("Browser");
// 			setGroup("Browser");
			addCheckBox("ShowOnStart", "Show browser on start");
			addCheckBox("ShowMaximized", "Show maximized");
			addComboBox("IconSize", "Default icon size", QStringList() << "64" << "128" << "256" << "512");
			}
	};

/**
 * \class NVBFilesSettingsWidget
 *
 * An NVBSettingsWidget for NVBFilesFactory.
 *
 * Here, the user can set properties for individual plugins,
 * such as disable loading of files with these plugins,
 * or change plugin-specific settings.
 *
 */

class NVBFilesSettingsWidget : public NVBSettingsWidget {
	public:
		NVBFilesSettingsWidget(NVBSettings conf, QWidget* parent = 0) : NVBSettingsWidget(conf, parent) {
			setWindowIcon(getStandardIcon(NVBStandardIcon::Browser));
			setWindowIconText("Files");
// 		setGroup("");
			}

		virtual void init() {
			}

		virtual bool write() {
			}
	};


/**
 * NVBSettingsSectionDelegate for showing a list of big icons
 *
 * Adapted from kdelibs-4.5.3 KPageListViewDelegate
 *
 */

class NVBSettingsSectionDelegate : public QAbstractItemDelegate {
	public:
		NVBSettingsSectionDelegate( QObject *parent = 0 ) : QAbstractItemDelegate( parent ) {;}

		virtual void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	private:
		void drawFocus( QPainter*, const QStyleOptionViewItem&, const QRect& ) const;
	};

void NVBSettingsSectionDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	if ( !index.isValid() )
		return;

	const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
	const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
	const QPixmap pixmap = icon.pixmap( LIST_ICON_SIZE, LIST_ICON_SIZE );

	QFontMetrics fm = painter->fontMetrics();

	QPen pen = painter->pen();
	QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
	                          ? QPalette::Normal : QPalette::Disabled;

	if ( cg == QPalette::Normal && !(option.state & QStyle::State_Active) )
		cg = QPalette::Inactive;

	QStyleOptionViewItemV4 opt(option);
	opt.showDecorationSelected = true;
	QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

	if ( option.state & QStyle::State_Selected )
		painter->setPen( option.palette.color( cg, QPalette::HighlightedText ) );
	else
		painter->setPen( option.palette.color( cg, QPalette::Text ) );

	painter->drawPixmap(option.rect.x() + option.rect.width() / 2 - pixmap.width() / 2, option.rect.y() + LIST_ICON_MARGIN + LIST_ICON_SIZE / 2 - pixmap.height() / 2, pixmap );
	painter->drawText( QRect(option.rect.x(), option.rect.y() + LIST_ICON_SIZE + 1.5 * LIST_ICON_MARGIN, option.rect.width(), fm.height()), Qt::AlignHCenter, text);

	painter->setPen( pen );

	drawFocus( painter, option, option.rect );
	}

QSize NVBSettingsSectionDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	if ( !index.isValid() )
		return QSize( LIST_ICON_SIZE + 2 * LIST_ICON_MARGIN, 0 );

	QFontMetrics fm = option.fontMetrics;
	return QSize(LIST_ICON_SIZE + 2 * LIST_ICON_MARGIN, LIST_ICON_SIZE + 2.5 * LIST_ICON_MARGIN + fm.height());

	}

void NVBSettingsSectionDelegate::drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const {
	if (option.state & QStyle::State_HasFocus) {
		QStyleOptionFocusRect o;
		o.QStyleOption::operator=(option);
		o.rect = rect;
		o.state |= QStyle::State_KeyboardFocusChange;
		QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
		                          ? QPalette::Normal : QPalette::Disabled;
		o.backgroundColor = option.palette.color( cg, (option.state & QStyle::State_Selected)
		                    ? QPalette::Highlight : QPalette::Background );
		QApplication::style()->drawPrimitive( QStyle::PE_FrameFocusRect, &o, painter );
		}
	}


NVBSettingsDialog::NVBSettingsDialog(NVBSettings settings)
	: QDialog()
	, conf(settings) {

	QGridLayout * l = new QGridLayout(this);

//	setLayout(l);

	l->addWidget(sections = new QListWidget(this), 0, 0);
	l->addWidget(view = new QStackedWidget(this), 0, 1);
	connect(sections, SIGNAL(currentRowChanged(int)), view, SLOT(setCurrentIndex(int)));
	connect(view, SIGNAL(currentChanged(int)), this, SLOT(pageSwitch()));

	sections->setViewMode(QListView::ListMode);
	sections->setItemDelegate( new NVBSettingsSectionDelegate( this ) );
// 	sections->setResizeMode(QListView::Fixed);
	sections->setMaximumWidth(LIST_ICON_SIZE + 2 * LIST_ICON_MARGIN + 10);
	sections->setMinimumWidth(LIST_ICON_SIZE + 2 * LIST_ICON_MARGIN + 5 + style()->pixelMetric(QStyle::PM_ScrollBarExtent));
	sections->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	QPushButton * closeButton = new QPushButton("Close", this);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
	l->addWidget(closeButton, 1, 0);

	QDialogButtonBox * buttonBox = new QDialogButtonBox();
	applyButton = buttonBox->addButton("Apply", QDialogButtonBox::AcceptRole);
	resetButton = buttonBox->addButton("Revert", QDialogButtonBox::RejectRole);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(writeSettings()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reinitSettings()));

	l->addWidget(buttonBox, 1, 1, Qt::AlignRight);

	addPage(new NVBGeneralSettingsWidget(conf.group("General")));
	addPage(new NVBBrowserSettingsWidget(conf.group("Browser")));
// 	addPage(new NVBFileSettingsWidget(conf.group("Files")));
// 	addPage(new NVBToolsSettingsWidget(conf.group("Tools")));

	view->setCurrentIndex(0);
	}

void NVBSettingsDialog::addPage(QWidget* widget) {
	new QListWidgetItem(widget->windowIcon(), widget->windowIconText(), sections);
	view->addWidget(widget);
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(widget);

	if (w) {
		connect(w, SIGNAL(dataChanged()), this, SLOT(dataOutOfSync()));
		connect(w, SIGNAL(dataSynced()), this, SLOT(dataInSync()));
		w->init();
		}
	}

void NVBSettingsDialog::tryAccept() {
	if (applyButton->isEnabled())
		reinitSettings();

	accept();
	}

void NVBSettingsDialog::dataInSync() {
	applyButton->setEnabled(false);
	resetButton->setEnabled(false);
	}

void NVBSettingsDialog::dataOutOfSync() {
	applyButton->setEnabled(true);
	resetButton->setEnabled(true);
	}


void NVBSettingsDialog::reinitSettings() {
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());

	if (w) w->init();
	}

void NVBSettingsDialog::writeSettings() {
	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());

	if (w) w->write();
	}

void NVBSettingsDialog::switchToPage(int page) {
	if (applyButton->isEnabled())
		reinitSettings();

	view->setCurrentIndex(page);
	}

void NVBSettingsDialog::pageSwitch() {
// 	NVBSettingsWidget * w = qobject_cast<NVBSettingsWidget*>(view->currentWidget());
	}


int NVBSettingsDialog::showGeneralSettings() {
	if (globalInstance) {
		globalInstance->switchToPage(0);
		return globalInstance->exec();
		}
	}

int NVBSettingsDialog::showBrowserSettings() {
	if (globalInstance) {
		globalInstance->switchToPage(1);
		return globalInstance->exec();
		}
	}

/*
int NVBSettingsDialog::showFileSettings()
{
	if (globalInstance) {
		globalInstance->switchToPage(2);
		return globalInstance->exec();
		}
}

int NVBSettingsDialog::showPluginSettings()
{
	if (globalInstance) {
		globalInstance->switchToPage(3);
		return globalInstance->exec();
	}
}
*/

void NVBSettingsDialog::initGlobalDialog(NVBSettings settings) {
	if (globalInstance) {
		delete globalInstance;
		globalInstance = 0;
	}
	globalInstance = new NVBSettingsDialog(settings);
}


