//
// Copyright 2006 Timofey <typograph@elec.ru>
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


#ifndef NVBDISCRCOLORING_H
#define NVBDISCRCOLORING_H

#include "NVBDiscrColorModel.h"
#include "dimension.h"

namespace NVBColoring {

class NVBRandomDiscrColorModel : public NVBDiscrColorModel {
	private:
		mutable  QList<QColor> colors;
		QColor newcolor() const;
	public:
		NVBRandomDiscrColorModel(int ncolors = 0);
		virtual ~NVBRandomDiscrColorModel() {;}
		virtual QColor colorize(int) const;
		void resize(int ncolors) const;
	};

/*
class NVBRGBRampDiscrColorModel : public NVBDiscrColorModel {
private:
public:
    NVBRGBRampDiscrColorModel(QRgb start, QRgb end) {;}
    virtual ~NVBRGBRampDiscrColorModel() {;}

    virtual QColor colorize(int) const { return QColor(Qt::black); }
};
*/

class NVBConstDiscrColorModel : public NVBDiscrColorModel {
	protected:
		QColor c;
	public:
		NVBConstDiscrColorModel(QColor color): c(color) {;}
		virtual ~NVBConstDiscrColorModel() {;}

		virtual QColor colorize(int) const { return c; }

	};

class NVBRGBRampDiscrColorModel : public NVBDiscrColorModel {
	protected:
		int uplimit;
		QRgb cstart, cend;

		scaler<double, QRgb> * sc;

		void initScaler() {
			if (sc) delete sc;

			sc = new scaler<double, QRgb>(0, 1, cstart, cend);
			emit adjusted();
			}

	public:
		NVBRGBRampDiscrColorModel(int ncurves, QRgb start, QRgb end)
			: NVBDiscrColorModel(), uplimit(ncurves), cstart(start), cend(end), sc(0) { initScaler(); }
		NVBRGBRampDiscrColorModel(int ncurves, QColor start, QColor end)
			: NVBDiscrColorModel(), uplimit(ncurves), cstart(start.rgba()), cend(end.rgba()), sc(0) { initScaler(); }
		virtual ~NVBRGBRampDiscrColorModel() { if (sc) delete sc; }

		virtual QColor colorize(int index) const { return QColor(sc->scaleInt(index * 1.0 / uplimit)); }

		void setNCurves(int ncurves) { uplimit = ncurves; emit adjusted();}

		void setStartColor(QRgb color) { cstart = color; initScaler(); }
		void setStartColor(QColor color) { setStartColor(color.rgba());}
		void setEndColor(QRgb color) { cend = color; initScaler(); }
		void setEndColor(QColor color) { setEndColor(color.rgba()); }

	};

class NVBIndexedDiscrColorModel : public NVBDiscrColorModel, protected QList<QColor> {
		Q_OBJECT
	private:
		QColor defclr;
//  QList<QColor> colors;
	public:
		NVBIndexedDiscrColorModel(QColor default_color = Qt::black): NVBDiscrColorModel(), QList<QColor>(), defclr(default_color) {;}
		NVBIndexedDiscrColorModel(const NVBDiscrColorModel * model, int nc, QColor default_color = Qt::black): NVBDiscrColorModel(), QList<QColor>(), defclr(default_color) { setModel(model, nc); }
		virtual ~NVBIndexedDiscrColorModel() {;}

		virtual QColor colorize(int index) const {
			if (index >= 0 && index < size())
				return at(index);
			else
				return defclr;
			}

		virtual void setColor(QList<int> indexes, QColor color) {
			foreach(int index, indexes) {
				if (size() <= index) {
					for (int i = size(); i <= index; i++)
						append(defclr);
					}

				replace(index, color);
				}
			emit adjusted();
			}

		virtual void setColor(int index, QColor color) {
			if (size() <= index) {
				for (int i = size(); i <= index; i++)
					append(defclr);
				}

			replace(index, color);
			emit adjusted();
			}

		virtual void setModel(const NVBDiscrColorModel * model, int nc) {
			clear();
			operator<<(model->colorize(0, nc - 1));
			}

		void clear() { QList<QColor>::clear(); }
		/*
		  inline int size() const { return colors.size(); }
		*/
		NVBIndexedDiscrColorModel & operator<< (const QColor & color) {
			QList<QColor>::operator<<(color);
			emit adjusted();
			return *this;
			}

		NVBIndexedDiscrColorModel & operator<< (const QList<QColor> & colors) {
			QList<QColor>::operator<<(colors);
			emit adjusted();
			return *this;
			}

	};

}
#endif
