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
#ifndef NVBAVERAGECOLOR_H
#define NVBAVERAGECOLOR_H

class NVBAverageColor {
	private:
		QList<QColor> colors;
		mutable QColor avcolor;
		mutable bool averaged;

		void average() const {
			if (colors.size() <= 0) return;

			uint r = 0, g = 0, b = 0; // TODO think about HSV
			foreach(QColor c, colors) {
				r += c.red();
				g += c.green();
				b += c.blue();
				}
			r /= colors.size();
			g /= colors.size();
			b /= colors.size();
			avcolor = QColor::fromRgb(r, g, b);
			averaged = true;
			}
	public:
		NVBAverageColor(): averaged(false) {;}

		inline QColor color() const {
			if (!averaged) average();

			return avcolor;
			}

		inline NVBAverageColor & operator<<(const QColor & color) {
			colors << color;
			return *this;
			}
	};

#endif
