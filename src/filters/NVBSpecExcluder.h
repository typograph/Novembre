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
		QCheckBox *incl_extra;
	public:
		NVBSpecExcluderWidget(NVBSpecDataSource * source);
	signals :
		void activated(bool);
		void curveSelected(int);
		void curveAntiSelected(int);
		void neighboursActivated(bool);
	public slots:
		void reset();
	private slots:
		void inclSelected(int i);
		void exclSelected(int i);
	};

class NVBSpecExcluder : public NVBSpecFilterDelegate {
		Q_OBJECT
	public:
		enum Mode { NoExclusion = 0, Exclude, Include, IncludeNeighbours};

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
					QList<QPointF> list;
					if (mode == IncludeNeighbours && index > 0)
						list << sprovider->positions().at(index-1);
					list << sprovider->positions().at(index);
					if (mode == IncludeNeighbours && index+1 < sprovider->positions().count())
						list << sprovider->positions().at(index+1);
					return list;
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
					QList<QwtData*> list;
					if (mode == IncludeNeighbours && index > 0)
						list << sprovider->getData().at(index-1);
					list << sprovider->getData().at(index);
					if (mode == IncludeNeighbours && index+1 < sprovider->getData().count())
						list << sprovider->getData().at(index+1);
					return list;
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
					if (mode == IncludeNeighbours) {
						if (index == 0 || index == sprovider->datasize().height()-1)
							return QSize(sprovider->datasize().width(), 2);
						else
							return QSize(sprovider->datasize().width(), 3);
						}
					else
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
		void setNeighboursActive(bool);
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
