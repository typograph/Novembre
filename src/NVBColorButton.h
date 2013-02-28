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
#ifndef NVBCOLORBUTTON_H
#define NVBCOLORBUTTON_H

#if QT_VERSION >= 0x040300
#include <QIconEngineV2>
#else
#include <QIconEngine>
#include <QToolBar>
#endif

#include <QToolButton>
#include <QWidget>
#include <QIcon>
#include <QPainter>
#include <QBrush>
#include <QAction>
#include <QPointer>
#include <QStyleOptionToolButton>
#include <QColorDialog>
#include <QMouseEvent>

#if QT_VERSION >= 0x040300
class NVBSolidIconEngine : public QIconEngineV2 {
#else
class NVBSolidIconEngine : public QIconEngine {
#endif

	private :
		QColor d_color;

	public :
#if QT_VERSION >= 0x040300
		NVBSolidIconEngine(QColor color): QIconEngineV2(), d_color(color) {;}
#else
		NVBSolidIconEngine(QColor color): QIconEngine(), d_color(color) {;}
#endif
		~NVBSolidIconEngine() {;}

		virtual void paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state) {
			Q_UNUSED(mode);
			Q_UNUSED(state);
//     painter->save();
			painter->fillRect(rect, QBrush(d_color));
//     painter->restore();
			}

		QColor color() { return d_color; }
		void setColor(QColor color) { d_color = color; }
	};


class NVBColorButton : public QToolButton {
		Q_OBJECT
	private :
		NVBSolidIconEngine* colorprovider;
		bool colorSelectable;

	public :
		NVBColorButton(QColor color, QWidget * parent): QToolButton(parent), colorSelectable(false) {
			colorprovider = new NVBSolidIconEngine(color);
			setDefaultAction(new QAction(QIcon(colorprovider), color.name(), this));
			setAutoRaise(true);
			setCheckable(true);
			defaultAction()->setCheckable(true);
			setChecked(false);
			connect(this, SIGNAL(triggered(QAction*)), SLOT(emitTriggeredColor()));
			}
		~NVBColorButton() {;}

		inline QColor color() { return colorprovider->color(); }

		bool isColorSelectable() { return colorSelectable; }
		void setColorSelectable(bool selectable) {
			colorSelectable = selectable;
			setPopupMode(selectable ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup);
			}

	protected :
		void mousePressEvent(QMouseEvent * e) {
			QStyleOptionToolButton opt;
#if QT_VERSION >= 0x040300
			initStyleOption(&opt);
#else
			opt.init(this);

			if (parentWidget()) {
				if (QToolBar *toolBar = qobject_cast<QToolBar *>(parentWidget())) {
					opt.iconSize = toolBar->iconSize();
					}
				else {
					opt.iconSize = iconSize();
					}
				}

			opt.text = text();
			opt.icon = icon();
			opt.arrowType = arrowType();

			if (isDown())
				opt.state |= QStyle::State_Sunken;

			if (isChecked())
				opt.state |= QStyle::State_On;

			opt.state |= QStyle::State_AutoRaise;

			if (!isChecked() && !isDown())
				opt.state |= QStyle::State_Raised;

			opt.subControls = QStyle::SC_ToolButton;
			opt.activeSubControls = QStyle::SC_None;

			opt.features = QStyleOptionToolButton::None;

			if (popupMode() == QToolButton::MenuButtonPopup) {
				opt.subControls |= QStyle::SC_ToolButtonMenu;
				opt.features |= QStyleOptionToolButton::Menu;
				opt.state |= QStyle::State_MouseOver;
				opt.activeSubControls |= QStyle::SC_ToolButtonMenu;
				}

			if (popupMode() == QToolButton::DelayedPopup)
				opt.features |= QStyleOptionToolButton::PopupDelay;

			opt.toolButtonStyle = toolButtonStyle();

			if (text().isEmpty() && opt.toolButtonStyle != Qt::ToolButtonIconOnly)
				opt.toolButtonStyle = Qt::ToolButtonIconOnly;

			opt.pos = pos();
			opt.font = font();
#endif

			if (e->button() == Qt::LeftButton && (popupMode() == QToolButton::MenuButtonPopup)) {
				QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt,
				                                       QStyle::SC_ToolButtonMenu, this);

				if (popupr.isValid() && popupr.contains(e->pos())) {
					setColor(QColorDialog::getColor(colorprovider->color()));
					return;
					}
				}

			QToolButton::mousePressEvent(e);
			}
//   void mouseReleaseEvent(QMouseEvent *);

	private slots:
		void emitTriggeredColor() {
			emit triggered(colorprovider->color());
			}

	public slots:
		void setColor(QColor color) {
			if (colorprovider) {
				colorprovider->setColor(color);
				update();

				if (isChecked())
					emit triggered(color);
				}
			}

	signals :
		void triggered(QColor);

	};

#endif
