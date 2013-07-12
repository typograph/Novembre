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
#include "../core/NVBGeneralDelegate.h"
#include "../core/NVBFile.h"
// #include "NVBPageViewModel.h"
#include "NVBVizModel.h"
#include "NVBWidgetStackModel.h"
#include "NVBDelegateStackView.h"
#include "NVBToolsFactory.h"
#include "../core/NVBViewController.h"

#include "NVBFileWindowLayout.h"

class NVBFileWindow;
class NVBToolsFactory;
class NVBUserPageViewModel;
class NVBDataView;

class NVBDoubleListView : public QSplitter {
		Q_OBJECT
	private:
		QListView * topList;
//   QList<QListView *> bottomLists;

	private slots:
		void topContextMenuRequested(const QPoint & pos);

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

		virtual void closeEvent(QCloseEvent * event) { event->ignore(); hide(); }

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
		NVBUserPageViewModel * viewmodel;
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

		QAction * pageDockAction;
		QAction * toolsDockAction;

	protected :

	public :

		/// Create the window displaying the provided \a page
		NVBFileWindow(NVBWorkingArea * area, NVBDataSource* page, NVB::ViewType stateMode = NVB::DefaultView, NVBVizUnion viz = NVBVizUnion());
		/// Create the window displaying a page at \a index from a file \a model
		NVBFileWindow(NVBWorkingArea * area, const QModelIndex & index, NVBFile * model, NVB::ViewType stateMode = NVB::DefaultView);

		/// Destroy the window
		virtual ~NVBFileWindow();

		/// Make the window use the tool factory \a toolsFactory
		void setToolsFactory(NVBToolsFactory * toolsFactory) { tools = toolsFactory;}

		/// The type of the view (2D, 3D, Graph,...)
		virtual NVB::ViewType viewType() { return viewtype;}

		/// The left 'dock', with the list of the pages
		QWidget * pageView() { return pageListView; }
		/// The right 'dock', with the list of control widgets
		QWidget * toolsView() { return stackView; }

	public slots :

		/// Use an action from a plugin
		void installDelegate(QAction * action);

		/// Replace the selected page and/or visualizer with \a page and \a viz
		virtual void setSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());

		/// Add new page to the window, optionally using \a viz as a visualizer \callgraph
		virtual void addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion());

		/// Reimplemented from \class NVBViewController
		virtual NVBViewController * openInNewWindow(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion(), NVB::ViewType vtype = NVB::DefaultView);

		/// Open page at \a index in a new file window
		NVBViewController * openInNewWindow(const QModelIndex & index, NVB::ViewType vtype = NVB::DefaultView);

		/// Set visualizer for the selected page to \a visualizer
		virtual void setVisualizer(NVBVizUnion visualizer);
		/// Add a control widget \a controlWidget for the selected page
		virtual void addControlWidget(QWidget * controlWidget);
		/// Set a model 'active' visualizer that will allow direct user interaction with the page
		virtual void setActiveVisualizer(NVBVizUnion visualizer);

		virtual void selectionChanged(const QItemSelection & , const QItemSelection &);

#if QT_VERSION >= 0x040300
		void copyView();
#endif

	signals :
		/// Signals that the selected page was changed from \a deselected to \a selected
		void selectionChanged(const QModelIndex & selected , const QModelIndex & deselected);
		/// Weird signal
		void activateVisualizer(NVBVizUnion viz, const QModelIndex &);
		/// Signals that a page of a sertain type (topo,spec) was selected
		void pageSelected(NVB::PageType);

	protected:

		NVBDataView * view;

		virtual void focusInEvent(QFocusEvent * event);

		virtual void dragEnterEvent(QDragEnterEvent * event);
		virtual void dragLeaveEvent(QDragLeaveEvent * event);
		virtual void dragMoveEvent(QDragMoveEvent * event);
		virtual void dropEvent(QDropEvent * event);

		virtual void resizeEvent(QResizeEvent * event);

	protected slots:
		void addSource(const QModelIndex & index);

		void activateVisualizers(const QModelIndex & parent, int start, int end);

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

		void showPageOperationsMenu(const QModelIndex&, const QPoint & pos);

#ifndef NVB_NO_FW_DOCKS
		inline void setLeftVisible(bool visible = true) {
			if (pageDockAction->isChecked() != visible)
				pageDockAction->setChecked(visible);

			setWidgetVisible(pageListView, visible, true);
			}
		inline void setRightVisible(bool visible = true) {
			if (toolsDockAction->isChecked() != visible)
				toolsDockAction->setChecked(visible);

			setWidgetVisible(stackView, visible, false);
			}
/// Make child widget visible/unvisible with an option to compensate for widget position
		void setWidgetVisible(QWidget* widget, bool visible, bool shift);
#endif

#if QT_VERSION >= 0x040300
		void print();
#endif
	};

#endif
