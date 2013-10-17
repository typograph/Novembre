//
// Copyright 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBCONVERTERMENU_H
#define NVBCONVERTERMENU_H

#include <QtGui/QMenu>
#include <QtCore/QSignalMapper>

class NVBFile2ImageConverter;
class QActionGroup;

class NVBConverterMenu : public QMenu
{
	Q_OBJECT
public:
	~NVBConverterMenu();
	NVBConverterMenu(QWidget* parent);
signals:
	void converterSelected(NVBFile2ImageConverter* converter);
	
private:
	QList<NVBFile2ImageConverter*> converters;
	QSignalMapper mapper;
	QActionGroup * group;
	QAction* addConverter(QString caption, NVBFile2ImageConverter* converter);
private slots:
	void selected(int index);

};

#endif // NVBCONVERTERMENU_H
