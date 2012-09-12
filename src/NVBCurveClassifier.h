//
// C++ Interface: NVBDiscrColorPainter
//
// Description:
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
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
			if (index >=0 && index < mapping.size())
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
