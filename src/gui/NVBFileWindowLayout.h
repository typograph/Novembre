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
#ifndef NVBFILEWINDOWLAYOUT_H
#define NVBFILEWINDOWLAYOUT_H

#include <QLayout>

class QWidget;
class NVBDataView;

class NVBFileWindowLayout : public QHBoxLayout {
	private:
		bool active;

		int minAcceptableWidth(int h) const;
		int maxAcceptableHeight(int w) const;
#ifndef NVB_NO_FW_DOCKS
		QWidget * leftDock;
		QWidget * rightDock;
#endif
		NVBDataView * centralView;
		QLayoutItem * toolBar;

	public:
		NVBFileWindowLayout(QWidget * parent);
		virtual ~NVBFileWindowLayout();

#ifndef NVB_NO_FW_DOCKS
		void setLeftDock(QWidget * dock);
		void setRightDock(QWidget * dock);
#endif
		void setToolBar(QLayoutItem * toolbar);
		void setView(NVBDataView * view);

		virtual QSize minimumSize() const { return QSize(100, 100); } // TODO
		virtual QSize sizeHint() const { return QSize(minAcceptableWidth(200), 200); }
		virtual void setGeometry(const QRect & r);

		QSize closestAcceptableSize(const QSize & request, int width_correction, int height_correction) const;
	};

#endif
