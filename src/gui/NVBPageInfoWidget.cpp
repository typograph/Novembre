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
#include "NVBPageInfoWidget.h"
#include <QLabel>

NVBPageInfoWidget::NVBPageInfoWidget(NVBDataSource * page, QWidget * parent): QWidget(parent) {
	layout = new QGridLayout(this);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, -1);
	layout->setColumnStretch(2, 100);
	setLayout(layout);
	refresh(page);
	}

void NVBPageInfoWidget::refresh(NVBDataSource * page) {
	clear();

	if (page) {
		setWindowTitle(page->name()); // Not really needed

		switch (page->type()) {
			case NVB::TopoPage : {
//        NVB3DDataSource * tPage = qobject_cast<NVB3DDataSource *>(page);
				NVB3DDataSource * tPage = (NVB3DDataSource *)(page);

				if (!tPage) break;

				addRow("Page type", "Topography");
				addRow("Data points", QString("%1 x %2").arg(tPage->resolution().width()).arg(tPage->resolution().height()));
// TODO NVBDimension
				addRow("Size", QString("%1 x %2").arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().width()))).toString(0, 2, 3)).arg(NVBPhysValue(QString("%1 m").arg(fabs(tPage->position().height()))).toString(0, 2, 3)));
				break;
				}

			case NVB::SpecPage : {
//        NVBSpecDataSource * sPage = qobject_cast<NVBSpecDataSource *>(page);
				NVBSpecDataSource * sPage = (NVBSpecDataSource *)(page);

				if (!sPage) break;

				addRow("Page type", "Spectroscopy");
				addRow("Number of curves", QString("%1").arg(sPage->datasize().height()));
				addRow("Points per curve", QString("%1").arg(sPage->datasize().width()));
// TODO NVBDimension
				addRow("X axis span", NVBPhysValue(sPage->boundingRect().width(), sPage->xDim()).toString(0, 2, 3));
				break;
				}

			default : {
				addRow("Page type", "Unknown");
				break;
				}
			}

		foreach(QString key, page->getAllComments().keys()) {
			addRow(key, page->getComment(key));
			}
		}
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
