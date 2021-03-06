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

#include "NVBPageInfoWidget.h"
#include "NVBFile.h"

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

NVBPageInfoWidget::NVBPageInfoWidget(NVBDataSet * page, QWidget * parent): QWidget(parent) {
	layout = new QGridLayout(this);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, -1);
	layout->setColumnStretch(2, 100);
	setLayout(layout);
	refresh(page);
	}

QString dataShapeString(QVector<axissize_t> sizes) {
	QStringList sszs;
	foreach (axissize_t s, sizes)
	sszs << QString::number(s);
	return sszs.join(" x ");
	}

void NVBPageInfoWidget::refresh(NVBDataSet * page) {
	clear();

	if (page) {
		setWindowTitle(page->name()); // Not really needed

		switch (page->type()) {
			case NVBDataSet::Topography :
				addRow("Page type", "Topography");
				break;

			case NVBDataSet::Spectroscopy :
				addRow("Page type", "Spectroscopy");
				break;

			case NVBDataSet::Undefined :
				addRow("Page type", "Undefined");
				break;
			}

		addRow("Data shape", dataShapeString(page->sizes()));
		addRow("Data units", page->dimension().toStr());
		addRow("", "");
		addComments(page->getAllComments());

		if (page->dataSource()) {
			addRow("", "");
			addComments(page->dataSource()->getAllComments());

			if (page->dataSource()->origin()) {
				addRow("", "");
				addComments(page->dataSource()->origin()->getAllComments());
				}
			}
		}
	}

void NVBPageInfoWidget::addComments(NVBDataComments c) {
	foreach(QString key, c.keys())
	addRow(key, c.value(key));
	}


void NVBPageInfoWidget::addRow(QString property, QString value) {
	int row = layout->count();
	QLabel * l = new QLabel(property);
	l->setAlignment(l->alignment() | Qt::AlignRight);
	layout->addWidget(l, row, 0);
	layout->addWidget(new QLabel(":"), row, 1);
	QLabel * lv = new QLabel(value);
	lv->setWordWrap(true);
	layout->addWidget(lv, row, 2);
	layout->setRowStretch(row, 1);
	}

void NVBPageInfoWidget::clear() {
	while (layout->count()) {
		QLayoutItem * i = layout->takeAt(0);

		if (i) {
			if (i->widget()) delete i->widget();

			delete i;
			}
		}
	}
