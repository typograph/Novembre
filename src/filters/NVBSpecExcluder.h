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
#ifndef NVBSPECEXCLUDER_H
#define NVBSPECEXCLUDER_H

#include "../core/NVBFilterDelegate.h"
#include <QWidget>
#include <QAction>

class QCheckBox;
class QComboBox;
class QRadioButton;

/**
  @author Timofey <timoty@pi-balashov>
*/
class NVBSpecExcluderWidget : public QWidget {
		Q_OBJECT
	private:
		QCheckBox *excl_box;
		QRadioButton *r_excl, *r_incl;
		QComboBox *ecurve_list, *icurve_list;
	public:
		NVBSpecExcluderWidget(NVBSpecDataSource * source);
	signals :
		void activated(bool);
		void curveSelected(int);
		void curveAntiSelected(int);
	public slots:
		void reset();
	private slots:
		void inclSelected(int i);
		void exclSelected(int i);
	};

class NVBSpecExcluder : public NVBSpecFilterDelegate {
		Q_OBJECT
	public:
		enum Mode { NoExclusion = 0, Exclude, Include};

	private :
		NVBSpecDataSource * sprovider;
		NVBDiscrColorModel * scolors;
		int index;
		Mode mode;
	public  :
		NVBSpecExcluder(NVBSpecDataSource * source);
		virtual ~NVBSpecExcluder() {;}

		NVB_FORWARD_SPECDIM(sprovider);

		virtual const NVBDiscrColorModel * getColorModel()  const  {
			if (mode != NoExclusion)
				return scolors;
			else
				return sprovider->getColorModel();
			}

		/// \returns a list of position in real space where curves were taken
		virtual QList<QPointF> positions() const {
			if (mode != NoExclusion && index >= 0) {
				if (mode == Exclude) {
					QList<QPointF> p = sprovider->positions();
					p.takeAt(index);
					return p;
					}
				else {
					return QList<QPointF>() << sprovider->positions().at(index);
					}
				}
			else
				return sprovider->positions();
			}

		/// \returns a list of pointers to the curves of the page in form of \c QwtData .
		virtual QList<QwtData*> getData() const {
			if (mode != NoExclusion && index >= 0) {
				if (mode == Exclude) {
					QList<QwtData*> p = sprovider->getData();
					p.takeAt(index);
					return p;
					}
				else {
					return QList<QwtData*>() << sprovider->getData().at(index);
					}
				}
			else
				return sprovider->getData();
			}

		/// \returns number of points per curve as QSize::width() and number of curves in page as QSize::height()
		virtual QSize datasize() const {
			if (mode != NoExclusion && index >= 0) {
				if (mode == Exclude) {
					return QSize(sprovider->datasize().width(), sprovider->datasize().height() - 1);
					}
				else {
					return QSize(sprovider->datasize().width(), 1);
					}
				}
			else
				return sprovider->datasize();
			}

		QWidget * widget();
		static QAction * action();

	public slots:
		void setActive(bool);
		void setExcludeIndex(int);
		void setIncludeIndex(int);
		virtual void setSource(NVBDataSource * source);

	private slots:
		void parentColorsChanged();
		void parentColorsAboutToBeChanged();
		void checkIndex();

	private:
		void connectSignals();

	signals:
		void delegateReset();
	};

#endif
