//
// C++ Implementation: NVBFileWindow
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBFileWindow.h"
#ifdef WITH_2DVIEW
  #include "NVB2DPageView.h"
#endif
#ifdef WITH_3DVIEW
  #include "NVB3DPageView.h"
#endif
#ifdef WITH_GRAPHVIEW
  #include "NVBGraphView.h"
#endif
// #include "NVBFilterDelegate.h"
#include "../icons/icons_FW.xpm"
//#include "NVBListItemDelegate.h"

#include <QMenu>
#include <QPainter>
#include <QShortcut>
#include <QClipboard>

#ifndef newIcon
#define newIcon(var,name) \
  QIcon var; \
  var.addPixmap(name ## _16); \
  var.addPixmap(name ## _24); \
  var.addPixmap(name ## _32);
#endif

#if QT_VERSION >= 0x040300
#include <QPrinter>
#include <QPrintDialog>
#include <QFont>
#include <QFontMetrics>
#endif

// --------------

NVBDoubleListView::NVBDoubleListView(NVBFileWindow * parent, QAbstractListModel * topmodel, QAbstractListModel * bottommodel) : QSplitter(parent)
{
  setMouseTracking(true);

  setOrientation(Qt::Vertical);

  topList = new QListView(this);
  topList->setSelectionMode(QAbstractItemView::SingleSelection);
  topList->setSpacing(4);
  topList->setIconSize(QSize(32,32));
//   topList->setDragDropMode(QAbstractItemView::DragDrop);
//   topList->setDropIndicatorShown(true);
  topList->setContextMenuPolicy(Qt::CustomContextMenu);
  topList->setMovement(QListView::Snap);
//   frame->addWidget(topList);
//  topList->setMouseTracking(true);
//  topList->viewport()->setMouseTracking(true);
//  topList->setAttribute(Qt::WA_Hover);
//  connect(topList,SIGNAL(entered( const QModelIndex & )),topList,SLOT(edit( const QModelIndex & )));
//  topList->setItemDelegate(new NVBListItemDelegate(topList));


  connect(topList,SIGNAL(activated(const QModelIndex&)),
                  SIGNAL(topActivated(const QModelIndex&)));
  connect(topList,SIGNAL(clicked(const QModelIndex&)),
                  SIGNAL(topClicked(const QModelIndex&)));
  connect(topList,SIGNAL(doubleClicked(const QModelIndex&)),
                  SIGNAL(topDoubleClicked(const QModelIndex&)));

  connect(topList,SIGNAL(customContextMenuRequested(const QPoint &)),
                  SLOT(topContextMenuRequested(const QPoint &)));


  setTopModel(topmodel);
  
  if (bottommodel)
    addBottomModel(bottommodel);

  setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
  resize(minimumSizeHint());
}

NVBDoubleListView::~ NVBDoubleListView()
{
/*  if (topList) {
    if (topList->model()) {
      QAbstractItemModel * m = topList->model();
      topList->setModel(NULL);
      delete m;
      }
    delete topList;
    }
  if (bottomList) {
    if (bottomList->model()) {
      QAbstractItemModel * m = bottomList->model();
      bottomList->setModel(NULL);
      delete m;
      }
    delete bottomList;
    }*/
}

void NVBDoubleListView::setTopModel(QAbstractListModel * model)
{
  if (model) {
    topList->setModel(model);
    topList->show();
    }
  else
    topList->hide();
}

// void NVBDoubleListView::setBottomModel(QAbstractListModel * model)
// {
//   if (model) {
//     bottomList->setModel(model);
//     bottomList->show();
//     }
//   else
//     bottomList->hide();
// }

void NVBDoubleListView::addBottomModel(QAbstractListModel * model)
{
  if (model) {
    QListView * bottomList = new QListView(this);
    bottomList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bottomList->setSpacing(4);
    bottomList->setIconSize(QSize(32,32));
    bottomList->setDragDropMode(QAbstractItemView::DragOnly);
//     frame->addWidget(bottomList);
/*    bottomList->setMouseTracking(true);
    bottomList->viewport()->setMouseTracking(true);
    bottomList->setAttribute(Qt::WA_Hover);
    connect(bottomList,SIGNAL(entered( const QModelIndex & )),bottomList,SLOT(edit( const QModelIndex & )));
    bottomList->setItemDelegate(new NVBListItemDelegate(bottomList));*/
    
    connect(bottomList, SIGNAL(activated(const QModelIndex&)),
                        SIGNAL(bottomActivated(const QModelIndex&)));
    connect(bottomList, SIGNAL(clicked(const QModelIndex&)),
                        SIGNAL(bottomClicked(const QModelIndex&)));
    connect(bottomList, SIGNAL(doubleClicked(const QModelIndex&)),
                        SIGNAL(bottomDoubleClicked(const QModelIndex&)));
      
    bottomList->setModel(model);
    bottomList->show();
    }
}


QModelIndex NVBDoubleListView::selectedTopPage( )
{
  if (topList && topList->selectionModel()->hasSelection())
    return topList->selectionModel()->selectedRows().at(0);
  else
    return QModelIndex();
}

// QModelIndex NVBDoubleListView::selectedBottomPage( )
// {
//   if (bottomList && bottomList->selectionModel()->hasSelection())
//     return bottomList->selectionModel()->selectedRows().at(0);
//   else
//     return QModelIndex();
// }

void NVBDoubleListView::setCurrentTopIndex(const QModelIndex & index)
{
  if (topList)
    topList->setCurrentIndex(index);
}

// void NVBDoubleListView::setCurrentBottomIndex(const QModelIndex & index)
// {
//   if (bottomList)
//     bottomList->setCurrentIndex(index);
// }

// =====================

NVBFileWindow::NVBFileWindow( NVBWorkingArea * area, const QModelIndex & index, NVBFile * model, NVB::ViewType stateMode)
#if QT_VERSION >= 0x040300
  : QMdiSubWindow(area->newWindowParentWidget())
#else
  : QFrame(area->newWindowParentWidget())
#endif
  , NVBViewController(area),file(model),view(0)
{
  tools = qApp->property("toolsFactory").value<NVBToolsFactory*>();

  NVBDataSource * page = NVBToolsFactory::hardlinkDataSource(index.data(PageRole).value<NVBDataSource*>());
  
  if (file) {
    file->use();
		setWindowTitle(file->name());
    }
  else if (page)
		setWindowTitle(page->owner->sources().name());

  if (stateMode == NVB::DefaultView) {
    stateMode = (NVB::ViewType)(index.data(PageTypeRole).value<NVB::PageType>());
    }
  createView(stateMode, model);
  addSource(page);
  show();
  area->addWindow(this);
  emit pageSelected(page->type());

#if QT_VERSION >= 0x040300
	new QShortcut(QKeySequence(QKeySequence::Copy),this,SLOT(copyView()));
#endif
}

NVBFileWindow::NVBFileWindow( NVBWorkingArea * area, NVBDataSource * page, NVB::ViewType stateMode, NVBVizUnion viz)
#if QT_VERSION >= 0x040300
  : QMdiSubWindow(area->newWindowParentWidget())
#else
  : QFrame(parent)
#endif
  , NVBViewController(area),file(0),view(0)
{
  tools = qApp->property("toolsFactory").value<NVBToolsFactory*>();

	if (page->owner)
		setWindowTitle(page->owner->sources().name());
	else
		NVBOutputError("NVBDataSource has no owner");
  if (stateMode == NVB::DefaultView)
    stateMode = (NVB::ViewType)(page->type());
  createView(stateMode);
  addSource(page,viz);
  show();
  area->addWindow(this);
  emit pageSelected(page->type());

#if QT_VERSION >= 0x040300
	new QShortcut(QKeySequence(QKeySequence::Copy),this,SLOT(copyView()));
#endif
}

NVBFileWindow::~ NVBFileWindow()
{
  pageListView->topSelection()->disconnect(this);

  if (viewmodel) delete viewmodel;
  if (widgetmodel) delete widgetmodel;
  if (pageListView) delete pageListView;
  if (stackView) delete stackView;
  if (file) file->release();
//  if (stackview) delete stackView;
}

void NVBFileWindow::createView( NVB::ViewType vtype,  QAbstractListModel * model )
{
  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);

  viewmodel = new NVBPageViewModel();

  connect(viewmodel,SIGNAL(rowsInserted(const QModelIndex &,int,int)),SLOT(activateVisualizers(const QModelIndex &,int,int)));

  widgetmodel = new NVBWidgetStackModel(viewmodel);

  pageListView = new NVBDoubleListView(this,viewmodel,model);
  pageListView->setWindowTitle("Pages");
  pageListView->hide();

  connect(pageListView,SIGNAL(topDoubleClicked(const QModelIndex& )),SLOT(openInNewWindow(const QModelIndex&)));
  connect(pageListView,SIGNAL(bottomDoubleClicked(const QModelIndex& )),SLOT(addSource(const QModelIndex&)));
  connect(pageListView->topSelection(),SIGNAL(selectionChanged( const QItemSelection & , const QItemSelection & )),this,SLOT(selectionChanged( const QItemSelection & , const QItemSelection & )));
  connect(pageListView,SIGNAL(topContextMenuRequested(const QModelIndex&, const QPoint & )),
                   SLOT(showPageOperationsMenu(const QModelIndex&, const QPoint &)));

  stackView = new NVBDelegateStackView(widgetmodel);
  connect(this,SIGNAL(selectionChanged( const QModelIndex & , const QModelIndex & )),stackView,SLOT(setSelectedIndex(const QModelIndex&)));
  stackView->hide();

  toolBarLayout = new QVBoxLayout();

#if QT_VERSION >= 0x040300
  delete layout(); // QMdiSubWindow has a useless layout
#endif

  myLayout = new NVBFileWindowLayout(this);
  myLayout->setToolBar(toolBarLayout);

#ifndef NVB_NO_FWDOCKS
// Dock show

  QToolBar * tBar = new QToolBar("Lists",this);

  if (tBar) {
    tBar->setOrientation(Qt::Vertical);
    tBar->show();
    toolBarLayout->addWidget(tBar);

    pageDockAction = tBar->addAction(QIcon(_FW_PageList),"Show/hide page list");
    pageDockAction->setCheckable(true);
    connect(pageDockAction,SIGNAL(toggled(bool)),this,SLOT(setLeftVisible(bool)));
//     connect(pageDock,SIGNAL(visibilityChanged(bool)),pageDockAction,SLOT(setChecked(bool)));
  
    toolsDockAction = tBar->addAction(QIcon(_FW_WidgetList),"Show/hide controls list");
    toolsDockAction->setCheckable(true);
    connect(toolsDockAction,SIGNAL(toggled(bool)),this,SLOT(setRightVisible(bool)));
//     connect(stackView,SIGNAL(visibilityChanged(bool)),toolsDockAction,SLOT(setChecked(bool)));
  }

  myLayout->setLeftDock(pageListView);
#endif

  // Detemine view type

  if (vtype != NVB::NoView) {

    switch (vtype) {
      case NVB::DefaultTopoView :
      case NVB::DefaultSpecView : {
        vtype = tools->getDefaultPageViewType(( NVB::PageType)vtype);
        break;
        }
      default : break;
      }      

    vizmodel = new NVBVizModel(viewmodel,vtype);

    switch (vtype) {
#ifdef WITH_2DVIEW
      case NVB::TwoDView: {
        set2DView();
        break;
        }
#endif
#ifdef WITH_3DVIEW
      case NVB::ThreeDView: {
        set3DView();
        break;
        }
#endif
#ifdef WITH_GRAPHVIEW
      case NVB::GraphView: {
        setGraphView();
        break;
        }
#endif
      case NVB::ListView:
      default : {
        setListView();
        break;
        }
      }

    viewtype = vtype;

    if (view) {
      QToolBar * t = view->generateToolbar(this);
      if (t) {
				t->setOrientation(Qt::Vertical);
        toolBarLayout->addWidget(t);
        t->show();
        }

      myLayout->setView(view);
      view->w()->show();
      view->w()->setFocus(Qt::OtherFocusReason);
      }

#ifndef NVB_NO_FWDOCKS
		connect(stackView,SIGNAL(fillStatus(bool)),SLOT(setRightVisible(bool)));
    myLayout->setRightDock(stackView);
#endif

  // Include tools buttons

#if QT_VERSION >= 0x040300
    // create window toolbar
    wndtools = new QToolBar("View operations",this);
    if (wndtools) {
      toolBarLayout->addWidget(wndtools);
      newIcon(printicon,_FW_print);
      wndtools->addAction(printicon,"Print",this,SLOT(print()));
      wndtools->show();
      }
#endif

  toolBarLayout->addStretch(1);

  }

//   resize(sizeHint());

}

void NVBFileWindow::setListView()
{
  pageListView->show();
}

#ifdef WITH_2DVIEW
void NVBFileWindow::set2DView()
{
  NVB2DPageView * tview = new NVB2DPageView(vizmodel,this);
  if (!tview) {
		NVBOutputError("Creating 2DView failed.");
    return;
    }

  connect(this,SIGNAL(selectionChanged( const QModelIndex & , const QModelIndex & )), tview,SLOT(select( const QModelIndex &)));
  connect(this,SIGNAL(activateVisualizer( NVBVizUnion, const QModelIndex & )), tview,SLOT(setActiveVisualizer( NVBVizUnion, const QModelIndex & )));

  view = tview;
}
#endif

#ifdef WITH_3DVIEW
void NVBFileWindow::set3DView()
{
  NVB3DPageView * tview = new NVB3DPageView(vizmodel,this);
  if (!tview) {
		NVBOutputError("Creating 3DView failed.");
    return;
    }
  connect(this,SIGNAL(selectionChanged( const QModelIndex & , const QModelIndex & )), tview,SLOT(select( const QModelIndex &)));
  connect(this,SIGNAL(activateVisualizer( NVBVizUnion, const QModelIndex & )), tview,SLOT(setActiveVisualizer( NVBVizUnion, const QModelIndex & )));

  view = tview;
}
#endif

#ifdef WITH_GRAPHVIEW
void NVBFileWindow::setGraphView()
{
  NVBGraphView * gview = new NVBGraphView(vizmodel,this);
  if (!gview) {
		NVBOutputError("Creating GraphView failed.");
    return;
    }

  view = gview;
}
#endif

void NVBFileWindow::installDelegate( QAction * action )
{
  tools->activateDelegate(action->data().toInt(),pageListView->selectedTopPage().data(PageRole).value<NVBDataSource*>(),this);
}

/*
void NVBFileWindow::setVisualizer(NVBVizUnion visualizer, QWidget * controlWidget)
{
  vizmodel->setVisualizer(visualizer,pageListView->selectedTopPage());
  widgetmodel->setVizWidget(controlWidget,pageListView->selectedTopPage());
}

void NVBFileWindow::addController(NVBVizUnion visualizer, QWidget * controlWidget)
{
  vizmodel->setVisualizer(visualizer);
  widgetmodel->addControlWidget(controlWidget);
}
*/
void NVBFileWindow::setSource(NVBDataSource * page, NVBVizUnion viz)
{
  if (viewmodel->data(pageListView->selectedTopPage(),PageRole).value<NVBDataSource*>() != page)
    viewmodel->setData(pageListView->selectedTopPage(),QVariant::fromValue(page),PageRole);
  if (viz.valid)
    vizmodel->setVisualizer(viz,pageListView->selectedTopPage());
  else if (vizmodel->data(pageListView->selectedTopPage(),PageVizItemRole).value<NVBVizUnion>().valid == 0)
    tools->activateDefaultVisualizer(page,this);
}

void NVBFileWindow::addSource(NVBDataSource * page, NVBVizUnion viz)
{
  if (!page) return;

  disconnect(viewmodel,SIGNAL(rowsInserted(const QModelIndex &,int,int)),this,SLOT(activateVisualizers(const QModelIndex &,int,int)));
  viewmodel->addSource(page);
  pageListView->topSelection()->select(viewmodel->index(0),QItemSelectionModel::ClearAndSelect);
  connect(viewmodel,SIGNAL(rowsInserted(const QModelIndex &,int,int)),SLOT(activateVisualizers(const QModelIndex &,int,int)));

  if (viz.valid)
    vizmodel->setVisualizer(viz,pageListView->selectedTopPage());
  else
    tools->activateDefaultVisualizer(page,this);

/*
  if (activate) {
    stackView->setSelectedIndex(viewmodel->index(0));
    }
*/
}

void NVBFileWindow::addSource(const QModelIndex & index)
{
  addSource(NVBToolsFactory::hardlinkDataSource(index.data(PageRole).value<NVBDataSource*>()));
}

NVBViewController * NVBFileWindow::openInNewWindow(NVBDataSource * page, NVBVizUnion viz, NVB::ViewType vtype)
{
  NVBFileWindow * c = new NVBFileWindow(parent_area,page,vtype,viz);
  area()->addWindow(c);
  return c;
}

NVBViewController * NVBFileWindow::openInNewWindow(const QModelIndex & index, NVB::ViewType vtype)
{
   // The const_cast construction is safe, because we are not going to use any non-const model function in the lifetime of the index
   NVBFileWindow * c = new NVBFileWindow(parent_area,index,dynamic_cast<NVBFile*>(const_cast<QAbstractItemModel*>(index.model())),vtype);
  area()->addWindow(c);
  return c;
}

void NVBFileWindow::addControlWidget(QWidget * controlWidget)
{
  if (controlWidget)
    widgetmodel->addWidget(controlWidget,pageListView->selectedTopPage());
}

void NVBFileWindow::setVisualizer(NVBVizUnion visualizer)
{
  vizmodel->setVisualizer(visualizer, pageListView->selectedTopPage());
}

void NVBFileWindow::setActiveVisualizer(NVBVizUnion visualizer)
{ // this one emits signals because of the unknown nature of the view
  emit activateVisualizer(visualizer,pageListView->selectedTopPage());
}

void NVBFileWindow::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  if (selected.indexes().size() != 1) {
		NVBOutputError( "Non-single selection detected for dock");
    emit pageSelected(NVB::InvalidPage);
    }
  else {
    emit pageSelected(selected.indexes().at(0).data(PageTypeRole).value<NVB::PageType>());
    if (deselected.indexes().size() != 1)
      emit selectionChanged(selected.indexes().at(0),QModelIndex());
    else
      emit selectionChanged(selected.indexes().at(0),deselected.indexes().at(0));
    }
}

void NVBFileWindow::activateVisualizers(const QModelIndex & parent, int start, int end)
{
  if (parent.isValid()) return;
  while (start <= end) { // FIXME
    pageListView->topSelection()->select(viewmodel->index(start),QItemSelectionModel::ClearAndSelect);
    tools->activateDefaultVisualizer(viewmodel->index(start).data(PageRole).value<NVBDataSource*>(),this);
    start += 1;
    }
}

void NVBFileWindow::dragEnterEvent(QDragEnterEvent * event)
{
  if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType()))
    event->acceptProposedAction();
  else
#if QT_VERSION >= 0x040300
    QMdiSubWindow::dragEnterEvent(event);
#else
    QFrame::dragEnterEvent(event);
#endif
}

void NVBFileWindow::dragLeaveEvent(QDragLeaveEvent * event)
{
#if QT_VERSION >= 0x040300
    QMdiSubWindow::dragLeaveEvent(event);
#else
    QFrame::dragLeaveEvent(event);
#endif
}

void NVBFileWindow::dragMoveEvent(QDragMoveEvent * event)
{
  if ( /*event->mimeData()->hasFormat("text/plain")
    &&*/ event->answerRect().intersects(pageListView->geometry()))
#if QT_VERSION >= 0x040300
    QMdiSubWindow::dragMoveEvent(event);
#else
    QFrame::dragMoveEvent(event);
#endif
  else {
    event->acceptProposedAction();
    }
}

void NVBFileWindow::dropEvent(QDropEvent * event)
{
  if ( pageListView->geometry().contains(event->pos()))
#if QT_VERSION >= 0x040300
    QMdiSubWindow::dropEvent(event);
#else
    QFrame::dropEvent(event);
#endif
  else {
    if ( pageListView->dragOriginatesFromTop(event) )
      return;
    if (event->mimeData()->hasFormat(NVBDataSourceMimeData::dataSourceMimeType())) {
      event->acceptProposedAction();
      addSource(((NVBDataSourceMimeData*)event->mimeData())->getPageData());
      }
    }
}

bool NVBDoubleListView::dragOriginatesFromTop(QDropEvent * event)
{
  return (event->source() == topList || event->source() == topList->viewport());
}

void NVBDoubleListView::topContextMenuRequested(const QPoint & pos)
{
  emit topContextMenuRequested(topList->indexAt(pos),topList->viewport()->mapToGlobal(pos));
}

void NVBFileWindow::showPageOperationsMenu(const QModelIndex & index, const QPoint & pos)
{
  if (!index.isValid()) return;

  QMenu * m = new QMenu(this);
  m->addAction("Open in new window");
  switch (index.data(PageTypeRole).value<NVB::PageType>()) {
    case NVB::TopoPage :
#ifdef WITH_GRAPHVIEW
      if (tools->getDefaultPageViewType(NVB::TopoPage) != NVB::GraphView)
        m->addAction("Open as curves");
#endif
#ifdef WITH_3DVIEW
      if (tools->getDefaultPageViewType(NVB::TopoPage) != NVB::ThreeDView)
        m->addAction("Open in 3D");
#endif
      break;
    case NVB::SpecPage :
#ifdef WITH_2DVIEW
      if (tools->getDefaultPageViewType(NVB::TopoPage) != NVB::TwoDView)
        m->addAction("Open on plane");
#endif
      break;
    default: break;
    }
  m->addAction("Move up")->setEnabled(index.row() != 0);
  m->addAction("Move down")->setEnabled(index.row() != index.model()->rowCount()-1);
  m->addAction("Remove")->setEnabled(index.model()->rowCount() != 1);
  QAction * a = m->exec(pos);

  if (!a) return;

  if (a->text() == "Open in new window") {
    openInNewWindow(index);
    }
  else if (a->text() == "Move up") {
    }
  else if (a->text() == "Move down") {
    }
  else if (a->text() == "Remove") {
    viewmodel->removeRow(index.row());
    }
#ifdef WITH_2DVIEW
  else if (a->text() == "Open on plane") {
    openInNewWindow(index);
//     openInNewWindow(NVBToolsFactory::hardlinkDataSource(index.data(PageRole).value<NVBDataSource*>()),NVBVizUnion(),NVB::TwoDView);
    }
#endif
#ifdef WITH_GRAPHVIEW
  else if (a->text() == "Open as curves") {
    openInNewWindow(index,NVB::GraphView);
//     openInNewWindow(NVBToolsFactory::hardlinkDataSource(index.data(PageRole).value<NVBDataSource*>()),NVBVizUnion(),NVB::GraphView);
    }
#endif
#ifdef WITH_3DVIEW
  else if (a->text() == "Open in 3D") {
    openInNewWindow(index,NVB::ThreeDView);
//     openInNewWindow(NVBToolsFactory::hardlinkDataSource(index.data(PageRole).value<NVBDataSource*>()),NVBVizUnion(),NVB::ThreeDView);
    }
#endif
}

// QWidget * NVBFileWindow::view()
// {
//   return layout()->itemAt(1)->widget();
// }

// #include <QDebug>
// 
// bool NVBFileWindow::event(QEvent * event)
// {
//   switch(event->type()) {
//     case 8: qDebug() << "FocusIn"; break;
//     case 9: qDebug() << "FocusOut"; break;
// /*    case 10: qDebug() << "Enter"; break;
//     case 11: qDebug() << "Leave"; break;
//     case 13: qDebug() << "Move"; break;
//     case 14: qDebug() << "Resize"; break;
//     case 17: qDebug() << "Show"; break;
//     case 18: qDebug() << "Hide"; break;
//     case 19: qDebug() << "Close"; break;
//     case 21: qDebug() << "ParentChange"; break;
//     case 24: qDebug() << "WindowActivate"; break;
//     case 25: qDebug() << "WindowDeactivate"; break;
//     case 26: qDebug() << "ShowToParent"; break;
//     case 27: qDebug() << "HideToParent"; break;
//     case 50: qDebug() << "SockAct"; break;
//     case 52: qDebug() << "DeferredDelete"; break;
//     case 76: qDebug() << "LayoutRequest"; break;
//     case 77: qDebug() << "UpdateRequest"; break;
//     case 78: qDebug() << "UpdateLater"; break;
//     case 94: qDebug() << "OkRequest"; break;
//     case 96: qDebug() << "IconDrag"; break;
//     case 97: qDebug() << "FontChange"; break;
//     case 98: qDebug() << "EnabledChange"; break;
//     case 99: qDebug() << "ActivationChange"; break;
//     case 100: qDebug() << "StyleChange"; break;
//     case 101: qDebug() << "IconTextChange"; break;
//     case 102: qDebug() << "ModifiedChange"; break;
//     case 103: qDebug() << "WindowBlocked"; break;
//     case 104: qDebug() << "WindowUnblocked"; break;
//     case 105: qDebug() << "WindowStateChange"; break;
//     case 109: qDebug() << "MouseTrackingChange"; break;
//     case 121: qDebug() << "ApplicationActivate"; break;
//     case 122: qDebug() << "ApplicationDeactivate"; break;
//     case 126: qDebug() << "ZOrderChange"; break;
//     case 130: qDebug() << "AccessibilityDescription"; break;
//     case 131: qDebug() << "ParentAboutToChange"; break;
//     case 132: qDebug() << "WinEventAct"; break;
//     case 150: qDebug() << "EnterEditFocus"; break;
//     case 151: qDebug() << "LeaveEditFocus"; break;
//     case 153: qDebug() << "MenubarUpdated"; break;
//     case 169: qDebug() << "KeyboardLayoutChange"; break;
//     case 170: qDebug() << "DynamicPropertyChange"; break;
//     case 173: qDebug() << "NonClientAreaMouseMove"; break;
//     case 174: qDebug() << "NonClientAreaMouseButtonPress"; break;
//     case 175: qDebug() << "NonClientAreaMouseButtonRelease"; break;
//     case 176: qDebug() << "NonClientAreaMouseButtonDblClick"; break;
//     case 177: qDebug() << "MacSizeChange"; break;
//     case 178: qDebug() << "ContentsRectChange"; break;
//     case 186: qDebug() << "GrabMouse"; break;
//     case 187: qDebug() << "UngrabMouse"; break;
//     case 188: qDebug() << "GrabKeyboard"; break;
//     case 189: qDebug() << "UngrabKeyboard"; break;*/
//     default: break;
// //     default: qDebug() << event->type();
//     }
// 
//   return QMdiSubWindow::event(event);
// }

void NVBFileWindow::focusInEvent(QFocusEvent * event)
{
#if QT_VERSION >= 0x040300
  QMdiSubWindow::focusInEvent(event);
#else
  QFrame::focusInEvent(event);
#endif
  if (event->isAccepted())
    emit pageSelected(pageListView->selectedTopPage().data(PageTypeRole).value<NVB::PageType>());
}

void NVBFileWindow::setWidgetVisible(QWidget * widget, bool visible, bool shift)
{
  if (widget->isVisible() == visible) return;

  if (visible) {
    widget->setVisible(visible);
    update();
    resize(width()+widget->sizeHint().width()+myLayout->spacing(),height());
    if (shift)
      move(x()-widget->width()+myLayout->spacing(),y());
    }
  else {
    int owwidth = widget->width() + myLayout->spacing();
    widget->setVisible(visible);
    update();
    resize(width()-owwidth,height());
    if (shift)
      move(x()+owwidth,y());
  }
}

void NVBFileWindow::resizeEvent(QResizeEvent * event)
{
  QSize s = myLayout->closestAcceptableSize( event->size(), 0, -1+style()->pixelMetric(QStyle::PM_TitleBarHeight,0,this) );
  if (s != event->size())
    resize(s);
#if QT_VERSION >= 0x040300
  QMdiSubWindow::resizeEvent(event);
#else
  QFrame::resizeEvent(event);
#endif

}

#if QT_VERSION >= 0x040300
void NVBFileWindow::print( )
{
  QPrinter printer;
  QPrintDialog pdialog(&printer,this);
  pdialog.setEnabledOptions(QAbstractPrintDialog::PrintToFile);
  if (pdialog.exec() != QDialog::Accepted) return;
  QSize page = printer.pageRect().size();
  QSize pict = view->w()->size();

  QPainter p(&printer);

  QPoint rndoffset;

  if (pict.width() > page.width() || pict.height() > page.height()) {
		NVBOutputPMsg("The view is larger than the paper");
    
    QPixmap pxmap(pict);
    
    view->w()->render(&pxmap);   
    
    pict.scale(page,Qt::KeepAspectRatio);
    
    rndoffset.setX((page.width()-pict.width())/2);
    rndoffset.setY((page.height()-pict.height())/2);
   
    p.drawPixmap(QRect(rndoffset,pict),pxmap);
   
    }
  else {
    rndoffset.setX((page.width()-pict.width())/2);
    rndoffset.setY((page.height()-pict.height())/2);
    
    view->w()->render(&printer,rndoffset);
    }  
  QPoint txtoffset;

  QFont fnt;
  fnt.setPointSize(14); //TODO make fontsize paper-dependent
  QSize text = QFontMetrics::QFontMetrics(fnt).size(0,windowTitle());
  
  txtoffset.setX((page.width()-text.width())/2);
  txtoffset.setY(text.height());
  
  p.drawText(txtoffset,windowTitle());
  
  p.end();
}

void NVBFileWindow::copyView()
{
	QClipboard * cb = QApplication::clipboard();
	cb->setText(windowTitle());
	QImage i(view->w()->size(),QImage::Format_RGB32);
	view->w()->render(&i);
	cb->setImage(i);
}

#endif

