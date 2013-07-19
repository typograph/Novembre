//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVB_BROWSER_H
#define NVB_BROWSER_H

#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QPropertyAnimation>

#include "NVBFileInfo.h"
#include "NVBDirModel.h"

class NVBFileListView;

class QAction;
class QLabel;
class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QToolButton;
class QFileDialog;
class QToolBar;
class QActionGroup;
class QMenu;
class QSettings;
class QShowEvent;
class QCloseEvent;
class QSplitter;

class NVBFileFactory;
class NVBFileListView;
class NVBPageInfoView;
// class NVBPageRefactorModel;
class NVBDirModel;
class NVBDirViewModel;
class NVBDirView;
class NVBSingleView;


/**
 * \brief Novembre browser window
 *
 * A browser is like a sofisticated open dialog. It's purpose is to let
 * user select the needed file with more ease. It has thumbnails and
 * other different means of making the contents of the files clearer.
 *
 */
class NVBBrowser: public QFrame {
		Q_OBJECT
	private:
		QToolBar * foldersToolBar;
		QAction * addRootFolderAction;
		QAction * addFolderAction;
		QAction * editFolderAction;
		QAction * removeFolderAction;
		QAction * exportFolderAction;
		QAction * adjustColumnsAction;
		QAction * showFiltersAction;
		QAction * clearFiltersAction;
		QAction * showPageInfoAction;
		QAction * setViewFileAction;
		QAction * refreshFoldersContentsAction;

		QAction * miscModeAction;

		QActionGroup * iconSizeActionGroup;

		QMenu * foldersMenu;
		QMenu * columnsMenu;
		QMenu * remColumnsMenu;

		NVBFileFactory * files;

		QSplitter* hSplitter;
		NVBFileListView * fileList;
		NVBPageInfoView * piview;
		NVBDirModel * fileModel;
		NVBDirViewModel * dirViewModel;
		NVBDirView * dirView;
		NVBSingleView * pageView;

		QModelIndex folderMenuTarget;

//	QPropertyAnimation pageViewShowAnimation;

		bool showPagesInBrowser;

		void fillFolders( QString index, QModelIndex parent);

	protected:
		Q_PROPERTY(unsigned short iconSize READ getIconSize())
		unsigned short iconSize;
		QSettings * confile;

//  void closeEvent(QCloseEvent * event);
		void populateListLevel(int, QString = QString(), const QModelIndex& parent = QModelIndex());

	public:
		NVBBrowser( QWidget *parent = 0, Qt::WindowFlags flags = 0 );
		~NVBBrowser();

		virtual void populateList();
		unsigned short getIconSize() { return iconSize; }

		virtual QSize sizeHint () const;

	public slots:

		void showItems();
		void loadPage(const QModelIndex & item);
		void showPageView(NVBDataSet*);
		void hidePageView();

		void addFolder(const QModelIndex & index);
		void switchIconSize(QAction*);

	private slots:

		void setViewType(bool);

		void addRootFolder();
		void addSubfolder();
		void editFolder();
		void removeFolder();
		void exportData();

		void showFoldersMenu();
		void enableFolderActions(const QModelIndex & index);

		void updateFolders();
		void updateColumnsVisibility();
		void updateColumns();

		void moveColumn(int, int, int);
		void columnAction();
		void populateColumnsMenu();
		void showColumnsMenu();

		void switchMode(bool);

//	virtual void showEvent ( QShowEvent * event ) { event->accept(); emit shown(); }

	signals:
//	void shown();
//	void closeRequest();
		void pageRequest(const NVBAssociatedFilesInfo &, int pagenum);
	};

#endif
