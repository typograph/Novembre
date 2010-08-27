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
#include "NVBBrowser.h"
#include "NVBLogger.h"
#include <QHeaderView>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QDirModel>
#include <QCompleter>
#ifdef Q_WS_WIN
#include <QMessageBox>
#endif

#include "NVBColumnDialog.h"
#include "NVBPageRefactorModel.h"

#include <math.h>

#include "../icons/browser.xpm"

NVBBrowser::NVBBrowser( QWidget *parent, Qt::WindowFlags flags)
#if QT_VERSION >= 0x040300
#ifndef NVB_BROWSER_ONLY
    : QMdiSubWindow(parent,flags)
#else
		: QFrame(parent,flags)
#endif
#else
    : QFrame(parent,flags)
#endif
{

  files = qApp->property("filesFactory").value<NVBFileFactory*>();

  confile = qApp->property("NVBSettings").value<QSettings*>();

#ifndef NVB_BROWSER_ONLY
	setWindowTitle("Browser");
#else
	setWindowTitle("Novembre File Browser");
#endif

  if (!(confile->contains("Browser/IconSize")))
    confile->setValue("Browser/IconSize",QVariant(64));

  iconSize = confile->value("Browser/IconSize").toInt();

  QSplitter * hSplitter = new QSplitter(Qt::Horizontal);

  hSplitter->setCursor(Qt::ArrowCursor); // Without that, the cursor is undefined and keeps the entry form

#if QT_VERSION >= 0x040300
  setWidget(hSplitter);
#else
  setLayout(new QVBoxLayout(this));
  layout()->addWidget(hSplitter);
#endif
  
  QFrame * lframe = new QFrame(hSplitter);
  QVBoxLayout * llayout = new QVBoxLayout(lframe);
  llayout->setSpacing(0);
#if QT_VERSION >= 0x040300
  llayout->setContentsMargins(0,0,0,0);
#else
  llayout->setMargin(0);
#endif
  lframe->setLayout(llayout);


  foldersToolBar = new QToolBar( QString("Browser tools") , lframe);

  QToolButton * folders = qobject_cast<QToolButton *>(foldersToolBar->widgetForAction( foldersToolBar->addAction(QIcon(_browser_folders),QString("Change folders"))));

  if (folders) {

    folders->setPopupMode(QToolButton::InstantPopup);
    QMenu * foldersMenu = new QMenu(this);
  
    addRootFolderAction = foldersMenu->addAction(QIcon(_browser_rootplus),QString("Add folder"));
    connect(addRootFolderAction,SIGNAL(triggered()),this,SLOT(addRootFolder()));
  
    addFolderAction = foldersMenu->addAction(QIcon(_browser_plus),QString("Add subfolder"));
    connect(addFolderAction,SIGNAL(triggered()),this,SLOT(addSubfolder()));
  
    removeFolderAction = foldersMenu->addAction(QIcon(_browser_minus),QString("Remove folder"));
    connect(removeFolderAction,SIGNAL(triggered()),this,SLOT(removeFolder()));

    folders->setMenu(foldersMenu);

    }

  refreshFoldersContentsAction = foldersToolBar->addAction(QIcon(_browser_refresh),QString("Refresh file list"));

  foldersToolBar->addSeparator();

  adjustColumnsAction = foldersToolBar->addAction(QIcon(_browser_columns),QString("Edit columns"));
  showFiltersAction = foldersToolBar->addAction(QIcon(_browser_filters),QString("Show data filters"));

  foldersToolBar->addSeparator();

  setViewFileAction = foldersToolBar->addAction(QIcon(_browser_turnspeconoff),QString("Select view mode"));
  setViewFileAction->setCheckable(true);
  connect(setViewFileAction,SIGNAL(toggled(bool)),this,SLOT(setViewType(bool)));

  showPageInfoAction = foldersToolBar->addAction(QIcon(_browser_pageinfo),QString("Info"));
  showPageInfoAction->setCheckable(true);
  showPageInfoAction->setChecked(true);

//// Bottom option
  
//  progressBar = new NVBProgressBar(this);
//  progressBar->hide();
//  connect(progressBar,SIGNAL(wannaBeVisible(bool)),progressBar,SLOT(setVisible(bool)));
//  qApp->setProperty("progressBar",QVariant::fromValue((QObject*)progressBar));

//// Toolbar option

//   progressBar = new NVBProgressBar(this);
//   qApp->setProperty("progressBar",QVariant::fromValue((QObject*)progressBar));
//   progressBarAction = foldersToolBar->addWidget(progressBar);
//   progressBarAction->setVisible(false);
//   connect(progressBar,SIGNAL(wannaBeVisible(bool)),progressBarAction,SLOT(setVisible(bool)));

  foldersToolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

  llayout->addWidget(foldersToolBar);

  try {
    fileModel = new NVBDirModel(files,this);
    }
  catch (int err) {
		NVBCriticalError(QString("DirModel creation failed : error #%1").arg(err));
    }

  connect(showFiltersAction,SIGNAL(triggered()),fileModel,SLOT(showFilterDialog()));
  connect(adjustColumnsAction,SIGNAL(triggered()),fileModel,SLOT(showColumnDialog()));
  connect(refreshFoldersContentsAction,SIGNAL(triggered()),fileModel,SLOT(refresh()));
//   connect(adjustColumnsAction,SIGNAL(triggered()),this,SLOT(updateColumns()));

  if (!confile->contains("Browser/UserColumns")) confile->setValue("Browser/UserColumns",0);
  int nuc = confile->value("Browser/UserColumns").toInt();
  for (int i = 1; i<=nuc; i++) {
    QString s = confile->value(QString("Browser/UserColumn%1").arg(i)).toString();
    int pos = s.indexOf('/');
    if (pos>=0)
      fileModel->addColumn(s.left(pos),s.mid(pos+1));
    }
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  populateList();
  QApplication::restoreOverrideCursor();

  try {
    fileList = new QTreeView(lframe); 
//    fileList->setMinimumSize(100,1);
    fileList->resize(confile->value("Browser/FileListSize", QSize(50, 100)).toSize());
//    fileList->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    fileList->setRootIsDecorated(true);
    fileList->setAllColumnsShowFocus(true);

    fileList->setModel(fileModel);
    fileList->setSortingEnabled(true);
    fileList->sortByColumn(0,Qt::AscendingOrder);
//     fileList->header()->setMovable(false);
    fileList->header()->setCascadingSectionResizes(true);
    fileList->header()->setStretchLastSection(false);
    fileList->header()->setResizeMode(QHeaderView::ResizeToContents);
//    resizeColumns();

    for (int i = 1; i<=nuc; i++)
      if (!confile->value(QString("Browser/UserColumn%1v").arg(i),true).toBool())
        fileList->hideColumn(i);

    fileList->viewport()->installEventFilter(this);
    fileList->header()->viewport()->installEventFilter(this);
    }
  catch (int err) {
		NVBCriticalError(QString("DirView creation failed : Model logic failed : error #%1").arg(err));
    }

  llayout->addWidget(fileList);  
  llayout->addWidget(progressBar);

  connect(fileList,SIGNAL(activated(const QModelIndex&)), this,SLOT(showItem(const QModelIndex&)));
//   connect(fileList,SIGNAL(expanded(const QModelIndex&)), this,SLOT(resizeColumns()));
  connect(fileList->header(),SIGNAL(sectionMoved(int,int,int)),SLOT(moveColumn(int,int,int)));
//   resizeColumns();
//  connect(fileList,SIGNAL(currentChanged(QModelIndex&,QModelIndex&)),this,SLOT(showItem(QModelIndex&)));
//  connect(fileList,SIGNAL(expanded(QTreeWidgetItem*)),this,SLOT(expandItem(QTreeWidgetItem*)));
//  connect(fileList,SIGNAL(collapsed(QTreeWidgetItem*)),this,SLOT(collapseItem(QTreeWidgetItem*)));

  hSplitter->setStretchFactor(0,0);

  QSplitter * vSplitter = new QSplitter(Qt::Vertical,hSplitter);

  theFile = NULL;


//	pageRefactor = new NVBPageRefactorModel();

//  QMenu * iconSizeMenu = new QMenu(this);

  dirView = new NVBDirView(vSplitter);
  dirView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  dirView->setSelectionMode(QAbstractItemView::SingleSelection);
  dirView->setSelectionBehavior(QAbstractItemView::SelectItems);
  dirView->setContextMenuPolicy(Qt::ActionsContextMenu);
  dirView->setDragDropMode(QAbstractItemView::DragOnly);
  dirView->setGridSize(QSize(iconSize + 20, iconSize + 40));
  dirView->setIconSize(QSize(iconSize, iconSize));
  dirView->hide();
  dirViewModel = 0;

	iconSizeActionGroup = new QActionGroup(this);
	dirView->insertAction(0,iconSizeActionGroup->addAction("512x512"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("256x256"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("128x128"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("64x64"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("32x32"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("16x16"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("Custom..."));

	QAction * tempAction = new QAction(this);
	tempAction->setSeparator(true);

	dirView->insertAction(0,tempAction);

	foreach (QAction * a, iconSizeActionGroup->actions())
		a->setCheckable(true);

	switch (iconSize) {
		case 512 :
		case 256 :
		case 128 :
		case 64 :
		case 32 :
		case 16 : {
			iconSizeActionGroup->actions()[9-(int)log2(iconSize)]->setChecked(true);
			break;
			}
		default : {
			iconSizeActionGroup->actions().last()->setText(QString("Custom (%1)").arg(iconSize));
			iconSizeActionGroup->actions().last()->setChecked(true);
			}
		}

	connect(iconSizeActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(switchIconSize(QAction*)));

  connect(dirView,SIGNAL(activated(const QModelIndex&)), this,SLOT(loadPage(const QModelIndex&)));

  piview = new NVBPageInfoView(vSplitter);
  connect(fileList,SIGNAL(activated(const QModelIndex&)), piview, SLOT(clearView()));
  connect(dirView,SIGNAL(clicked(const QModelIndex &)),piview,SLOT(showPage(const QModelIndex &)));

  dirView->insertAction(0,showPageInfoAction);
  connect(showPageInfoAction,SIGNAL(toggled(bool)),piview,SLOT(setVisible(bool)));

  if (!confile->contains("Browser/ShowPageInfo")) confile->setValue("Browser/ShowPageInfo",true);
  showPageInfoAction->setChecked(confile->value("Browser/ShowPageInfo").toBool());

  vSplitter->setStretchFactor(1,0);

//  fileListMenu = new QPopupMenu(this,"fileListMenu");
  
#ifndef NVB_BROWSER_ONLY
  resize(confile->value("Browser/Size", QSize(400, 300)).toSize());
  move(confile->value("Browser/Pos", QPoint(0, 0)).toPoint());
#endif
   
}

void NVBBrowser::populateList()
{

  confile->beginGroup("Browser");

  confile->beginGroup("Folders");
  if (!confile->contains("_count"))  confile->setValue("_count",0);

  int n0Dir = confile->value("_count").toInt();
  populateListLevel(n0Dir,"_");

  confile->endGroup(); // Folders
  confile->endGroup(); // Browser

}

/// This function works on nonsorted model
void NVBBrowser::populateListLevel(int count, QString index, const QModelIndex & parent) {

  int i;
  QString path, label;

  QModelIndex tIndex;

  for (i=1;i<=count;i++) {

    QString label = confile->value(QString("%1%2_label").arg(index).arg(i)).toString();
    QString path = confile->value(QString("%1%2_path").arg(index).arg(i)).toString();

    if ( label.isEmpty()  && path.isEmpty() ) {
			NVBOutputError(QString("Confile error: no entry for #%1%2").arg(index).arg(i));
      break;
      }

    int s_count = confile->value(QString("%1%2_count").arg(index).arg(i)).toInt();

    if (label.isEmpty()) label = path;

    tIndex = fileModel->addFolderItem(label,path, (s_count == -1),parent);
    if (s_count > 0)
      populateListLevel(s_count,QString("%1%2_").arg(index).arg(i),tIndex);
    }
}

void NVBBrowser::closeEvent(QCloseEvent *event)
{
  event->accept();
//  event->ignore();
  emit closeRequest();
}

NVBBrowser::~ NVBBrowser( )
{
  updateColumnsVisibility();
  updateColumns();
#ifndef NVB_BROWSER_ONLY
	confile->setValue("Browser/Size", size());
  confile->setValue("Browser/Pos", pos());
#endif
  confile->setValue("Browser/FileListSize", fileList->size());
  confile->setValue("Browser/ShowPageInfo", piview->isVisible());
  if (fileModel) delete fileModel;
  if (theFile) theFile->release();
}

void NVBBrowser::showItem( const QModelIndex & item ) {

  if (fileModel->isAFile(item)) {
		dirView->setModel(0);

		if (dirViewModel) {
      delete dirViewModel;
      dirViewModel = 0;
      }

    if (theFile) {
      theFile->release();
      theFile = NULL;
      pageRefactor->setModel(NULL);
      } 

		theFile = files->openFile(fileModel->getAllFiles(item));

    if (theFile) {
      theFile->use();
      }
    else {
			NVBOutputError(QString("Couldn't get file %1").arg(fileModel->getAllFiles(item).name()));
      }

		dirView->setModel(theFile);

    }
  else {
    if (dirViewModel) delete dirViewModel;
    dirViewModel = new NVBDirViewModel(files, QPersistentModelIndex(item.sibling(item.row(),0)), fileModel, this);
    dirView->setModel(dirViewModel);
    connect(dirView,SIGNAL(dataWindow(int,int)),dirViewModel,SLOT(defineWindow(int,int)));
    }
}

void NVBBrowser::loadPage( const QModelIndex & item )
{
  if (dirView->isVisible())
    emit pageRequest(item.parent().data(Qt::ToolTipRole).toString(),item.row());
  else
		emit pageRequest(theFile->sources(),item.row());
}

void NVBBrowser::switchIconSize( QAction* action ) {
  QString str = action->iconText();
  if (str.indexOf('x')>=0) {
    str.truncate(str.indexOf('x'));
    iconSize = str.toInt();
    }
  else {
    bool ok;
    iconSize = QInputDialog::getInteger(this,"Custom icon size","Enter icon size in pixels:",iconSize,1,1024,1,&ok);
    if (!ok) return;
    switch (iconSize) {
      case 512 :
      case 256 :
      case 128 :
      case 64 :
      case 32 :
      case 16 : {
        iconSizeActionGroup->actions()[9-(int)log2(iconSize)]->setChecked(true);
        break;
        }
      default : {
        iconSizeActionGroup->actions().last()->setText(QString("Custom (%1)").arg(iconSize));
//        iconSizeActionGroup->actions().last()->setChecked(true);
        }
      }
    }

  confile->setValue("Browser/IconSize",iconSize);
  dirView->setGridSize(QSize(iconSize + 20, iconSize + 40));
  dirView->setIconSize(QSize(iconSize, iconSize));
	NVBOutputPMsg(QString("Switched to %1").arg(iconSize,3));
/*  
  if (fileList->selectionModel()->selectedIndexes().size()) {
//    selection->loadIcons(iconSize);
    theFile->reloadImages(QSize(iconSize, iconSize));
    }
*/
  }

void NVBBrowser::updateFolders( )
{
  confile->remove( "Browser/Folders" );
  confile->beginGroup( "Browser/Folders" );
  fillFolders("_",QModelIndex());
  confile->endGroup();
}

/// Works on non-sorted Model
void NVBBrowser::fillFolders( QString index, QModelIndex parent)
{  
  int cnt = fileModel->folderCount(parent, true);
  
  if (cnt != 0) confile->setValue(QString("%1count").arg(index),cnt);

  if (cnt > 0) {

    for (int i = 1; i<=cnt ; i++) {
      QString new_index = QString("%1%2_").arg(index).arg(i);
      QModelIndex new_parent = fileModel->index(i-1,0,parent);

      QString str = fileModel->data(new_parent,Qt::DisplayRole).toString();
      confile->setValue(QString("%1label").arg(new_index),str);

      str = fileModel->getFullPath(new_parent);
      if (!str.isNull()) confile->setValue(QString("%1path").arg(new_index),str);

      fillFolders(new_index,new_parent);
      }

    }
  
}

NVBFolderInputDialog::NVBFolderInputDialog( QWidget * parent ):
QDialog(parent)
{  
  setModal(true);
  setWindowTitle("Create new folder");
  resize(378, 109);

  QGridLayout * gridLayout = new QGridLayout(this);

  nameLabel = new QLabel("&Label",this);
  nameLabel->setTextFormat(Qt::PlainText);
  nameLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

  nameEdit = new QLineEdit(this);
  nameLabel->setBuddy(nameEdit);
#ifndef Q_WS_WIN
  connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(checkInput()));
#endif
  gridLayout->addWidget(nameEdit, 0, 1, 1, 3);

  dirLabel = new QCheckBox("&Path",this);
  dirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  dirLabel->setChecked(true);
#ifndef Q_WS_WIN
  connect(dirLabel,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
#endif
  gridLayout->addWidget(dirLabel, 1, 0, 1, 1);

  dirEdit = new QLineEdit(this);
  QCompleter *completer = new QCompleter(this);
  completer->setModel(new QDirModel(QStringList(),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable |  QDir::Executable, QDir::Name,completer));
  dirEdit->setCompleter(completer);
  connect(dirLabel, SIGNAL(toggled(bool)), dirEdit, SLOT(setEnabled(bool)));
#ifndef Q_WS_WIN
  connect(dirEdit,SIGNAL(textChanged(QString)),this,SLOT(checkInput()));
#endif
  gridLayout->addWidget(dirEdit, 1, 1, 1, 2);


  folderSelectButton = new QToolButton(this);
  folderSelectAction = new QAction(QIcon(_browser_plus),"Select folder",folderSelectButton);
  folderSelectButton->setDefaultAction(folderSelectAction);
  connect(dirLabel, SIGNAL(toggled(bool)), folderSelectButton, SLOT(setEnabled(bool)));

  fileDialog = new QFileDialog();
  fileDialog->setFileMode(QFileDialog::Directory);
#if QT_VERSION >= 0x040500  
  fileDialog->setOption(QFileDialog::ShowDirsOnly,true);
#endif
  connect(folderSelectAction,SIGNAL(triggered(bool)),fileDialog,SLOT(exec()));
  connect(fileDialog,SIGNAL(accepted()),this,SLOT(dirSelected()));

  gridLayout->addWidget(folderSelectButton, 1, 3, 1, 1);

  subfolderCheck = new QCheckBox("&Include subfolders",this);
  connect(dirLabel, SIGNAL(toggled(bool)), subfolderCheck, SLOT(setEnabled(bool)));
  gridLayout->addWidget(subfolderCheck, 2, 0, 1, 2);

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(tryAccept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  gridLayout->addWidget(buttonBox, 2, 2, 1, 2);

}

bool NVBFolderInputDialog::getFolder( QString & label, QString & dirname, bool & includeSubfolders)
{

  NVBFolderInputDialog * dialog = new NVBFolderInputDialog();

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

void NVBFolderInputDialog::dirSelected( )
{
  if (dirEdit && fileDialog) {
    dirEdit->setText(fileDialog->selectedFiles().first());
    }
}

QString NVBFolderInputDialog::getDir( )
{
  if (dirLabel->isChecked())
    return dirEdit->text();
  else
    return QString();
}

QString NVBFolderInputDialog::getName( )
{
  return nameEdit->text();
}

bool NVBFolderInputDialog::getIncludeSubfolders()
{
  return subfolderCheck->isEnabled() && subfolderCheck->isChecked();
}

void NVBFolderInputDialog::tryAccept( )
{
  bool success = checkInput();

  if (success) { 
    if (dirLabel->isChecked() && nameEdit->text().isEmpty())
      nameEdit->setText(dirEdit->text());
    accept();
    }
}

bool NVBFolderInputDialog::checkInput( )
{
  bool result = true;

  if (nameEdit->text().isEmpty()) {
#ifdef Q_WS_WIN
    QMessageBox::warning(this, QString(), QString("The label cannot be empty"), QMessageBox::Ok, QMessageBox::Ok);
#else
    QPalette p = dirEdit->palette();
    p.setColor(QPalette::Inactive,QPalette::Base,QColor(0xDD7777));
    p.setColor(QPalette::Active,QPalette::Base,QColor(0xDD7777));
    nameEdit->setPalette(p);
#endif
    result = false;
    }
  else
    nameEdit->setPalette(QApplication::palette());

  if (dirLabel->isChecked()) {
    QString dir = dirEdit->text();
    if (!(QDir(dir).exists())) {
#ifdef Q_WS_WIN
      QMessageBox::warning(this, QString(), QString("The folder %1 does not exist").arg(dir), QMessageBox::Ok, QMessageBox::Ok);
#else
      QPalette p = dirEdit->palette();
      p.setColor(QPalette::Inactive,QPalette::Base,QColor(0xDD7777));
      p.setColor(QPalette::Active,QPalette::Base,QColor(0xDD7777));
      dirEdit->setPalette(p);
#endif
      result = false;
      }
    else {
#ifndef Q_WS_WIN
      nameEdit->setPalette(QApplication::palette());
      dirEdit->setPalette(QApplication::palette());
#endif
      result = true;
      }
    }

  return result;
}

void NVBBrowser::resizeColumns() {
    int nuc = confile->value("Browser/UserColumns").toInt();
    for (int i = 0; i<=nuc; i++)
      fileList->resizeColumnToContents(i);
//    fileList->setColumnWidth(1,0);
//    fileList->resizeColumnToContents(2);
//    fileList->resizeColumnToContents(3);
}

bool NVBBrowser::eventFilter(QObject * obj, QEvent * event) {
//  if (fileList != qobject_cast<QTreeView*>(obj)) return false;
  if (event->type() != QEvent::MouseButtonPress) return false;
  QMouseEvent * e = (QMouseEvent*)(event);
  if (!e) return false;
  if (!(e->button() & Qt::RightButton)) return false;
  if (fileList->header()->viewport() == dynamic_cast<void*>(obj)) {
    QMenu * clmns = new QMenu("Show columns",fileList->header()->viewport());
    QMenu * rm_clmns = new QMenu("Remove column",clmns);
    QAction * a;
    for (int i = 1; i < fileModel->columnCount(); i++) {
      int j = fileList->header()->logicalIndex(i);
      a = rm_clmns->addAction(fileModel->headerData(j,Qt::Horizontal,Qt::DisplayRole).toString());
      a->setData(-j);
      a = clmns->addAction(fileModel->headerData(j,Qt::Horizontal,Qt::DisplayRole).toString());
      a->setCheckable(true);
      if (!fileList->isColumnHidden(j)) a->setChecked(true);
      a->setData(j);
      }
    clmns->addSeparator();
    a = clmns->addAction("Add column");
    clmns->addMenu(rm_clmns);
//    connect(clmns,SIGNAL(triggered(QAction*)),SLOT(columnAction(QAction*)));
    columnAction(clmns->exec(e->globalPos()));
    delete clmns;
//     resizeColumns();
    return true;
    }
  else {
    QMenu * folders = new QMenu("Folders",fileList->viewport());
    QModelIndex index = fileList->indexAt(e->pos());
    if (index.isValid()) {
      if (fileModel->isAFile(index)) {
        return false;
        }
      else {
        folders->addAction(addFolderAction);
        folders->addAction(removeFolderAction);
        folders->addSeparator();
        folders->addAction("Export data");
//         folders->addAction(exportFolderAction);
        }
      }
    else
      folders->addAction(addRootFolderAction);
    folders->addSeparator();
    folders->addAction("Filter data");
    folders->addAction("Remove filter");
    folderAction(fileModel->index(index.row(),0,index.parent()),folders->exec(e->globalPos()));
    delete folders;
//     resizeColumns();
    return false; // we want the selection to propagate
    }
}

void NVBBrowser::columnAction(QAction * action) {
  if (!action) return;
  if (action->text() == "Add column") {
    QString s = NVBColumnInputDialog::getColumn();

    if (s.isEmpty()) return;

    int nuc = confile->value("Browser/UserColumns").toInt();
    confile->setValue("Browser/UserColumns",nuc+1);
    confile->setValue(QString("Browser/UserColumn%1").arg(nuc+1),s);

    int pos = s.indexOf('/');
    fileModel->addColumn(s.left(pos),s.mid(pos+1));
    }
  else if (action->data().toInt() > 0) {
    if (action->isChecked())
      fileList->showColumn(action->data().toInt());
    else
      fileList->hideColumn(action->data().toInt());
    }
  else {
    int i = -(action->data().toInt());
    int j = fileList->header()->visualIndex(i);
    fileList->hideColumn(i);
    fileModel->removeColumn(i);
    confile->beginGroup("Browser");
    int nuc = confile->value("UserColumns").toInt();
    confile->setValue("UserColumns",nuc-1);
    for (; j < nuc; j++) { // Settings follow the visual model, therefore they have to have visualIndex
      QString clmn = confile->value(QString("UserColumn%1").arg(j+1)).toString();
      confile->setValue(QString("UserColumn%1").arg(j),clmn);
      confile->setValue(QString("UserColumn%1v").arg(j),!fileList->isColumnHidden(j));
      }
    confile->remove(QString("UserColumn%1").arg(j));
    confile->remove(QString("UserColumn%1v").arg(j));
    confile->endGroup();
    }
}

void NVBBrowser::addFolder(const QModelIndex & index) {

  QString dirname;
  QString label;

  bool followSubfolders;

  if (NVBFolderInputDialog::getFolder(label,dirname,followSubfolders)) {
    fileModel->addFolderItem(label,dirname,followSubfolders,index);
    updateFolders();
    }

}

/*
NVBColumnDialog::NVBColumnDialog() {
  QHBoxLayout * h = new QHBoxLayout(this);
  QVBoxLayout * e = new QVBoxLayout();
  QGridLayout * l = new QGridLayout();

  h->addLayout(e);
  e->addLayout(l);

  l->addWidget(new QLabel("Name",this),0,0,Qt::AlignRight);
  l->addWidget(new QLabel("Contents",this),1,0,Qt::AlignRight);

  nameEdit = new QLineEdit(this);
  l->addWidget(nameEdit,0,1,Qt::AlignHCenter);
  cntEdit = new QLineEdit(this);
  l->addWidget(cntEdit,1,1,Qt::AlignHCenter);

  QToolButton * tb = new QToolButton(this);
  tb->setDefaultAction(new QAction("?",tb));
  tb->defaultAction()->setToolTip("Show help on possible field values");
  tb->defaultAction()->setCheckable(true);
  l->addWidget(tb,1,2,Qt::AlignLeft);

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox,SIGNAL(accepted()),this,SLOT(tryAccept()));
  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

  e->addWidget(buttonBox);

  QLabel * help = new QLabel("The \"Contents\" field may contain one of the following:<ol><li>Predefined file parameter names, i.e. <b>fileName</b>, <b>fileSize</b>, <b>fileDate</b>, <b>fileTime</b>, <b>fileDateTime</b>.</li><li>Predefined page paremeter names, i.e. <b>Name</b>, <b>Size</b>, <b>isTopo</b>, <b>isSpec</b>.</li><li>Page comment names, e.g. <b>Bias</b> or <b>Setpoint</b>. Please bear in mind that those are file format-specific.</li><li>Page selectors in the form \"?T\", \"?S\" or \"?!\", followed by any of the following:<ul><li>Free text, output as given.</li><li>Page paremeters as above, parenthesised.</li><li>One of <b>$X</b> (x size), <b>$Y</b> (y size) or <b>$$</b> for $.</li></ul></li></ol>");
  help->hide();
  h->addWidget(help);

  connect(tb,SIGNAL(toggled(bool)),help,SLOT(setVisible(bool)));

  setLayout(h);
}

QString NVBColumnDialog::getColumn()
{
  int result;
  NVBColumnDialog * dialog  = new NVBColumnDialog();
  result = dialog->exec();
  QString clmncode = dialog->nameEdit->text()+"/"+dialog->cntEdit->text();
  delete dialog;
  if (result == QDialog::Accepted)
    return clmncode;
  return QString();
//  if (ok) *ok = (result == QDialog::Accepted);
}

void NVBColumnDialog::tryAccept() {

  // TODO One should check "contents" validity here.
  // Unfortunately it depends greatly on NVBFileInfo, so maybe a method is needed there.

  accept();
}

*/

void NVBBrowser::updateColumnsVisibility() {
  int nuc = confile->value("Browser/UserColumns").toInt();
  for (int i = 1; i<=nuc; i++) {
    confile->setValue(QString("Browser/UserColumn%1v").arg(i),!(fileList->isColumnHidden(i)));
    }
}

void NVBBrowser::moveColumn( int logicalIndex, int oldVisualIndex, int newVisualIndex )
{
  Q_UNUSED(logicalIndex);
  static int invalidMoveIndex = 0;

  if (oldVisualIndex == newVisualIndex) return;

  if (newVisualIndex == 0) {
    if (invalidMoveIndex == 0) {
      invalidMoveIndex = oldVisualIndex;
      fileList->header()->moveSection(0,1);
      return;
      }
    else {
      invalidMoveIndex = 0;
      return;
      }
    }
  else if (oldVisualIndex == 0) {
    if (invalidMoveIndex == 0) {
      invalidMoveIndex = 117; // Dummy
      fileList->header()->moveSection(newVisualIndex,oldVisualIndex);
      return;
      }
    else {
      oldVisualIndex = invalidMoveIndex;
      invalidMoveIndex = 0;
      // Fall through
      }
    }

  int shift = oldVisualIndex>newVisualIndex ? -1 : 1;
  QVariant tmp = confile->value(QString("Browser/UserColumn%1").arg(oldVisualIndex));
  for (int i = oldVisualIndex; i != newVisualIndex; i += shift) {
    confile->setValue(QString("Browser/UserColumn%1").arg(i),confile->value(QString("Browser/UserColumn%1").arg(i+shift)));
    confile->setValue(QString("Browser/UserColumn%1v").arg(i),!fileList->isColumnHidden(i));
    }
  confile->setValue(QString("Browser/UserColumn%1").arg(newVisualIndex),tmp);
  confile->setValue(QString("Browser/UserColumn%1v").arg(newVisualIndex),true);
}

/*
NVBDataFilterDialog::NVBDataFilterDialog(QStringList columns)
{
  QHBoxLayout * clmnLayout = new QHBoxLayout();

  clmnLayout->addWidget(new QLabel("Column",this));
  columnCombo = new QComboBox(this);
  columnCombo->addItems(columns);
  columnCombo->setCurrentIndex(1);
  clmnLayout->addWidget(columnCombo);

  QHBoxLayout * filterLayout = new QHBoxLayout();

  filterLayout->addWidget(new QLabel("Filter",this));
  filterEdit = new QLineEdit(this);
  filterLayout->addWidget(filterEdit);

  QVBoxLayout * l = new QVBoxLayout();

  l->addLayout(clmnLayout);
  l->addLayout(filterLayout);
  
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
  connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

  l->addWidget(buttonBox);
  
  setLayout(l);
}

*/
void NVBBrowser::addRootFolder()
{
  addFolder(QModelIndex());
}

void NVBBrowser::addSubfolder()
{
  QModelIndexList ixs = fileList->selectionModel()->selectedRows();

  if (ixs.isEmpty())
    addFolder(QModelIndex());
  else {
    QModelIndex c = ixs.at(0);
    if (fileModel->isAFile(c))
      c = c.parent();
    addFolder(c);
    }
}

void NVBBrowser::removeFolder()
{
  QModelIndexList ixs = fileList->selectionModel()->selectedRows();

  if (!ixs.isEmpty()) {
    QModelIndex c = ixs.at(0);
    if (!fileModel->isAFile(c)) {
      fileModel->removeItem(c);
      updateFolders();
      }
    }
}

/*
void NVBBrowser::editColumns()
{
}

void NVBBrowser::showFilters()
{
}
*/

void NVBBrowser::folderAction(const QModelIndex & index, QAction * action)
{
  if (!action) return;
  if (action->text() == "Add folder") {
    addFolder(QModelIndex());
    }
  else if (action->text() == "Add subfolder") {
    addFolder(index);
    }
  else if (action->text() == "Remove folder") {
    fileModel->removeItem(index);
    updateFolders();
    }
  else if (action->text() == "Filter data") {
    fileModel->showFilterDialog();
/*    QStringList clmns;
    for (int i = 0; i < fileModel->columnCount(); i++) {
      int j = fileList->header()->logicalIndex(i);
      clmns << fileModel->headerData(j,Qt::Horizontal,Qt::DisplayRole).toString();
      }
    NVBDataFilterDialog dialog(clmns);
    int result = dialog.exec();
    if (result == QDialog::Accepted) {
      fileModel->setFilterKeyColumn(dialog.selectedColumn());
      fileModel->setFilterWildcard(dialog.filterText());
      }*/
    }
  else if (action->text() == "Remove filter") {
    fileModel->removeFilters();
//    fileModel->setFilterRegExp(QRegExp());
    }
  else if (action->text() == "Export data") {
    NVBDirExportOptions o = NVBDirExportDialog::getOptions();
    if (o.valid)
      fileModel->exportData(index,o);
    }
  else
    return;
}

void NVBBrowser::updateColumns()
{
  confile->setValue("Browser/UserColumns",fileModel->columnCount()-1);
  for (int i = 1; i<fileModel->columnCount();i++) {
    confile->setValue(QString("Browser/UserColumn%1").arg(i),QString("%1/%2").arg(fileModel->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()).arg(fileModel->headerData(i,Qt::Horizontal,NVBColStrKeyRole).toString()));
    }

}

void NVBBrowser::setViewType(bool b)
{
  pageRefactor->setMode(b ? NVBPageRefactorModel::MarkSpectra : NVBPageRefactorModel::None );
}

NVBDirExportDialog::NVBDirExportDialog( QWidget * parent ):QDialog(parent)
{
  setModal(true);
  setWindowTitle("Export folder");
  resize(378, 109);

  QVBoxLayout * vLayout = new QVBoxLayout(this);
  QHBoxLayout * hLayout = new QHBoxLayout();
  vLayout->addLayout(hLayout);

  QLabel * nameLabel = new QLabel("&Export to",this);
  hLayout->addWidget(nameLabel);
  
  fileNameEdit = new QLineEdit(this);
  nameLabel->setBuddy(fileNameEdit);
  hLayout->addWidget(fileNameEdit);

  QToolButton * folderSelectButton = new QToolButton(this);
  QAction * folderSelectAction = new QAction(QIcon(_browser_plus),"Select file",folderSelectButton);
  folderSelectButton->setDefaultAction(folderSelectAction);
  hLayout->addWidget(folderSelectButton);

  fileDialog = new QFileDialog(this, "Export");
  connect(folderSelectAction,SIGNAL(triggered(bool)),fileDialog,SLOT(exec()));
  connect(fileDialog,SIGNAL(accepted()),this,SLOT(fileSelected()));

  recursiveOpt = new QCheckBox("Export folders &recursively",this);
  recursiveOpt->setChecked(false);
  vLayout->addWidget(recursiveOpt);

  loadOpt = new QCheckBox("Expand &all subfolders",this);
  loadOpt->setChecked(false);
  loadOpt->setEnabled(false);
  vLayout->addWidget(loadOpt);
  
  connect(recursiveOpt,SIGNAL(toggled(bool)),loadOpt,SLOT(setEnabled(bool)));

  fullnamesOpt = new QCheckBox("Export &full filenames",this);
  fullnamesOpt->setChecked(false);
  vLayout->addWidget(fullnamesOpt);

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  vLayout->addWidget(buttonBox);

}

NVBDirExportOptions NVBDirExportDialog::getOptions( QWidget * parent )
{
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

void NVBDirExportDialog::fileSelected( )
{
  fileNameEdit->setText(fileDialog->selectedFiles().first());
}

NVBDirExportOptions NVBDirExportDialog::options( ) const
{
  NVBDirExportOptions o;
  o.valid = true;
  o.fileName = fileNameEdit->text();
  o.recursiveExport = recursiveOpt->isChecked();
  o.loadOnExport = loadOpt->isChecked();
  o.fullFileNames = fullnamesOpt->isChecked();
  return o;
}
