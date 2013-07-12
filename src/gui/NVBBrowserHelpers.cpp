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
#include "NVBBrowserHelpers.h"
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QAction>
#include <QToolButton>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStyle>

NVBFileSystemDirEntry::NVBFileSystemDirEntry(NVBFileSystemDirEntry * _parent, QString nname)
	: name(nname)
	, parent(_parent)
	, populated(false) {
	if (!parent || parent->name.isNull())
		path = nname + QDir::separator();
	else
		path = parent->path + nname + QDir::separator();

	finfo = QFileInfo(path);

	}

NVBFileSystemDirEntry::~NVBFileSystemDirEntry() {
	foreach(NVBFileSystemDirEntry * e, subs)
	delete e;
	}

void NVBFileSystemDirEntry::populate() {
	if (populated) return;

	QDir d(path);

	foreach(QString s, d.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable , QDir::Name)) {
		subs.append(new NVBFileSystemDirEntry(this, s));
		}

	populated = true;
	}

NVBFileSystemDirModel::NVBFileSystemDirModel(QObject * parent)
	: QAbstractItemModel(parent)
//	, iconFactory(new QFileIconProvider())
	{
	QFileInfoList drives = QDir::drives();

#ifdef Q_WS_WIN
	foreach(QFileInfo drive, drives) {
		QString s = drive.filePath();
		s.chop(1);
		rootEntry.subs.append(new NVBFileSystemDirEntry(&rootEntry, s));
		}
	rootEntry.populated = true;
#else
	rootEntry.path = QString("/");
	rootEntry.name = QString("/");
	rootEntry.finfo = QFileInfo("/");
	rootEntry.populate();
#endif
	}

QVariant NVBFileSystemDirModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) {
#ifndef Q_WS_WIN

		if (role == Qt::ToolTipRole)
			return QString("/");

		if (role == Qt::DecorationRole)
			return iconFactory.icon(rootEntry.finfo);

#endif
		return QVariant();
		}

	switch (role) {
		case Qt::DisplayRole:
		case Qt::EditRole:
			return static_cast<NVBFileSystemDirEntry*>(index.internalPointer())->subs.at(index.row())->name;

		case Qt::ToolTipRole :
			return static_cast<NVBFileSystemDirEntry*>(index.internalPointer())->subs.at(index.row())->path;

		case Qt::DecorationRole :
			return iconFactory.icon(static_cast<NVBFileSystemDirEntry*>(index.internalPointer())->subs.at(index.row())->finfo);

		default:
			return QVariant();
		}
	}

QModelIndex NVBFileSystemDirModel::index(int row, int column, const QModelIndex &parent) const {
	if (column > 0) return QModelIndex();

	if (!parent.isValid()) return createIndex(row, 0, (void*)(&rootEntry));

	NVBFileSystemDirEntry * e = static_cast<NVBFileSystemDirEntry*>(parent.internalPointer());
	return createIndex(row, column, (void*)(e->subs[parent.row()]));
	}

QModelIndex NVBFileSystemDirModel::parent(const QModelIndex &index) const {
	if (!index.isValid()) return QModelIndex();

	NVBFileSystemDirEntry * e = static_cast<NVBFileSystemDirEntry*>(index.internalPointer());

	if (!e->parent)
		return QModelIndex();

	return createIndex(e->parent->subs.indexOf(e->parent), 0, (void*)(e->parent));
	}

Qt::ItemFlags NVBFileSystemDirModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}

int NVBFileSystemDirModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid()) return rootEntry.subs.count();

	NVBFileSystemDirEntry * e = static_cast<NVBFileSystemDirEntry*>(parent.internalPointer())->subs[parent.row()];

	if (!e->populated)
		e->populate();

	return e->subs.count();
	}

bool NVBFileSystemDirModel::hasChildren(const QModelIndex &parent) const {
	if (!parent.isValid()) return true;

	NVBFileSystemDirEntry * e = static_cast<NVBFileSystemDirEntry*>(parent.internalPointer())->subs[parent.row()];
	return !e->populated || e->subs.count() > 0;
	}

NVBDirCompleter::NVBDirCompleter(QObject * parent) : QCompleter(parent) {
	setModel(new NVBFileSystemDirModel(this));
	setModelSorting(QCompleter::CaseSensitivelySortedModel);
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(QCompleter::PopupCompletion);
	}

QString NVBDirCompleter::pathFromIndex(const QModelIndex &index) const {
	QString s = index.data(Qt::ToolTipRole).toString();
	s.chop(1); // separator
	return s;
	}

QStringList NVBDirCompleter::splitPath(const QString &path) const {
	QStringList l = path.split(QDir::separator());
#ifndef Q_WS_WIN
	l.removeFirst();
#endif
	return l;
	}

QValidator::State NVBPathValidator::validate(QString & s, int &) const {
	if (s.isEmpty())
		return QValidator::Intermediate;

	if (d.exists(s))
		return QValidator::Acceptable;

	int i = s.lastIndexOf(d.separator());

#ifdef Q_WS_WIN

	if (i == -1 || d.exists(s.left(i)))
#else
	if (i == 0 || d.exists(s.left(i)))
#endif
		return QValidator::Intermediate;

	return QValidator::Invalid;
	}

NVBFolderInputDialog::NVBFolderInputDialog(QWidget * parent):
	QDialog(parent) {
	setModal(true);
	setWindowTitle("Create new folder");

	buildUi();
	}

NVBFolderInputDialog::NVBFolderInputDialog(QString label, QString dirname, bool includeSubfolders, QWidget *parent)
	: QDialog(parent) {
	setModal(true);
	setWindowTitle("Create new folder");

	buildUi();
	nameEdit->setText(label);

	if (!dirname.isEmpty())
		dirEdit->setText(dirname);
	else
		dirLabel->setChecked(false);

	subfolderCheck->setChecked(includeSubfolders);
	}

void NVBFolderInputDialog::buildUi() {
	resize(378, 109);

	QGridLayout * gridLayout = new QGridLayout(this);

	nameLabel = new QLabel("&Label", this);
	nameLabel->setTextFormat(Qt::PlainText);
	nameLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
	gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

	nameEdit = new QLineEdit(this);
	nameLabel->setBuddy(nameEdit);
	connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(checkInput()));
	gridLayout->addWidget(nameEdit, 0, 1, 1, 3);

	dirLabel = new QCheckBox("&Path", this);
	dirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dirLabel->setChecked(true);
	connect(dirLabel, SIGNAL(toggled(bool)), this, SLOT(checkInput()));
	gridLayout->addWidget(dirLabel, 1, 0, 1, 1);

	dirEdit = new QLineEdit(this);
	dirEdit->setCompleter(new NVBDirCompleter(dirEdit));
	dirEdit->setValidator(new NVBPathValidator(this));
	connect(dirLabel, SIGNAL(toggled(bool)), dirEdit, SLOT(setEnabled(bool)));
	connect(dirEdit, SIGNAL(editingFinished()), this, SLOT(checkInput()));
	gridLayout->addWidget(dirEdit, 1, 1, 1, 2);

	folderSelectButton = new QToolButton(this);
	folderSelectAction = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), "Select folder", folderSelectButton);
	folderSelectButton->setDefaultAction(folderSelectAction);
	connect(dirLabel, SIGNAL(toggled(bool)), folderSelectButton, SLOT(setEnabled(bool)));

	fileDialog = new QFileDialog();
	fileDialog->setFileMode(QFileDialog::Directory);
#if QT_VERSION >= 0x040500
	fileDialog->setOption(QFileDialog::ShowDirsOnly, true);
#endif
	connect(folderSelectAction, SIGNAL(triggered(bool)), fileDialog, SLOT(exec()));
	connect(fileDialog, SIGNAL(accepted()), this, SLOT(dirSelected()));

	gridLayout->addWidget(folderSelectButton, 1, 3, 1, 1);

	subfolderCheck = new QCheckBox("&Include subfolders", this);
	connect(dirLabel, SIGNAL(toggled(bool)), subfolderCheck, SLOT(setEnabled(bool)));
	gridLayout->addWidget(subfolderCheck, 2, 0, 1, 2);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	gridLayout->addWidget(buttonBox, 2, 2, 1, 2);
	}

bool NVBFolderInputDialog::getFolder(QString & label, QString & dirname, bool & includeSubfolders) {

	NVBFolderInputDialog * dialog = new NVBFolderInputDialog();
	dialog->setWindowTitle("Create new folder");

	int result = dialog->exec();

	if (result == QDialog::Accepted) {
		dirname = dialog->getDir();
		label = dialog->getName();
		includeSubfolders = dialog->getIncludeSubfolders();
		delete dialog;
		return true;
		}
	else {
		delete dialog;
		return false;
		}
	}

bool NVBFolderInputDialog::editFolder(QString & label, QString & dirname, bool & includeSubfolders) {

	NVBFolderInputDialog * dialog = new NVBFolderInputDialog(label, dirname, includeSubfolders);
	dialog->setWindowTitle("Edit folder parameters");

	int result = dialog->exec();

	if (result == QDialog::Accepted) {
		dirname = dialog->getDir();
		label = dialog->getName();
		includeSubfolders = dialog->getIncludeSubfolders();
		delete dialog;
		return true;
		}
	else {
		delete dialog;
		return false;
		}
	}

void NVBFolderInputDialog::dirSelected() {
	if (dirEdit && fileDialog)
		dirEdit->setText(fileDialog->selectedFiles().first());
	}

QString NVBFolderInputDialog::getDir() {
	if (dirLabel->isChecked())
		return dirEdit->text();
	else
		return QString();
	}

QString NVBFolderInputDialog::getName() {
	return nameEdit->text();
	}

bool NVBFolderInputDialog::getIncludeSubfolders() {
	return subfolderCheck->isEnabled() && subfolderCheck->isChecked();
	}

void NVBFolderInputDialog::checkInput() {
	bool result = true;

	if (nameEdit->text().isEmpty()) {
		if (dirLabel->isChecked()) {
			QString sdir = dirEdit->text();
			nameEdit->setText(sdir.right(sdir.length() - sdir.lastIndexOf(QDir::separator()) - 1));
			}
		else {
			result = false;
			}
		}

	if (dirLabel->isChecked()) {
		QString dir = dirEdit->text();
		result = QDir(dir).exists();
		}

	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);
	}

NVBDirExportDialog::NVBDirExportDialog(QWidget * parent): QDialog(parent) {
	setModal(true);
	setWindowTitle("Export folder");
	resize(378, 109);

	QVBoxLayout * vLayout = new QVBoxLayout(this);
	QHBoxLayout * hLayout = new QHBoxLayout();
	vLayout->addLayout(hLayout);

	QLabel * nameLabel = new QLabel("&Export to", this);
	hLayout->addWidget(nameLabel);

	fileNameEdit = new QLineEdit(this);
	nameLabel->setBuddy(fileNameEdit);
	hLayout->addWidget(fileNameEdit);

	QToolButton * folderSelectButton = new QToolButton(this);
	QAction * folderSelectAction = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), "Select file", folderSelectButton);
	folderSelectButton->setDefaultAction(folderSelectAction);
	hLayout->addWidget(folderSelectButton);

	fileDialog = new QFileDialog(this, "Export");
	connect(folderSelectAction, SIGNAL(triggered(bool)), fileDialog, SLOT(exec()));
	connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileSelected()));

	recursiveOpt = new QCheckBox("Export folders &recursively", this);
	recursiveOpt->setChecked(false);
	vLayout->addWidget(recursiveOpt);

	loadOpt = new QCheckBox("Expand &all subfolders", this);
	loadOpt->setChecked(false);
	loadOpt->setEnabled(false);
	vLayout->addWidget(loadOpt);

	connect(recursiveOpt, SIGNAL(toggled(bool)), loadOpt, SLOT(setEnabled(bool)));

	fullnamesOpt = new QCheckBox("Export &full filenames", this);
	fullnamesOpt->setChecked(false);
	vLayout->addWidget(fullnamesOpt);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	vLayout->addWidget(buttonBox);

	}

NVBDirExportOptions NVBDirExportDialog::getOptions(QWidget * parent) {
	NVBDirExportDialog d(parent);
	NVBDirExportOptions opts;
	opts.valid = false;

	if (d.exec() == QDialog::Rejected)
		return opts;

	opts = d.options();

	if (opts.fileName.isEmpty())
		opts.valid = false;

	return opts;
	}

void NVBDirExportDialog::fileSelected() {
	fileNameEdit->setText(fileDialog->selectedFiles().first());
	}

NVBDirExportOptions NVBDirExportDialog::options() const {
	NVBDirExportOptions o;
	o.valid = true;
	o.fileName = fileNameEdit->text();
	o.recursiveExport = recursiveOpt->isChecked();
	o.loadOnExport = loadOpt->isChecked();
	o.fullFileNames = fullnamesOpt->isChecked();
	return o;
	}

