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

#ifndef NVBPAGEINFOWIDGET_H
#define NVBPAGEINFOWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QString>
#include "NVBDataSource.h"

class QGridLayout;

class NVBPageInfoWidget : public QWidget {
		Q_OBJECT
	private:
		QGridLayout * layout;
		void addRow(QString property, QString value);
		void addComments(NVBDataComments c);
	protected:
		void clear();
	public:
		NVBPageInfoWidget(NVBDataSet * page = 0, QWidget * parent = 0);
		~NVBPageInfoWidget() {;}
	public slots:
		void refresh(NVBDataSet * page = 0);
	};

#endif
