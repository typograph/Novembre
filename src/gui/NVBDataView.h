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
#ifndef NVBDATAVIEW_H
#define NVBDATAVIEW_H

class NVBDataSource;
class QWidget;
class QToolBar;

class NVBDataView {
	public:
/// Unfortunately, Qt does not provide these functions, we must do it ourselves to ensure uniform 2DView
		virtual bool hasWidthForHeight() const = 0;
		virtual int  widthForHeight(int h) const = 0;

//   virtual bool hasHeightForWidth() const { return w()->hasHeightForWidth(); }
//   virtual int  heightForWidth(int h) const { return w()->heightForWidth(); }

/// For use with drag&drop. Will be called by NVBFileWindow to find out if a drag/drop event should be accepted.
		virtual bool acceptsPage(NVBDataSource * source) const = 0;

/// Provides a toolbar for the NVBFileWindow to use.
		virtual QToolBar * generateToolbar(QWidget * parent) const = 0;

/// Typesafe casting. Implement as \code return this;
		virtual const QWidget * w() const = 0;
		virtual QWidget * w() = 0;
	};

#endif
