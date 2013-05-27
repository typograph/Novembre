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

#ifndef NVBCOLORWIDGETS_H
#define NVBCOLORWIDGETS_H

#include <QtGui/QWidgetAction>
#include <QtGui/QIconEngineV2>
#include <QtGui/QToolButton>
#include <QtGui/QColor>

class NVBColorMap;
class QMouseEvent;
class QPaintEvent;
class QPainter;

/**
	An icon filled with selected color
	*/
class NVBSolidIconEngine : public QIconEngineV2 {

	private :
		QColor d_color;

	public :
		NVBSolidIconEngine(QColor color): QIconEngineV2(), d_color(color) {;}
		~NVBSolidIconEngine() {;}

		virtual void paint ( QPainter * painter, const QRect & rect, QIcon::Mode, QIcon::State );

		QColor color() { return d_color; }
		void setColor(QColor color) { d_color = color; }
	};

/**
	A toolbutton that shows a color. The color can be selected from a drop-down menu.
	*/
class NVBColorButton : public QToolButton {
		Q_OBJECT
	private :
		NVBSolidIconEngine * colorprovider;
		bool colorSelectable;

	public :
		NVBColorButton(QColor color, QWidget * parent);
		~NVBColorButton() {;}

		inline QColor color();

		bool isColorSelectable() { return colorSelectable; }
		void setColorSelectable( bool selectable ) {
			colorSelectable = selectable;
			setPopupMode(selectable ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup);
			}

	protected :
		void mousePressEvent(QMouseEvent * e);

	private slots:
		void emitTriggeredColor();

	public slots:
		void setColor(QColor color);

	signals :
		void triggered(QColor);

	};

/**
	A widget that paints an NVBColorMap. The widget does no assume ownership of the map.
	*/
class NVBGradientWidget : public QWidget {
	private:
		NVBColorMap * colors;
		int sideMargin;
	public:
		NVBGradientWidget(NVBColorMap * gradient, QWidget * parent);
		virtual ~NVBGradientWidget() {;}

		int margin() const { return sideMargin; }
		void setMargin(int margin) { sideMargin = margin; }

	protected:
		void paint(QPainter * painter, QRect rect);
		virtual void paintEvent(QPaintEvent *);
	};

#endif // NVBCOLORWIDGETS_H
