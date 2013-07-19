//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBGRADIENTMENU_H
#define NVBGRADIENTMENU_H

#include <QtGui/QMenu>
#include <QtCore/QString>
#include <QtCore/QSignalMapper>
#include <QtGui/QWidgetAction>

class NVBColorMap;

/**
	An action for a gradient menu. The action assumes ownership of the map.
	*/
class NVBGradientAction : public QWidgetAction {
		Q_OBJECT
	protected :
		NVBColorMap * colors;
	protected slots:
		void emitGradient();
	public:
		NVBGradientAction(NVBColorMap * gradient, QObject * parent);
		virtual ~NVBGradientAction();
		virtual QWidget *	createWidget ( QWidget * parent );
	signals:
		void triggered(const NVBColorMap * gradient);
	};

class NVBColorSelectMenu : public QMenu {
		Q_OBJECT
		QSignalMapper colormapper;
		QAction * separator;
	public:
		NVBColorSelectMenu( QWidget * parent = 0 );
		virtual ~NVBColorSelectMenu() {;}

		void addColor(QColor color, QString name = QString());

	private slots:
		void colorClick(int c);
		void selectExtraColor();

	signals:
		void colorSelected(QColor color);
	};

/**
	A menu to select a gradient.
	*/
class NVBGradientMenu : public QMenu {
		Q_OBJECT
	private slots:
		void selectGradient(const NVBColorMap * gradient);

	public:
		NVBGradientMenu( QWidget * parent = 0) : QMenu(parent) {;}
		virtual ~NVBGradientMenu() {;}

		NVBGradientAction * addGradientAction(NVBColorMap * gradient);

		void addGradientAction(NVBGradientAction * gradientAction);
	};

#endif // NVBGRADIENTMENU_H
