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
#ifndef NVBLOGUTILS_H
#define NVBLOGUTILS_H

#ifdef NVB_ENABLE_LOG

#include <QFrame>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QFile>
#include <QCloseEvent>
#include <QIcon>
#include <QAction>
#include <QSortFilterProxyModel>

#include "../core/NVBLogger.h"

class NVBLogFile : public QFile {
		Q_OBJECT
	public:
		NVBLogFile(QString filename, QObject * parent = 0);
		virtual ~NVBLogFile() {;}
	private slots:
		void addMessage(NVB::LogEntryType type, QString issuer, QString message);
	};

struct NVBLogMessage {
	NVBLogMessage(NVB::LogEntryType _type, QString _issuer, QString _message, QTime _time): type(_type), issuer(_issuer), message(_message), time(_time) {;}
	NVB::LogEntryType type;
	QString issuer;
	QString message;
	QTime time;
	};

#define LogEntryTypeRole (Qt::UserRole + 300)

class NVBLogModel : public QAbstractItemModel {
		Q_OBJECT
	private:
		QList<NVBLogMessage> messages;
		QIcon getIcon(NVB::LogEntryType type) const;
	public:
		NVBLogModel(QObject * parent  = 0);
		virtual ~NVBLogModel() {;}

		virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const { return !parent.isValid();}
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { return parent.isValid() ? 0 : 4;}
//  virtual Qt::ItemFlags flags(const QModelIndex &index) const {;}
		virtual QVariant data(const QModelIndex &index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &) const { return QModelIndex();}
	private slots:
		void addMessage(NVB::LogEntryType type, QString issuer, QString message, QTime time);
	};

class NVBLogFilterModel : public QSortFilterProxyModel {
	private:
		NVB::LogEntryType loglevel;
	protected:
		virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
	public:
		NVBLogFilterModel(NVBLogModel * model);
		virtual ~NVBLogFilterModel() {;}

		inline void setLogLevel(NVB::LogEntryType new_loglevel) {
			loglevel = new_loglevel;
#if QT_VERSION >= 0x040300
			invalidateFilter();
#else
			filterChanged();
#endif
			}

	};

class NVBLogWidget : public QFrame {
		Q_OBJECT
	private:
//  NVBLogModel * model;
		QAction * actionShowErrors;
		QAction * actionShowPMsg;
#ifdef NVB_VERBOSE_LOG
		QAction * actionShowVPMsg;
#endif

		NVBLogFilterModel * model;
		QTreeView * view;
	private slots:
		void err_toggled(bool);
		void pmsg_toggled(bool);
#ifdef NVB_VERBOSE_LOG
		void vpmsg_toggled(bool);
#endif
	public:
		NVBLogWidget(QString title, QWidget * parent = 0);
		virtual ~NVBLogWidget();
	public slots:
		virtual void closeEvent(QCloseEvent * event) { event->ignore(); hide(); }
		virtual void showEvent(QShowEvent * event) {
			event->accept();
			emit shown();
			view->resizeColumnToContents(0);
			view->resizeColumnToContents(1);
			view->resizeColumnToContents(2);
			}
	signals:
		void shown();
	};

#endif

#endif
