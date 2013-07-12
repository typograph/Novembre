//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#ifdef NVB_ENABLE_LOG

#include "NVBLogUtils.h"
#include <QApplication>
#include <QDateTime>
#include <QTreeView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include "../../icons/icons_log.xpm"
#include "../core/NVBLogger.h"

NVBLogModel::NVBLogModel(QObject * parent): QAbstractItemModel(parent) {
	NVBLogger * lgr = qApp->property("Logger").value<NVBLogger*>();

	connect(lgr, SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)), this, SLOT(addMessage(NVB::LogEntryType, QString, QString, QTime)));
	}

int NVBLogModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) return 0;

	return messages.size();
	}

QVariant NVBLogModel::data(const QModelIndex & index, int role) const {
	if (!index.isValid()) return QVariant();

	if (index.column() == 1) {
		if (role == Qt::DecorationRole)
			return getIcon(messages.at(index.row()).type);
		else if (role == LogEntryTypeRole)
			return QVariant::fromValue(messages.at(index.row()).type);
		}

	if (role != Qt::DisplayRole) return QVariant();

	if (index.column() == 0)
		return messages.at(index.row()).time.toString();

	if (index.column() == 2)
		return messages.at(index.row()).issuer;

	if (index.column() == 3)
		return messages.at(index.row()).message;

	return QVariant();
	}

QVariant NVBLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();

	switch (section) {
		case 0 :
			return QString("Time");

		case 1 :
			return QString();

		case 2 :
			return QString("Issuer");

		case 3 :
			return QString("Message");

		default :
			return QVariant();
		}
	}

QModelIndex NVBLogModel::index(int row, int column, const QModelIndex & parent) const {
	if (parent.isValid()) return QModelIndex();

	return createIndex(row, column);
	}

void NVBLogModel::addMessage(NVB::LogEntryType type, QString issuer, QString message, QTime time) {
	if (type != NVB::DebugEntry) {
		beginInsertRows(QModelIndex(), messages.size(), messages.size());
		messages.append(NVBLogMessage(type, issuer, message, time));
		endInsertRows();
		}
	}

NVBLogWidget::NVBLogWidget(QString title, QWidget * parent): QFrame(parent) {
	setWindowTitle(title);

	QHBoxLayout * l = new QHBoxLayout(this);
#if QT_VERSION >= 0x040300
	l->setContentsMargins(1, 1, 1, 1);
#else
	l->setMargin(1);
#endif
	setLayout(l);

	QToolBar * tb = new QToolBar(this);
	tb->setOrientation(Qt::Vertical);

	actionShowErrors = new QAction(QIcon(_log_error), "Show errors", this);
	actionShowErrors->setCheckable(true);
	actionShowErrors->setChecked(true);
	connect(actionShowErrors, SIGNAL(triggered(bool)), SLOT(err_toggled(bool)));
	tb->addAction(actionShowErrors);

	actionShowPMsg = new QAction(QIcon(_log_info), "Show messages", this);
	actionShowPMsg->setCheckable(true);
	actionShowPMsg->setChecked(true);
	connect(actionShowPMsg, SIGNAL(triggered(bool)), SLOT(pmsg_toggled(bool)));
	tb->addAction(actionShowPMsg);

#ifdef NVB_VERBOSE_LOG
	actionShowVPMsg = new QAction(QIcon(_log_verbose), "Show all messages", this);
	actionShowVPMsg->setCheckable(true);
	connect(actionShowVPMsg, SIGNAL(triggered(bool)), SLOT(vpmsg_toggled(bool)));
	tb->addAction(actionShowVPMsg);
#endif

	tb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	l->addWidget(tb);

	view = new QTreeView(this);
	l->addWidget(view);

	model = new NVBLogFilterModel(new NVBLogModel());
	model->setLogLevel(NVB::PMsgEntry);
	view->setModel(model);
	view->setAllColumnsShowFocus(true);
	view->setRootIsDecorated(false);
	view->sortByColumn(0, Qt::AscendingOrder);
	view->show();
	hide();
	}

void NVBLogFile::addMessage(NVB::LogEntryType type, QString issuer, QString message) {
	if (!isOpen()) return;

	QString typeStr;

	switch (type) {
		case NVB::ErrorEntry :
			{ typeStr = "EE";  break; }

		case NVB::PMsgEntry :
			{ typeStr = "II";  break; }

#ifdef NVB_VERBOSE_LOG

		case NVB::VerboseEntry :
			{ typeStr = "VV";  break; }

#endif

		default :
			{ typeStr = "??"; break; }
		}

	if (!(type == NVB::DebugEntry
#ifndef NVB_VERBOSE_LOG
	      || type == NVB::VerboseEntry
#endif
	     )) {
		write(QString("(%1) : %2 -> %3\n").arg(typeStr, issuer, message).toUtf8());
		flush();
		}
	}

NVBLogFile::NVBLogFile(QString filename, QObject * parent): QFile(filename, parent) {
	if (!open(QIODevice::Append)) {
		NVBOutputFileError(this);
		return;
		}

	write(QString("-----< %1 >-----\n").arg(QDateTime::currentDateTime().toString()).toUtf8());
	flush();

	NVBLogger * lgr = qApp->property("Logger").value<NVBLogger*>();

	connect(lgr, SIGNAL(message(NVB::LogEntryType, QString, QString, QTime)), this, SLOT(addMessage(NVB::LogEntryType, QString, QString)));
	}

QIcon NVBLogModel::getIcon(NVB::LogEntryType type) const {
	switch (type) {
		case NVB::ErrorEntry :
			return QIcon(_log_error);

		case NVB::PMsgEntry :
			return QIcon(_log_info);
#ifdef NVB_VERBOSE_LOG

		case NVB::VerboseEntry :
			return QIcon(_log_verbose);
#endif

		default :
			return QIcon(_log_unk);
		}
	}

NVBLogFilterModel::NVBLogFilterModel(NVBLogModel * model): QSortFilterProxyModel() {
	setSourceModel(model);
	setFilterKeyColumn(1);
	}

bool NVBLogFilterModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {
	if (source_parent.isValid()) return false;

	if ((sourceModel()->data(sourceModel()->index(source_row, 1, source_parent), LogEntryTypeRole).value<NVB::LogEntryType>()) > loglevel) return false;

	return true;
	}

void NVBLogWidget::err_toggled(bool checked) {
	actionShowPMsg->setChecked(false);
#ifdef NVB_VERBOSE_LOG
	actionShowVPMsg->setChecked(false);
#endif

	if (checked) {
		model->setLogLevel(NVB::ErrorEntry);
		}
	else {
		model->setLogLevel(NVB::NoEntry);
		}
	}

void NVBLogWidget::pmsg_toggled(bool checked) {
#ifdef NVB_VERBOSE_LOG
	actionShowVPMsg->setChecked(false);
#endif

	if (checked) {
		actionShowErrors->setChecked(true);
		model->setLogLevel(NVB::PMsgEntry);
		}
	else {
		model->setLogLevel(NVB::ErrorEntry);
		}
	}

#ifdef NVB_VERBOSE_LOG
void NVBLogWidget::vpmsg_toggled(bool checked) {
	if (checked) {
		actionShowErrors->setChecked(true);
		actionShowPMsg->setChecked(true);
		model->setLogLevel(NVB::VerboseEntry);
		}
	else {
		model->setLogLevel(NVB::PMsgEntry);
		}
	}
#endif

NVBLogWidget::~ NVBLogWidget() {
	if (model) {
		QAbstractItemModel * m = model->sourceModel();
		delete model;
		delete m;
		}
	}


#endif
