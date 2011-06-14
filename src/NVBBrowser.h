/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov   *
 *   Timofey.Balashov@pi.uka.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NVB_BROWSER_H
#define NVB_BROWSER_H

// #define FULL_PATH 1
// #define INDEX_PATH 2

#include <QWidget>

#include "NVBFile.h"
#include "NVBDirModel.h"
#include "NVBDirViewModel.h"
#include "NVBDirView.h"
#include "NVBPageInfoView.h"

#include <QSettings>
#include <QSplitter>
#if QT_VERSION >= 0x040300
#include <QMdiSubWindow>
#endif
#include <QDir>
#include <QMenu>
#include <QToolButton>
#include <QComboBox>
#include <QMessageBox>
#include <QToolBar>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QPixmap>
#include <QListView>
// #include <QItemDelegate>
#include <QApplication>

//#define NVB_BROWSER_ICONSIZE 64

class NVBFileListView;

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
public:
    NVBFolderInputDialog (QWidget * parent = 0);
    static bool getFolder(QString & label, QString & dirname, bool & includeSubfolders);

    QString getDir();
    QString getName();
    bool getIncludeSubfolders();
private slots:
    void dirSelected();
    bool checkInput(  );
    void tryAccept();
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
  NVBDirExportDialog (QWidget * parent = 0);
  static NVBDirExportOptions getOptions(QWidget * parent = 0);
  NVBDirExportOptions options() const;
private slots:
  void fileSelected();
};

class NVBPageRefactorModel;

/**
 * \brief Novembre browser window
 *
 * A browser is like a sofisticated open dialog. It's purpose is to let
 * user select the needed file with more ease. It has thumbnails and
 * other different means of making the contents of the files clearer.
 *
 * The class shall use the isSTMFile and loadSTMFile for file lists
 */
#if QT_VERSION >= 0x040300
class NVBBrowser: public QMdiSubWindow {
#else
class NVBBrowser: public QFrame {
#endif
Q_OBJECT
private:
  QToolBar * foldersToolBar;
//    QAction * plusRootAction;
//    QAction * plusContainerAction;
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
//     QToolButton * plusButton;
  QActionGroup * iconSizeActionGroup;

	QMenu * foldersMenu;
	QMenu * columnsMenu;
	QMenu * remColumnsMenu;

  NVBFileFactory * files;

	NVBFileListView * fileList;
	NVBPageInfoView * piview;
	NVBPageRefactorModel * pageRefactor;
	NVBDirModel * fileModel;
	NVBDirViewModel * dirViewModel;
	NVBDirView * dirView;

	void fillFolders( QString index, QModelIndex parent);

protected:
  Q_PROPERTY(unsigned short iconSize READ getIconSize());
  unsigned short iconSize;
  QSettings* confile;

  void closeEvent(QCloseEvent *event);
  void populateListLevel(int, QString = QString(), const QModelIndex& parent = QModelIndex());

public:
  NVBBrowser( QWidget *parent = 0, Qt::WindowFlags flags = 0 );
  ~NVBBrowser();

  virtual void populateList();
  unsigned short getIconSize() { return iconSize;}
  
  virtual QSize sizeHint () const { return confile->value("Browser/Size", QSize(400, 300)).toSize(); }

public slots:

	void showItems();
  void loadPage(const QModelIndex & item);

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

	void moveColumn(int,int,int);
	void columnAction();
	void populateColumnsMenu();
	void showColumnsMenu() {columnsMenu->exec(QCursor::pos());}

	virtual void showEvent ( QShowEvent * event ) { event->accept(); emit shown(); }

signals:
	void shown();
	void closeRequest();
	void pageRequest(const NVBAssociatedFilesInfo &, int pagenum);
};

#endif
