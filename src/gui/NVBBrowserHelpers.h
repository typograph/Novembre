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
#ifndef NVBBROWSERHELPERS_H
#define NVBBROWSERHELPERS_H

#include "NVBDirModel.h"
#include <QCompleter>
#include <QValidator>
#include <QFileIconProvider>
#include <QDir>
#include <QDialog>

class QLabel;
class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QFileDialog;
class QAction;
class QToolButton;

struct NVBFileSystemDirEntry {
	QString path;
	QString name;
	NVBFileSystemDirEntry * parent;
	bool populated;
	QFileInfo finfo;
	QList<NVBFileSystemDirEntry*> subs;

	NVBFileSystemDirEntry(): parent(0), populated(false) {;}
	NVBFileSystemDirEntry(NVBFileSystemDirEntry *, QString);
	~NVBFileSystemDirEntry();

	void populate();
	};

class NVBFileSystemDirModel : public QAbstractItemModel {

// Here we could add an icon factory, but there seem to be no reason for that
		QFileIconProvider iconFactory;
		NVBFileSystemDirEntry rootEntry;

	public:
		NVBFileSystemDirModel(QObject * parent = 0);

		QVariant data(const QModelIndex &index, int role) const;
		QModelIndex index(int row, int column, const QModelIndex &parent) const;
		QModelIndex parent(const QModelIndex &index) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		int columnCount(const QModelIndex & /* parent */) const { return 1; }
		int rowCount(const QModelIndex &parent) const;
		bool hasChildren(const QModelIndex &parent) const;

	};

class NVBDirCompleter : public QCompleter {
	public:
		NVBDirCompleter(QObject * parent = 0);

		QString pathFromIndex(const QModelIndex &index) const;
		QStringList splitPath(const QString &path) const;
	};

class NVBPathValidator : public QValidator {
	private:
		QDir d;
	public:
		NVBPathValidator(QObject * parent = 0)
			: QValidator(parent)
			, d(QDir::root())
			{;}

		QValidator::State validate(QString & s, int &) const;
	};

class NVBFolderInputDialog : public QDialog {
		Q_OBJECT
	private:
		QLabel * nameLabel;
		QLineEdit * nameEdit;
		QCheckBox * dirLabel;
		QLineEdit * dirEdit;
		QCheckBox * subfolderCheck;
		QDialogButtonBox * buttonBox;
		QToolButton * folderSelectButton;
		QAction * folderSelectAction;
		QFileDialog * fileDialog;

//     bool isDirValid;

		void buildUi();

	public:
		NVBFolderInputDialog(QWidget * parent = 0);
		NVBFolderInputDialog(QString label, QString dirname, bool includeSubfolders, QWidget * parent = 0);
		static bool getFolder(QString & label, QString & dirname, bool & includeSubfolders);
		static bool editFolder(QString & label, QString & dirname, bool & includeSubfolders);

		QString getDir();
		QString getName();
		bool getIncludeSubfolders();
	private slots:
		void dirSelected();
		void checkInput();
	};

class NVBDirExportDialog : public QDialog {
		Q_OBJECT
	private:
		QLineEdit * fileNameEdit;
		QCheckBox * recursiveOpt;
		QCheckBox * loadOpt;
		QCheckBox * fullnamesOpt;
		QDialogButtonBox * buttonBox;
		QFileDialog * fileDialog;
	public:
		NVBDirExportDialog(QWidget * parent = 0);
		static NVBDirExportOptions getOptions(QWidget * parent = 0);
		NVBDirExportOptions options() const;
	private slots:
		void fileSelected();
	};

#endif // NVBBROWSERHELPERS_H
