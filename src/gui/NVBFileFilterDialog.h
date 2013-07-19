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

#ifndef NVBFILEFILTERDIALOG_H
#define NVBFILEFILTERDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QRegExp>
#include "NVBUnits.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;
class QToolButton;
class QAbstractItemModel;

struct NVBFileFilter {
	enum FilterBinding {And = 0, AndNot = 1, Or = 2, OrNot = 3};
	enum FilterDirection {Less, Equal, Greater};
	int column;
	NVBPhysValue limit;
	QRegExp match;
	FilterBinding binding;
	FilterDirection direction;
	NVBFileFilter(int _column = 0, QRegExp _match = QRegExp(), FilterBinding _binding = And)
		: column(_column)
		, match(_match)
		, binding(_binding)
		, direction(Equal)
		{;}
	NVBFileFilter(int _column, NVBPhysValue _limit, FilterDirection _direction = Less, FilterBinding _binding = And)
		: column(_column)
		, limit(_limit)
		, binding(_binding)
		, direction(_direction)
		{;}
	};

class NVBFileFilterWidget : public QWidget {
		Q_OBJECT
	private:
		int index;

		QHBoxLayout *horizontalLayout;

		QComboBox *logic_box;
		QLabel *where_label;

		QComboBox *column_box;
		QComboBox *match_box;
		QLineEdit *match_line;

		QPushButton *removeButton;

	public:
		NVBFileFilterWidget(int w_index, QAbstractItemModel * columns, QWidget * parent = 0);

		NVBFileFilter getState();
		void initFromStruct(NVBFileFilter f);

	public slots:
		void renumber(int xindex);
	private slots:
		void removeFilter() {
			emit removeFilter(index);
			}
	signals:
		void removeFilter(int);
	};

class NVBFileFilterDialog : public QDialog {
		Q_OBJECT
	private:
		QVBoxLayout *verticalLayout;
		QLabel *show_label;
		QPushButton *addButton;
		QPushButton *clearButton;
		QPushButton *okButton;
		QPushButton *cancelButton;

		QAbstractItemModel * columnModel;

	public:
		NVBFileFilterDialog(QAbstractItemModel * columns, const QList<NVBFileFilter> & filters = QList<NVBFileFilter>(), QWidget * parent = 0);

		QList<NVBFileFilter> getFilterList();

	public slots:
		NVBFileFilterWidget * addFilter();
		void removeFilter(int);
		void clearFilters();

	signals:
		void removedFilter(int);

	};

#endif // NVBFILEFILTERDIALOG_H
