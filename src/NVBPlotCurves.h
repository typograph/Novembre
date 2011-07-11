/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef NVBPLOTCURVES_H
#define NVBPLOTCURVES_H

#include "NVBDataSource.h"
#include <qwt6/qwt_plot_item.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_compat.h>

class NVBPlotCurves : public QObject, public QwtPlotItem
{
Q_OBJECT;
protected:
  const NVBDataSet * parent;
	axisindex_t axisX;
	QVector<axisindex_t> slice_axes;
  QRectF rect;
	unsigned int max_curves;
	double * xdata;
//	double * ydata;
	
	QwtPlotCurve * drawCurve;

public:
  NVBPlotCurves(const NVBDataSet * data);
  virtual ~NVBPlotCurves();
	
  virtual int rtti () const { return QwtPlotItem::Rtti_PlotUserItem + 117; }
	virtual void draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF & canvasRect) const;
  virtual QRectF boundingRect() const { return rect; }
  
  inline unsigned int maxCurves() const { return max_curves; }
  void setMaxCurves(unsigned int max);
  void setXAxis(axisindex_t x);
//	void setSliceAxes(QVector<axisindex_t> axes = QVector<axisindex_t>());
//	void setSliceIndexes(QVector<axissize_t> indexes = QVector<axissize_t>());
  
public slots:
	void setDataSet(const NVBDataSet * data = 0);
private slots:
  void parentDataChanged();
  void parentAxesChanged();
};

#endif // NVBPLOTCURVES_H