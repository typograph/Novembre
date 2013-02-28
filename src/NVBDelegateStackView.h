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
#ifndef NVBDELEGATESTACK_H
#define NVBDELEGATESTACK_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLayout>
#include "NVBWidgetStackModel.h"
#include "QCollapsibleBox.h"

class NVBDelegateStackEntry : public QCollapsibleBox {
// Q_OBJECT
	public:
		NVBDelegateStackEntry(QWidget* content = 0, QWidget* parent = 0);
		virtual ~NVBDelegateStackEntry() {;}
// public slots:
//   void collapse();
//   void deactivate();
// signals:
//  void activated( NVBDelegateStackEntry* );
	};

class NVBDelegateStackView : public QScrollArea {
		Q_OBJECT
	private:
		NVBWidgetStackModel * wmodel;
		int selection;
	public:
		Q_PROPERTY(bool empty
		           READ isEmpty());


		NVBDelegateStackView(NVBWidgetStackModel * widgets);
		virtual ~NVBDelegateStackView() {;}

		QSize minimumSizeHint() const;
		QSize sizeHint() const;

		bool isEmpty();

	public slots:
		void setSelectedIndex(const QModelIndex& index);
		void clear();
		void refresh();

		void widgetsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
		void widgetsInserted(const QModelIndex & parent, int start, int end);
		void vizwidgetInserted(const QModelIndex& index);
	signals:
		void empty();
		void contentsChanged();
		void fillStatus(bool);
	};

#endif
