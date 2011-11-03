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

#include <QtCore/QSettings>
#include <QtGui/QSplitter>
// #include <QtGui/QMdiSubWindow>
#include <QtCore/QDir>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QComboBox>
#include <QtGui/QMessageBox>
#include <QtGui/QToolBar>
#include <QtGui/QCloseEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFrame>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QInputDialog>
#include <QtGui/QPixmap>
#include <QtGui/QListView>
#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QFileSystemModel>
#include <QtGui/QCompleter>


#include "NVBColumnDialog.h"
#include "NVBFileListView.h"
#include "NVBPageInfoView.h"
#include "NVBBrowserHelpers.h"
#include "NVBDirModel.h"
#include "NVBDirViewModel.h"
#include "NVBDirView.h"
#include "NVBSettings.h"
#include "NVBSingleView.h"
#include "NVBDataCore.h"
#include "NVBMap.h"
#include "NVBDataTransforms.h"
#include "NVBGradientMenu.h"
#include "NVBMutableGradients.h"
#include "NVBColorMaps.h"

#include <math.h>

#include "../icons/browser.xpm"

class NVBDataSet34Substractor : public NVBFile2ImageConverter {
private:
	virtual QPixmap convertToImage(NVBFile * file) const {
		if (file->isEmpty() || file->first()->dataSets().isEmpty())
			return QPixmap();
		if (file->first()->dataSets().count() != 8 ||
				file->first()->dataSets().at(2)->type() != NVBDataSet::Topography ||
				file->first()->dataSets().at(3)->type() != NVBDataSet::Topography)
			return NVBDataColorInstance::colorize(file->first()->dataSets().first());
		NVBDataSet * d3 = file->first()->dataSets().at(2);
		NVBDataSet * d4 = file->first()->dataSets().at(3);
		NVBColorInstance * ci = new NVBColorInstance(d3->colorMap());
		axissize_t sz = prod(d3->sizes());
		double * data = (double*) malloc(sz*sizeof(double));
		QSize isize = QSize(d3->sizeAt(0),d3->sizeAt(1));
		memcpy(data,d3->data(),sz*sizeof(double));
//		axissize_t w = d3->sizeAt(0);
//		axissize_t mi = NVBMaxMinTransform::max_index(d4->data()+10, 1, &w)+10;
//		double factor = (d3->data()[mi] - (d3->data()[mi+9] + d3->data()[mi-10])/2)/(d4->data()[mi] - (d4->data()[mi+9] + d4->data()[mi-10])/2);
		axissize_t mi = NVBMaxMinTransform::max_index(d4->data(), 1, &sz);
		double factor = 0;
		if (mi < 10)
			factor = (d3->data()[mi] - d3->data()[mi+9])/(d4->data()[mi] - d4->data()[mi+9] );
		else
			factor = (d3->data()[mi] - (d3->data()[mi+9] + d3->data()[mi-10])/2)/(d4->data()[mi] - (d4->data()[mi+9] + d4->data()[mi-10])/2);

		for(axissize_t i = 0; i<sz;i++)
			data[i] -= factor*d4->data()[i];
		ci->autoscale(data,sz);
		QPixmap result = ci->colorize(data,isize,QSize(256,256));
		free(data);
		delete(ci);
		return result;
	}
};

NVBBrowser::NVBBrowser( QWidget *parent, Qt::WindowFlags flags)
: QFrame(parent,flags), showPagesInBrowser(true)
{

  files = qApp->property("filesFactory").value<NVBFileFactory*>();
  if (!files) {
	NVBCriticalError("Browser cannot access the file factory");
	}

  confile = qApp->property("NVBSettings").value<QSettings*>();
  if (!confile) {
	NVBCriticalError("Browser cannot access the configuration file");
	}

	setWindowTitle("Novembre File Browser");

  if (!(confile->contains("Browser/IconSize")))
    confile->setValue("Browser/IconSize",QVariant(64));

  iconSize = confile->value("Browser/IconSize").toInt();

  hSplitter = new QSplitter(Qt::Horizontal,this);

  hSplitter->setCursor(Qt::ArrowCursor); // Without that, the cursor is undefined and keeps the entry form

  setLayout(new QVBoxLayout(this));
  layout()->addWidget(hSplitter);
  
  QFrame * lframe = new QFrame(hSplitter);
  QVBoxLayout * llayout = new QVBoxLayout(lframe);
  llayout->setSpacing(0);
  llayout->setContentsMargins(0,0,0,0);
  lframe->setLayout(llayout);


  foldersToolBar = new QToolBar( QString("Browser tools") , lframe);

  QToolButton * folders = qobject_cast<QToolButton *>(foldersToolBar->widgetForAction( foldersToolBar->addAction(QIcon(_browser_folders),QString("Change folders"))));

  if (folders) {

    folders->setPopupMode(QToolButton::InstantPopup);
		QMenu * foldersBtnMenu = new QMenu(this);
  
		addRootFolderAction = foldersBtnMenu->addAction(QIcon(_browser_rootplus),QString("Add folder"));
    connect(addRootFolderAction,SIGNAL(triggered()),this,SLOT(addRootFolder()));
  
		addFolderAction = foldersBtnMenu->addAction(QIcon(_browser_plus),QString("Add subfolder"));
    connect(addFolderAction,SIGNAL(triggered()),this,SLOT(addSubfolder()));
  
		removeFolderAction = foldersBtnMenu->addAction(QIcon(_browser_minus),QString("Remove folder"));
    connect(removeFolderAction,SIGNAL(triggered()),this,SLOT(removeFolder()));

		folders->setMenu(foldersBtnMenu);

    }

	editFolderAction = new QAction(QString("Edit folder"),this);
	connect(editFolderAction,SIGNAL(triggered()),this,SLOT(editFolder()));

  refreshFoldersContentsAction = foldersToolBar->addAction(QIcon(_browser_refresh),QString("Refresh file list"));

  foldersToolBar->addSeparator();

  adjustColumnsAction = foldersToolBar->addAction(QIcon(_browser_columns),QString("Edit columns"));
  showFiltersAction = foldersToolBar->addAction(QIcon(_browser_filters),QString("Show data filters"));

	QToolButton * generators = qobject_cast<QToolButton *>(foldersToolBar->widgetForAction( foldersToolBar->addAction(QIcon(_browser_generators),QString("Select file formats"))));

	if (generators) {

		generators->setPopupMode(QToolButton::InstantPopup);
		QMenu * generatorsBtnMenu = new QMenu(this);
		if (generatorsBtnMenu) {
			generatorsBtnMenu->addActions(files->generatorActions());
			generators->setMenu(generatorsBtnMenu);
			}
		else
			NVBOutputError("Failed to create a menu");

		}
	else
		NVBOutputError("Toolbutton not created");

  foldersToolBar->addSeparator();


  setViewFileAction = foldersToolBar->addAction(QIcon(_browser_turnspeconoff),QString("Select view mode"));
  setViewFileAction->setCheckable(true);
  connect(setViewFileAction,SIGNAL(toggled(bool)),this,SLOT(setViewType(bool)));

  showPageInfoAction = foldersToolBar->addAction(QIcon(_browser_pageinfo),QString("Info"));
  showPageInfoAction->setCheckable(true);
  showPageInfoAction->setChecked(true);
  
	// FIXME miscmode should have more options and have a drop-down menu.
	miscModeAction = foldersToolBar->addAction(QIcon(_browser_magic),"Switch to magical mode",this,SLOT(switchMode(bool)));
	miscModeAction->setCheckable(true);

	QToolButton * gradientsButton = qobject_cast<QToolButton *>(
				foldersToolBar->widgetForAction(
					foldersToolBar->addAction(QIcon(_browser_gradient),QString("Set default color scheme"))
					)
				);
	NVBGradientMenu * gmenu = new NVBGradientMenu(gradientsButton);
	gradientsButton->setMenu(gmenu);
	gradientsButton->setPopupMode(QToolButton::InstantPopup);
	gmenu->addGradientAction(new NVBGrayRampColorMap());
	gmenu->addGradientAction(new NVBBlackToColorGradientAction(0));
	gmenu->addGradientAction(new NVBBlackToColorToWhiteGradientAction(0));
	gmenu->addGradientAction(new NVBHSVWheelColorMap(0.00555,0.163589,0.96796,0.90137,0.32778,1));
	gmenu->addGradientAction(new NVBRGBMixColorMap(
										 new NVBGrayStepColorMap(
											 QList<double>() << 0 << 0.12549 << 0.360784 << 0.596078 << 0.737255 << 1,
											 QList<double>() << 0 << 0.466667 << 0.776471 << 0.933333 << 0.976471 << 0.976471
											 ),
										 new NVBGrayStepColorMap(
											 QList<double>() << 0 << 0.0666667 << 0.184314 << 0.415686 << 0.713725 << 0.909804 << 1,
											 QList<double>() << 0 << 0 << 0.0823529 << 0.376471 << 0.815686 << 0.972549 << 0.976471
											 ),
										 new NVBGrayStepColorMap(
											 QList<double>() << 0 << 0.333333 << 0.627451 << 0.862745 << 1,
											 QList<double>() << 0 << 0 << 0.298039 << 0.835294 << 0.984314
											 )
										 ));
	gmenu->addSeparator();
	gmenu->addAction(QString("Custom..."))->setEnabled(false);

  foldersToolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

  llayout->addWidget(foldersToolBar);

	fileModel = new NVBDirModel(files,this);

	if (!fileModel)
		NVBCriticalError(QString("DirModel creation failed"));

	exportFolderAction = new QAction(QString("Export data"),this);
	connect(exportFolderAction,SIGNAL(triggered()),this,SLOT(exportData()));

	clearFiltersAction = new QAction(QString("Remove filters"),this);
	clearFiltersAction->setEnabled(false);
	connect(clearFiltersAction,SIGNAL(triggered()),fileModel,SLOT(removeFilters()));
	connect(clearFiltersAction,SIGNAL(triggered(bool)),clearFiltersAction,SLOT(setEnabled(bool)));

  connect(showFiltersAction,SIGNAL(triggered()),fileModel,SLOT(showFilterDialog()));
	connect(showFiltersAction,SIGNAL(triggered(bool)),clearFiltersAction,SLOT(setDisabled(bool)));

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

	fileList = new NVBFileListView(lframe);
	if (!fileList)
		NVBCriticalError(QString("FileList creation failed : Model logic failed"));
	fileList->resize(confile->value("Browser/FileListSize", QSize(50, 100)).toSize());
	fileList->setModel(fileModel);

	for (int i = 1; i<=nuc; i++)
		if (!confile->value(QString("Browser/UserColumn%1v").arg(i),true).toBool())
			fileList->hideColumn(i);

  llayout->addWidget(fileList);  

	connect(fileList,SIGNAL(activated(const QModelIndex&)), this,SLOT(showItems()));
	connect(fileList,SIGNAL(rightPressed(QModelIndex)),this,SLOT(showFoldersMenu()));
	connect(fileList->header(),SIGNAL(sectionMoved(int,int,int)),SLOT(moveColumn(int,int,int)));
	connect(fileList->header(),SIGNAL(sectionRightPressed(int)),this,SLOT(showColumnsMenu()));
	connect(fileList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
	        this,SLOT(enableFolderActions(QModelIndex)));

	foldersMenu = new QMenu("Folders",fileList->viewport());

	columnsMenu = new QMenu("Show columns",fileList->header()->viewport());
	remColumnsMenu = new QMenu("Remove column",columnsMenu);
	populateColumnsMenu();
	connect(fileModel,SIGNAL(columnsInserted(const QModelIndex &, int, int)),this,SLOT(populateColumnsMenu()));
	connect(fileModel,SIGNAL(columnsRemoved(const QModelIndex &, int, int)),this,SLOT(populateColumnsMenu()));
	connect(fileModel,SIGNAL(modelReset()),this,SLOT(populateColumnsMenu()));
	connect(fileModel,SIGNAL(headerDataChanged(Qt::Orientation,int,int)),this,SLOT(populateColumnsMenu()));

  hSplitter->setStretchFactor(0,0);

  QSplitter * vSplitter = new QSplitter(Qt::Vertical,hSplitter);
  hSplitter->setStretchFactor(1,1);

  QAction * tempAction = new QAction(this);
  tempAction->setSeparator(true);
//  pageList->insertAction(0,tempAction);

  dirView = new NVBDirView(vSplitter);
  dirView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  dirView->setSelectionMode(QAbstractItemView::SingleSelection);
  dirView->setSelectionBehavior(QAbstractItemView::SelectItems);
  dirView->setContextMenuPolicy(Qt::ActionsContextMenu);
  dirView->setDragDropMode(QAbstractItemView::DragOnly);
  dirView->setGridSize(QSize(iconSize + 20, iconSize + 40));
  dirView->setIconSize(QSize(iconSize, iconSize));
//  dirView->hide();
	dirViewModel = new NVBDirViewModel(files,fileModel,this);
	dirView->setModel(dirViewModel);
// This will make the model release files. Might not be the best solution
//  connect(dirView,SIGNAL(dataWindow(int,int)),dirViewModel,SLOT(defineWindow(int,int)));
	iconSizeActionGroup = new QActionGroup(this);

	dirView->insertAction(0,iconSizeActionGroup->addAction("512x512"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("256x256"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("128x128"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("64x64"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("32x32"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("16x16"));
	dirView->insertAction(0,iconSizeActionGroup->addAction("Custom..."));

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

	pageView = new NVBSingleView(0,this);
//	pageView->setWindowModality(Qt::WindowModal);
	pageView->setFrameShape(QFrame::StyledPanel);
	pageView->setFrameShadow(QFrame::Plain);
	pageView->setWindowFlags(Qt::Popup);
	pageView->setLineWidth(2);
// 	pageView->setForegroundRole(QPalette::Light);
	connect(pageView,SIGNAL(dismissed()),this,SLOT(hidePageView()));
//	connect(pageView,SIGNAL(dismissed()),vSplitter,SLOT(show()));
	
//	hSplitter->addWidget(pageView);
	QPalette p = pageView->palette();
	p.setColor(QPalette::All,QPalette::Window,Qt::white);
	pageView->setPalette(p);
	
	pageView->hide();
	
	/*
	pageViewShowAnimation.setTargetObject(pageView);
	pageViewShowAnimation.setPropertyName("geometry");
	pageViewShowAnimation.setDuration(1000);
	*/
  piview = new NVBPageInfoView(vSplitter);
  connect(fileList,SIGNAL(activated(const QModelIndex&)), piview, SLOT(clearView()));
  connect(dirView,SIGNAL(clicked(const QModelIndex &)),piview,SLOT(showPage(const QModelIndex &)));

  dirView->insertAction(0,showPageInfoAction);
  connect(showPageInfoAction,SIGNAL(toggled(bool)),piview,SLOT(setVisible(bool)));

  if (!confile->contains("Browser/ShowPageInfo")) confile->setValue("Browser/ShowPageInfo",true);
  showPageInfoAction->setChecked(confile->value("Browser/ShowPageInfo").toBool());

  vSplitter->setStretchFactor(1,0);

//  fileListMenu = new QPopupMenu(this,"fileListMenu");
  
  resize(confile->value("Browser/Size", QSize(400, 300)).toSize());
  move(confile->value("Browser/Pos", QPoint(0, 0)).toPoint());

	int treeWidth = confile->value("Browser/TreeW", width()/2).toInt(); 
	hSplitter->setSizes(QList<int>() << treeWidth << width()-treeWidth);
}

QSize NVBBrowser::sizeHint () const {
	return confile->value("Browser/Size", QSize(400, 300)).toSize();
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

/*
void NVBBrowser::closeEvent(QCloseEvent *event)
{
  event->accept();
//  event->ignore();
  emit closeRequest();
}
*/

NVBBrowser::~ NVBBrowser( )
{
  updateColumnsVisibility();
  updateColumns();
	confile->setValue("Browser/Size", size());
  confile->setValue("Browser/Pos", pos());
	confile->setValue("Browser/TreeW", hSplitter->sizes().at(0));
	confile->setValue("Browser/FileListSize", fileList->size());
  confile->setValue("Browser/ShowPageInfo", piview->isVisible());
	if (dirView) delete dirView; // This ensures that the gradient menu is deleted after the models
  if (fileModel) delete fileModel;
}

void NVBBrowser::showItems() {
	dirViewModel->setDisplayItems(fileList->selectionModel()->selectedRows());
}

void NVBBrowser::loadPage( const QModelIndex & item )
{
	if (showPagesInBrowser)
		showPageView(item.data(PageRole).value<NVBDataSet*>());
	else
		emit pageRequest( dirViewModel->getAllFiles(item),item.row());
}

void NVBBrowser::showPageView(NVBDataSet* dset)
{
		pageView->setDataSet(dset);
		
		dirView->setEnabled(false);
		
		QRect dvr;
//		dvr.setTopLeft(dirView->mapTo(this,QPoint(0,0)));
		dvr.setTopLeft(dirView->mapToGlobal(QPoint(0,0)));
		dvr.setBottomRight(dirView->mapToGlobal(dirView->rect().bottomRight()));
		
//		pageViewShowAnimation.setStartValue(dvr.adjusted(100,100,-100,-100));
//		pageViewShowAnimation.setEndValue(dvr.adjusted(20,20,-20,-20));
		
//		pageViewShowAnimation.start();
		pageView->show();
		pageView->setGeometry(dvr.adjusted(20,20,-20,-20));
//		dirView->parentWidget()->hide();

}

void NVBBrowser::hidePageView()
{
		pageView->hide();
//	dirView->parentWidget()->show();
		dirView->setEnabled(true);
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

void NVBBrowser::columnAction() {
	QAction * action = qobject_cast<QAction *>(sender());
	if (!action) {
		NVBOutputError("Not called by QAction");
		return;
		}
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

void NVBBrowser::addRootFolder()
{
  addFolder(QModelIndex());
}

void NVBBrowser::addSubfolder()
{
	/*
  QModelIndexList ixs = fileList->selectionModel()->selectedRows();

  if (ixs.isEmpty())
    addFolder(QModelIndex());
  else {
    QModelIndex c = ixs.at(0);
    if (fileModel->isAFile(c))
      c = c.parent();
    addFolder(c);
    }
	*/
	if (fileModel->isAFile(folderMenuTarget))
		addFolder(folderMenuTarget.parent());
	else
		addFolder(folderMenuTarget);


}

void NVBBrowser::editFolder()
{
/*
	QModelIndexList ixs = fileList->selectionModel()->selectedRows();

	if (ixs.isEmpty()) {
		NVBOutputError("No selection");
		return;
		}

	QModelIndex c = ixs.at(0);
*/
	if (fileModel->isAFile(folderMenuTarget)) {
		NVBOutputError("File selected");
		return;
		}
	QString label = folderMenuTarget.data(Qt::DisplayRole).toString();
	QString path = folderMenuTarget.data(Qt::ToolTipRole).toString();
	bool r = fileModel->isRecursive(folderMenuTarget);

	if (NVBFolderInputDialog::editFolder(label,path,r)) {
		fileModel->editFolderItem(label,path,r,folderMenuTarget);
		updateFolders();
		}

}

void NVBBrowser::removeFolder()
{
	/*
  QModelIndexList ixs = fileList->selectionModel()->selectedRows();

  if (!ixs.isEmpty()) {
    QModelIndex c = ixs.at(0);
    if (!fileModel->isAFile(c)) {
      fileModel->removeItem(c);
      updateFolders();
      }
    }
	*/
	if (folderMenuTarget.isValid() && !fileModel->isAFile(folderMenuTarget)) {
		fileModel->removeItem(folderMenuTarget);
		updateFolders();
		}

}

void NVBBrowser::exportData() {
	NVBDirExportOptions o = NVBDirExportDialog::getOptions();
	if (o.valid)
		fileModel->exportData(fileList->selectionModel()->selectedRows().first(),o);
}

void NVBBrowser::enableFolderActions(const QModelIndex & index) {
	folderMenuTarget = index;
	bool isAFile = fileModel->isAFile(index);
	addFolderAction->setDisabled(isAFile);
	removeFolderAction->setDisabled(isAFile);
	exportFolderAction->setDisabled(isAFile);
}

void NVBBrowser::populateColumnsMenu() {
	QAction * a;
	columnsMenu->clear();
	remColumnsMenu->clear();
	for (int i = 1; i < fileModel->columnCount(); i++) {
		int j = fileList->header()->logicalIndex(i);
		if (j<0) {
			NVBOutputError(QString("Column %1 not found").arg(i));
			continue;
			}
		a = remColumnsMenu->addAction(fileModel->headerData(j,Qt::Horizontal,Qt::DisplayRole).toString(),this,SLOT(columnAction()));
		a->setData(-j);
		a = columnsMenu->addAction(fileModel->headerData(j,Qt::Horizontal,Qt::DisplayRole).toString(),this,SLOT(columnAction()));
		a->setCheckable(true);
		if (!fileList->isColumnHidden(j)) a->setChecked(true);
		a->setData(j);
		}
	columnsMenu->addSeparator();
	a = columnsMenu->addAction("Add column",this,SLOT(columnAction()));
	columnsMenu->addMenu(remColumnsMenu);
}

void NVBBrowser::updateColumns()
{
  confile->setValue("Browser/UserColumns",fileModel->columnCount()-1);
  for (int i = 1; i<fileModel->columnCount();i++) {
    confile->setValue(QString("Browser/UserColumn%1").arg(i),QString("%1/%2").arg(fileModel->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()).arg(fileModel->headerData(i,Qt::Horizontal,NVBColStrKeyRole).toString()));
    }

}

void NVBBrowser::showFoldersMenu() {
	folderMenuTarget = fileList->indexAt(fileList->viewport()->mapFromGlobal(QCursor::pos()));

	foldersMenu->clear();

	if (!folderMenuTarget.isValid()) { // Blank space
		foldersMenu->addAction(addRootFolderAction);
		}
	else if (fileModel->isAFile(folderMenuTarget)) { // File
		}
	else if (fileModel->flags(folderMenuTarget) & Qt::ItemIsEnabled) { // Valid folder
		foldersMenu->addAction(addFolderAction);
		foldersMenu->addAction(editFolderAction);
		foldersMenu->addAction(removeFolderAction);
		foldersMenu->addSeparator();
		foldersMenu->addAction(exportFolderAction);
		}
	else { // Status == error
		foldersMenu->addAction(editFolderAction);
		foldersMenu->addAction(removeFolderAction);
		}

	if (foldersMenu->actions().count())
		foldersMenu->addSeparator();
	foldersMenu->addAction(showFiltersAction);
	foldersMenu->addAction(clearFiltersAction);

	foldersMenu->exec(QCursor::pos());
}

void NVBBrowser::showColumnsMenu()
{
 columnsMenu->exec(QCursor::pos());
}


void NVBBrowser::setViewType(bool b)
{
	dirViewModel->setMode(b ? NVBDirViewModel::SpectroscopyOverlay : NVBDirViewModel::Normal );
}

void NVBBrowser::switchMode(bool b) {
	if (b) {
		dirViewModel->setSingleImageProvider(new NVBDataSet34Substractor());
		dirView->switchToGridMode();
//		dirViewModel->setMode(NVBDirViewModel::SingleImage);
		}
	else {
		dirViewModel->setMode(NVBDirViewModel::Normal);
		dirView->switchToNormalMode();
		}
}
