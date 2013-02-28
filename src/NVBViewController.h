//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBVIEWCONTROLLER_H
#define NVBVIEWCONTROLLER_H

#include <QModelIndex>
#include <QWidget>
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

/**
 * \brief Interface for the main workspace
 *
 * NVBWorkingArea defines a series of functions that all
 * the windows in Novembre can use. This class has only one child -
 * NVBMainWindow.
 */
class NVBWorkingArea {
	public:
		NVBWorkingArea() {;}
		virtual ~NVBWorkingArea() {;}
		/// Add \a window to the workspace, making it persistent
		/**
		 * A persistent window will hide when the user tries
		 * to close it.
		 */
		virtual void addPersistentWindow(QWidget * window) = 0;
		/// Add \a window to the workspace
		/**
		 * This window will close and be destroyed when user closes it
		 */
		virtual void addWindow(QWidget * window) = 0;
		/// Get the NVBWorking area as a QWidget
		/**
		 * When creating new windows to pass them to
		 * \p NVBWorkingArea::addWindow, use the widget returned
		 * by this function as a parent object.
		 */
		virtual QWidget * newWindowParentWidget() const = 0;
	};

/**
 * \brief A window that plugins can interact with.
 *
 * NVBViewController must be a parent for all windows
 * that want to be able to accept commands from plugins.
 * NVBFileWindow is an example of an subclass of this class.
 *
 * A subclass has the freedom to interpret the terms
 * like 'active page' or 'control widget' as it sees fit.
 */
class NVBViewController {
	private:
	public:
		/// Create new NVBViewController in \a area
		/**
		 * This window will use area::newWindowParentWidget
		 * as parent for QWidget.
		 */
		NVBViewController(NVBWorkingArea * area): parent_area(area) {;}
		virtual ~NVBViewController() {;}

		/**
		 * View type (2D, 3D, etc). This information is used
		 * to determine which plugins can be activated on this view
		 */
		virtual NVB::ViewType viewType() = 0;

		/// Replace the active page with \a page
		/**
		 * This function should be used by plugins to
		 * tell the view that the current page has been substituted
		 * by another page by the plugin. The new page will use
		 * the supplied \a viz or a default visualiser if needed.
		 */
		virtual void setSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion()) = 0;
		/// Add a new page to the window.
		/**
		 * The new page will use \a viz as visualizer or a default
		 * one if none has been provided.
		 */
		virtual void addSource(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion()) = 0;

		/// Set the visualizer for the active page
		/**
		 * In case \a visualizer is not valid, the current
		 * one will be left unchanged.
		 */
		virtual void setVisualizer(NVBVizUnion visualizer) = 0;

		/// Add a widget that will control plugin parameters
		/**
		 * Plugins will use this function to show widgets
		 * that will allow the user to interact with the plugin.
		 * No limitats are imposed on the \a controlWidget,
		 * but it should not be extremely wide, since the opposite
		 * might inconvenience the user.
		 */
		virtual void addControlWidget(QWidget * controlWidget) = 0;

		/// Set a modal visualizer for the window
		/**
		 * Active visualizer is the one that is interactive.
		 * Thus, there is supposed to be only one of them
		 * per window. Therefore, the old active visualizer
		 * is destroyed after a new one is set, and plugins
		 * are supposed to provide a way for the user to
		 * reinitialize it.
		 */
		virtual void setActiveVisualizer(NVBVizUnion visualizer) = 0;

		/// Open a page in a new window in the same work area
		/**
		 * This function should cause a new window to open. The
		 * type of the window is left to the implementation of
		 * the method in subclasses, but its type is supposed
		 * to be \vtype. The new window should contain
		 * \a page visualized via \a viz.
		 * \sa viewType()
		 */
		virtual NVBViewController * openInNewWindow(NVBDataSource * page, NVBVizUnion viz = NVBVizUnion(), NVB::ViewType vtype = NVB::DefaultView) = 0;

		/// Get the workare that this window belongs to
		/**
		 * This function will return the same NVBWorkingArea
		 * that this window was created with.
		 */
		NVBWorkingArea * area() { return parent_area; }

	protected:
		/// Stores the working area where the window is.
		NVBWorkingArea * parent_area;

	};

#endif
