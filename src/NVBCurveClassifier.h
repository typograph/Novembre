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
#ifndef NVBCURVECLASSIFIER_H
#define NVBCURVECLASSIFIER_H

#include "NVBGraphicsItems.h"
#include "NVBFilterDelegate.h"
#include "NVBDiscrColoring.h"
#include "NVBViewController.h"
#include <QList>
#include <QAction>
#include <QWidget>
#include <QRectF>
#include <QPainter>
#include <QVector>

class QSpinBox;
class QButton;

class NVBMappingDiscrColorModel : public NVBDiscrColorModel {
		Q_OBJECT
	private:
		QVector<int> mapping;
		QList<QColor> colors;

	public:
		NVBMappingDiscrColorModel(QList<QColor> c)
			: NVBDiscrColorModel()
			, colors(c)
			{;}

		virtual ~NVBMappingDiscrColorModel() {;}

		virtual QColor colorize(int index) const {
			if (index >= 0 && index < mapping.size())
				return colors.at(mapping.at(index));
			else
				return Qt::black;
			}

		virtual void setMapping(QVector<int> m) {
			mapping = m;
			emit adjusted();
			}

		virtual void setColors(QList<QColor> c) {
			colors = c;
			emit adjusted();
			}

	};

class NVBCurveClassifier : public NVBSpecFilterDelegate {
		Q_OBJECT
	private:
		NVBSpecDataSource * sprovider;
		NVBMappingDiscrColorModel * colors;
		int nClasses;
		int nIterations;
	public:
		NVBCurveClassifier(NVBSpecDataSource * source);
		virtual ~NVBCurveClassifier();

		NVB_FORWARD_SPECDATA(sprovider)

		virtual const NVBDiscrColorModel * getColorModel() const;

		static QAction * action();
		QWidget * widget();

	protected slots:
		virtual void setSource(NVBDataSource * source);
		virtual void connectSignals();
		void recalculate();
		void setClasses(int k) {
			if (nClasses != k) {
				nClasses = k;
				recalculate();
				}
			}
		void setIterations(int ni) {
			if (nIterations != ni) {
				nIterations = ni;
				recalculate();
				}
			}

	};

class NVBCurveClassifierWidget : public QWidget {
		Q_OBJECT
	private:
		QSpinBox * kBox;
		QSpinBox * miBox;

	public:
		NVBCurveClassifierWidget();
		virtual ~NVBCurveClassifierWidget();

		int k();
		void setK(int k);

		int maxIter();
		void setMaxIter(int mi);

	signals:
		void recalculate();
		void kChanged(int k);
		void maxIterChanged(int mi);
	};

#endif
