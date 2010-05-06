//
// C++ Interface: NVBFileWindow
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBFILEWINDOW_H
#define NVBFILEWINDOW_H


#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QListView>
#include <QAbstractListModel>
#include <QIcon>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QCloseEvent>
#include <QItemSelectionModel>
#if QT_VERSION >= 0x040300
  #include <QMdiSubWindow>
#else
  #include <QFrame>
#endif
#include "NVBGeneralDelegate.h"
#include "NVBFile.h"
#include "NVBPageViewModel.h"
#include "NVBVizModel.h"
#include "NVBWidgetStackModel.h"
#include "NVBDelegateStackView.h"
#include "NVBToolsFactory.h"
#include "NVBViewController.h"

#include "NVBFileWindowLayout.h"

class NVBFileWindow;
class NVBToolsFactory;
class NVBPageViewModel;
class NVBDataView;

class NVBDoubleListView : public QSplitter {
Q_OBJECT
private:
  QListView * topList;
//   QList<QListView *> bottomLists;

private slots:
  void topContextMenuRequested( const QPoint & pos );

public :
  NVBDoubleListView(NVBFileWindow * parent, QAbstractListModel * topmodel, QAbstractListModel * bottommodel = 0);
  ~NVBDoubleListView();

  QModelIndex selectedTopPage();
//   QModelIndex selectedBottomPage();

  inline QItemSelectionModel * topSelection() {
    if (topList)
      return topList->selectionModel();
    else
      return NULL;
    }

  bool dragOriginatesFromTop(QDropEvent * event);

public slots :

  virtual void closeEvent ( QCloseEvent * event ) { event->ignore(); hide(); }

  void setTopModel(QAbstractListModel * model);
  void addBottomModel(QAbstractListModel * model);
//   void setBottomModel(QAbstractListModel * model);

  void setCurrentTopIndex(const QModelIndex & index);
//   void setCurrentBottomIndex(const QModelIndex & index);

signals:

  void topActivated(const QModelIndex&);
  void topClicked(const QModelIndex&);
  void topDoubleClicked(const QModelIndex&);

  void topContextMenuRequested(const QModelIndex&, const QPoint & pos);

  void bottomActivated(const QModelIndex&);
  void bottomClicked(const QModelIndex&);
  void bottomDoubleClicked(const QModelIndex&);

};

/**
 * \brief Novembre file window
 *
 * The file window is responsible for showing file contents
 * The class selects automatically the best representation on the data
 * ( page list, 2D, 3D, graph )
 * The model can be shared by different file windows. Use _openInNewWindow_ for that.
 */
#if QT_VERSION >= 0x040300
class NVBFileWindow : public QMdiSubWindow, public NVBViewController
#else
class NVBFileWindow : public QFrame, public NVBViewController
#endif
{
  Q_OBJECT

//  NVBFile * model;
  NVBPageViewModel * viewmodel;
  NVBVizModel * vizmodel;
  NVBWidgetStackModel * widgetmodel;

  NVBToolsFactory * tools;
  NVBDoubleListView * pageListView;
  NVBDelegateStackView * stackView;

  QToolBar * wndtools;

  NVBFileWindowLayout * myLayout;
//   QHBoxLayout * viewLayout;
  QVBoxLayout * toolBarLayout;

  NVB::ViewType viewtype;

  NVBFile * file;

  NVBVizUnion nextViz;

protected :

public :

  NVBFileWindow( NVBWorkingArea * area, NVBDataSource* page, NVB::ViewType stateMode = NVB::DefaultView, NVBVizUnion viz = NVBVizUnion());
  NVBFileWindow( NVBWorkingArea * area, const QModelIndex & index, NVBFile * model, NVB::ViewType stateMode = NVB::DefaultView );

  virtual ~NVBFileWindow();

  void setToolsFactory(NVBToolsFactory * toolsFactory) { tools = toolsFactory;}

  virtual NVB::ViewType viewType() { return viewtype;}

  QWidget * pageView() { return pageListView; }
  QWidget * toolsView() { return stackView; }

public slots :

  void installDelegate(QAction * action);

  virtual void setSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());
  virtual void addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());

  virtual NVBViewController * openInNewWindow(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion(), NVB::ViewType vtype = NVB::DefaultView);
  NVBViewController * openInNewWindow(const QModelIndex & index, NVB::ViewType vtype = NVB::DefaultView);

  virtual void setVisualizer(NVBVizUnion visualizer);
  virtual void addControlWidget(QWidget * controlWidget);
  virtual void setActiveVisualizer(NVBVizUnion visualizer);

  virtual void selectionChanged( const QItemSelection & , const QItemSelection & );

signals :
  void selectionChanged( const QModelIndex & selected , const QModelIndex & deselected);
  void activateVisualizer(NVBVizUnion viz, const QModelIndex &);
  void pageSelected(NVB::PageType);
/*  void newWindowRequest(const QModelIndex & index);
  void newWindowRequest(int pagenum);
*/

protected:

  NVBDataView * view;

  virtual void focusInEvent ( QFocusEvent * event );

  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void dragLeaveEvent ( QDragLeaveEvent * event );
  virtual void dragMoveEvent ( QDragMoveEvent * event );
  virtual void dropEvent ( QDropEvent * event );

  virtual void resizeEvent ( QResizeEvent * event );

protected slots:
  void addSource(const QModelIndex & index);

  void activateVisualizers( const QModelIndex & parent, int start, int end );

  void createView(NVB::ViewType vtype = NVB::ListView, QAbstractListModel * model = 0);

  void setListView();
#ifdef WITH_2DVIEW
  void set2DView();
#endif
#ifdef WITH_3DVIEW
  void set3DView();
#endif
#ifdef WITH_GRAPHVIEW
  void setGraphView();
#endif

  void showPageOperationsMenu( const QModelIndex&, const QPoint & pos );

#ifndef NVB_NO_FW_DOCKS
  inline void setLeftVisible(bool visible) { setWidgetVisible(pageListView,visible,true); }
  inline void setRightVisible(bool visible)  { setWidgetVisible(stackView,visible,false); }
 /// Make child widget visible/unvisible with an option to compensate for widget position
  void setWidgetVisible(QWidget* widget, bool visible, bool shift);
#endif

#if QT_VERSION >= 0x040300
  void print();
#endif
};

#endif
