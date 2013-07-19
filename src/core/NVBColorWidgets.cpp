//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBColorWidgets.h"
#include "NVBMap.h"
#include <QtGui/QIcon>
#include <QtGui/QAction>
#include <QtGui/QToolBar>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QColorDialog>
#include <QtGui/QMenu>

void NVBSolidIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) {
	painter->fillRect(rect, QBrush(d_color));
	}

NVBColorButton::NVBColorButton(QColor color, QWidget *parent)
	: QToolButton(parent)
	, colorSelectable(false) {
	colorprovider = new NVBSolidIconEngine(color);
	setDefaultAction(new QAction(QIcon(colorprovider), color.name(), this));
	setAutoRaise(true);
	setCheckable(true);
	defaultAction()->setCheckable(true);
	setChecked(false);
	connect(this, SIGNAL(triggered(QAction*)), SLOT(emitTriggeredColor()));
	}

void NVBColorButton::mousePressEvent(QMouseEvent * e) {
	QStyleOptionToolButton opt;
	initStyleOption(&opt);

	if (e->button() == Qt::LeftButton && (popupMode() == QToolButton::MenuButtonPopup)) {
		QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);

		if (popupr.isValid() && popupr.contains(e->pos())) {
			setColor(QColorDialog::getColor(colorprovider->color()));
			return;
			}
		}

	QToolButton::mousePressEvent(e);
	}

void NVBColorButton::emitTriggeredColor() {
	emit triggered(colorprovider->color());
	}

void NVBColorButton::setColor(QColor color) {
	if (colorprovider) {
		colorprovider->setColor(color);
		update();

		if (isChecked()) emit triggered(color);
		}
	}

NVBGradientWidget::NVBGradientWidget(NVBColorMap *gradient, QWidget *parent)
	: QWidget(parent)
	, colors(gradient)
	, sideMargin(2) {
	}

void NVBGradientWidget::paintEvent(QPaintEvent * e) {
	e->accept();

	if (!colors) return;

	QPainter p(this);
	paint(&p, e->rect());
	}

void NVBGradientWidget::paint(QPainter *p, QRect r) {
	if (!colors) return;

	r.adjust(sideMargin, sideMargin, -sideMargin, -sideMargin);

	for(int i = r.left(); i <= r.right(); i++) {
		p->setPen(QPen(colors->colorize(i * 1.0 / (width() - 2 * sideMargin))));
		p->drawLine(i, r.top(), i, r.bottom() + 1);
		}
	}


