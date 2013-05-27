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

#include "NVBStandardIcons.h"
#include <QtGui/QIcon>

#include "../icons/novembre_v1.xpm"
#include "../icons/mainwindow.xpm"


class NVBStandardIconFactory {
		QIcon Novembre_icon, Browser_icon, Open_icon;

	public:
		NVBStandardIconFactory()
			: Novembre_icon(_main_novembre) {
			loadIcon(Browser_icon, _main_browse);
			loadIcon(Open_icon, _main_open);
			}

		QIcon getIcon(NVBStandardIcon::IconType icon) {
			switch(icon) {
				case NVBStandardIcon::Novembre:
					return Novembre_icon;

				case NVBStandardIcon::Browser:
					return Browser_icon;

				case NVBStandardIcon::Open:
					return Open_icon;

				default:
					return QIcon();
				}
			}
	};

QIcon getStandardIcon(NVBStandardIcon::IconType icon) {
	static NVBStandardIconFactory factory;
	return factory.getIcon(icon);
	}
