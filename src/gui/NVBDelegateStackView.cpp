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
#include "NVBDelegateStackView.h"
#include <QScrollBar>

NVBDelegateStackEntry::NVBDelegateStackEntry(QWidget * content, QWidget * parent): QCollapsibleBox(content->windowTitle(), parent) {
	setExclusiveOwner(false);
	setArrowIndicators();
//  setMargins(0,0,0,0);
	setAlignment(Qt::AlignRight);
	setIndicatorPosition(QCollapsibleBox::Left);
	setHeaderStyle(QCollapsibleBox::Horizontal);
	setCentralWidget(content);
	expand();
//   contents->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
	}

// void NVBDelegateStackEntry::collapse()
// {
//   ((QPushButton*)(layout()->itemAt(0)->widget()))->setChecked(false);
// }
//
// NVBDelegateStackEntry::~ NVBDelegateStackEntry()
// {
//   layout()->removeWidget(contents);
//   contents->setParent(0);
//   contents->hide();
// }

// ===============

NVBDelegateStackView::NVBDelegateStackView(NVBWidgetStackModel * widgets): QScrollArea(), wmodel(widgets), selection(-1) {
//  setWidgetResizable(true);
	QVBoxLayout * l = new QVBoxLayout();
//  l->setSizeConstraint(QLayout::SetMinimumSize);
#if QT_VERSION >= 0x040300
	l->setContentsMargins(0, 0, 0, 0);
#else
	l->setMargin(0);
#endif
//  setLayout(l);

	QWidget * w = new QWidget(this);
	w->setLayout(l);
	setWidget(w);

	setWidgetResizable(true);
	setAlignment(Qt::AlignTop);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	connect(wmodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), SLOT(widgetsAboutToBeRemoved(const QModelIndex&, int, int)));
	connect(wmodel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(widgetsInserted(const QModelIndex&, int, int)));
//  connect(wmodel,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),SLOT(vizwidgetInserted(const QModelIndex&)));
	}

void NVBDelegateStackView::setSelectedIndex(const QModelIndex & index) {
	if (selection != index.row()) {
		selection = index.row();
		refresh();
		}
	}

void NVBDelegateStackView::clear() {
	QLayoutItem *child;

	while ((child = widget()->layout()->takeAt(0)) != 0) {
		if (child->widget())
			delete child->widget();

		delete child;
		}

	emit empty();
	}

// void NVBDelegateStackEntry::deactivate()
// {
// }

void NVBDelegateStackView::widgetsInserted(const QModelIndex & parent, int start, int end) {
	if (parent.isValid()) {
		if (parent.row() == selection) refresh();
		}
	else {
		if (selection >= start)
			selection += end - start + 1;
		}
	}

void NVBDelegateStackView::widgetsAboutToBeRemoved(const QModelIndex & parent, int start, int end) {
	if (parent.isValid()) {
		if (parent.row() == selection) refresh();
		}
	else {
		if (selection > end)
			selection -= end - start + 1;
		else if (selection >= start)
			clear();
		}
	}

void NVBDelegateStackView::vizwidgetInserted(const QModelIndex & index) {
	// FIXME Can be more computational effective
	if (selection == index.row())
		refresh();
	}

void NVBDelegateStackView::refresh() {
	clear();

	if (selection == -1) return;

	QModelIndex index = wmodel->index(selection, 0, QModelIndex());

	QWidget * tw;

	// First the painter

	QVariant v = wmodel->data(index);
	tw = v.value<QWidget*>();

	if (v.isValid() && tw) {
		widget()->layout()->addWidget(new NVBDelegateStackEntry(tw, this));
		// A separator
		widget()->layout()->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
		}

	// Then the filters

	for (int i = wmodel->rowCount(index) - 1; i >= 0; i--) {
		tw = wmodel->data(wmodel->index(i, 0, index)).value<QWidget*>();

		if (tw) widget()->layout()->addWidget(new NVBDelegateStackEntry(tw, this));
		}

	widget()->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	updateGeometry();

	emit contentsChanged();

	if (widget()->layout()->count() <= 1) emit empty();

	emit fillStatus(widget()->layout()->count() > 1);
	}

QSize NVBDelegateStackView::minimumSizeHint() const {
	QSize s = widget()->minimumSizeHint();
	s += QSize(2 * frameWidth(), 2 * frameWidth());

	if (s.height() > height())
		s.setWidth(s.width() + verticalScrollBar()->sizeHint().width());

	if (widget() && widget()->layout()->count() && widget()->layout()->itemAt(0)->widget()) {
		s.setHeight(widget()->layout()->itemAt(0)->widget()->minimumSizeHint().height());
		}
	else {
		s.setHeight(0);
		}

	return s;
	}

QSize NVBDelegateStackView::sizeHint() const {
	return QScrollArea::sizeHint();
	}

bool NVBDelegateStackView::isEmpty() {
	return (widget() == 0) || (widget()->layout()->count() <= 1);
	}
