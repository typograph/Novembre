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

#include "NVBFileFilterDialog.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#include "NVBLogger.h"

NVBFileFilterWidget::NVBFileFilterWidget( int w_index, QAbstractItemModel * columns, QWidget * parent ): QWidget(parent), index(w_index) {
	resize(426, 59);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setContentsMargins(0, 0, 0, 0);

	logic_box = new QComboBox(this);
	logic_box->clear();
	logic_box->insertItems(0, QStringList() << "and" << "and not" << "or" << "or not");
	logic_box->setCurrentIndex(1);
	logic_box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	logic_box->setCurrentIndex(0);
	logic_box->hide();

	where_label = new QLabel("where", this);
	where_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
	where_label->setMinimumSize(logic_box->sizeHint());
	where_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//  where_label->hide();

	if (index == 0)
		horizontalLayout->addWidget(where_label);
	else {
		where_label->hide();
		logic_box->show();
		horizontalLayout->addWidget(logic_box);
		}

//  horizontalLayout->addWidget(where_label);

	column_box = new QComboBox(this);
	column_box->setModel(columns);

	horizontalLayout->addWidget(column_box);

	match_box = new QComboBox(this);
	match_box->insertItems(0, QStringList() << "contains" << "matches" << "is less than" << "is greater than");

	horizontalLayout->addWidget(match_box);

	match_line = new QLineEdit(this);
	match_line->clear();

	horizontalLayout->addWidget(match_line);

	removeButton = new QPushButton("-", this);
	QSize sh = removeButton->sizeHint();
	sh.setWidth(sh.height());
	removeButton->resize(sh);
	removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	horizontalLayout->addWidget(removeButton);
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFilter()));

	}

void NVBFileFilterWidget::initFromStruct(NVBFileFilter f) {
	switch (f.binding) {
		case NVBFileFilter::And : {
			logic_box->setCurrentIndex(0);
			break;
			}

		case NVBFileFilter::AndNot : {
			logic_box->setCurrentIndex(1);
			break;
			}

		case NVBFileFilter::Or : {
			logic_box->setCurrentIndex(2);
			break;
			}

		case NVBFileFilter::OrNot : {
			logic_box->setCurrentIndex(3);
			break;
			}
		}

	column_box->setCurrentIndex(f.column);

	if (!f.match.isEmpty() || f.direction == NVBFileFilter::Equal) {
		match_box->setCurrentIndex(f.match.patternSyntax() == QRegExp::RegExp ? 1 : 0);
		match_line->setText(f.match.isEmpty() ? f.limit.toString() : f.match.pattern());
		}
	else {
		if (f.direction == NVBFileFilter::Less)
			match_box->setCurrentIndex(2);
		else if (f.direction == NVBFileFilter::Greater)
			match_box->setCurrentIndex(3);

		match_line->setText(f.limit.toFullString());
		}

	}

NVBFileFilter NVBFileFilterWidget::getState() {
	NVBFileFilter f;

	f.binding = (NVBFileFilter::FilterBinding) (logic_box->currentIndex());
	f.column = column_box->currentIndex();

	switch (match_box->currentIndex()) {
		case 0:
			f.match = QRegExp(match_line->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
			break;

		case 1:
			f.match = QRegExp(match_line->text(), Qt::CaseSensitive, QRegExp::RegExp);
			break;

		case 2:
			f.direction = NVBFileFilter::Less;
			f.limit = NVBPhysValue(match_line->text());
			break;

		case 3:
			f.direction = NVBFileFilter::Greater;
			f.limit = NVBPhysValue(match_line->text());
			break;
		}

	return f;
	}

NVBFileFilterDialog::NVBFileFilterDialog(QAbstractItemModel * columns, const QList<NVBFileFilter> & filters, QWidget * parent ): QDialog(parent), columnModel(columns) {
	resize(527, 100);
	setSizeGripEnabled(true);
	setModal(true);

	verticalLayout = new QVBoxLayout(this);
//   verticalLayout->setSpacing(0);

	show_label = new QLabel("Show all files", this);
	verticalLayout->addWidget(show_label);

//  verticalLayout->insertWidget(lineEdit);

	verticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	QWidget * widget = new QWidget(this);

	QHBoxLayout *horizontalLayout = new QHBoxLayout(widget);
	horizontalLayout->setMargin(0);

	horizontalLayout->addWidget(addButton = new QPushButton("Add filter", widget));
	connect(addButton, SIGNAL(clicked()), this, SLOT(addFilter()));

	horizontalLayout->addWidget(clearButton = new QPushButton("Clear", widget));
	clearButton->setEnabled(false);
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clearFilters()));

	horizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	okButton = new QPushButton("OK", widget);
	okButton->setDefault(true);
	horizontalLayout->addWidget(okButton);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	cancelButton = new QPushButton("Cancel", widget);
	horizontalLayout->addWidget(cancelButton);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	verticalLayout->addWidget(widget);

	foreach(NVBFileFilter f, filters) {
		addFilter()->initFromStruct(f);
		}

	}

NVBFileFilterWidget * NVBFileFilterDialog::addFilter() {
	NVBFileFilterWidget * widget = new NVBFileFilterWidget(verticalLayout->count() - 3, columnModel, this);
	verticalLayout->insertWidget(verticalLayout->count() - 2, widget);
	connect(widget, SIGNAL(removeFilter(int)), this, SLOT(removeFilter(int)));
	connect(this, SIGNAL(removedFilter(int)), widget, SLOT(renumber(int)));
	clearButton->setEnabled(true);
	return widget;
	}

void NVBFileFilterDialog::removeFilter(int index) {
	QWidget * w = verticalLayout->itemAt(index + 1)->widget();

	if (w) {
		verticalLayout->removeWidget(w);
		delete w;
		emit removedFilter(index);
		}

	clearButton->setEnabled(verticalLayout->count() > 3);
	}

QList< NVBFileFilter > NVBFileFilterDialog::getFilterList() {
	QList< NVBFileFilter > result;

//   NVBOutputDMsg(QString("There are %1 elements").arg(verticalLayout->count()));
	for (int i = 1; i < verticalLayout->count() - 2; i++)
		result << qobject_cast<NVBFileFilterWidget*>(verticalLayout->itemAt(i)->widget())->getState();

	return result;
	}

void NVBFileFilterWidget::renumber( int xindex ) {
	if (index > xindex) index -= 1;

	if (index == 0) {
		logic_box->hide();
		horizontalLayout->removeWidget(0);
		horizontalLayout->insertWidget(0, where_label);
		where_label->show();
		}
	}

void NVBFileFilterDialog::clearFilters() {
	for (int i = verticalLayout->count(); i > 3; i--) {
		QLayoutItem * item = verticalLayout->takeAt(1);
		delete item->widget();
		delete item;
//     delete ;
		}

	clearButton->setEnabled(false);
	}
