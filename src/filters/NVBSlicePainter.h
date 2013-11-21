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
#ifndef NVBSLICEPAINTER_H
#define NVBSLICEPAINTER_H

#include <QWidget>
#include "../core/NVBDataSource.h"
#include "../core/NVBFilterDelegate.h"
#include "../core/NVBContColoring.h"
#include "../core/NVBDiscrColorModel.h"
#include "../core/NVBContColorModel.h"
#include "../core/NVBPhysStepSpinBox.h"

class NVBSliceColorModel : public NVBDiscrColorModel {
		Q_OBJECT
	private:
		const NVBContColorModel * model;
		const NVBSpecDataSource * sprovider;
		int xindex;
		bool active;
	public:
		NVBSliceColorModel(const NVBContColorModel * model = 0, const NVBSpecDataSource * source = 0);
		virtual ~NVBSliceColorModel();

		virtual QColor colorize(int) const;

	public slots:
		void setSource(const NVBSpecDataSource * source);
		void setModel(const NVBContColorModel * model);
		void setIndex(int index);

	private slots:
		void activate(bool newactive = true) {
			if (active != newactive) {
				active = newactive;
				emit adjusted();
				}
			}
		void deactivate() { active = false; }
	};

class NVBSlicePainterWidget : public QWidget {
		Q_OBJECT
	private:
		const NVBSpecDataSource * sprovider;
		NVBPhysStepSpinBox * sbox;
	private slots:
		void updateLimits();
	public:
		NVBSlicePainterWidget(const NVBSpecDataSource * source);
		virtual ~NVBSlicePainterWidget() {;}
	public slots:
		void setSource(const NVBSpecDataSource * source);
	signals:
		void indexChanged(int);
	};

class NVBSlicePainter : public NVBSpecFilterDelegate {
		Q_OBJECT

	private:
		NVBSpecDataSource * sprovider;
		NVBSliceColorModel * scolors;
		NVBRescaleColorModel * ccolors;
		NVBSlicePainterWidget * swidget;
	public:
		NVBSlicePainter(NVBSpecDataSource * source, NVBContColorModel * model);
		virtual ~NVBSlicePainter() { if (scolors) delete scolors; }

		NVB_FORWARD_SPECDATA(sprovider);

		virtual inline const NVBDiscrColorModel * getColorModel()  const  { return scolors; }

		static QAction * action();
		QWidget * widget();

	protected slots:
		void parentColorsAboutToBeChanged() {}
		void parentColorsChanged() {}
		virtual void setSource(NVBDataSource * source);

	private :
		void connectSignals();

	};

#endif
