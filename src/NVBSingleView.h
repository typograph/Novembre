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
#ifndef NVBSINGLEVIEW_H
#define NVBSINGLEVIEW_H

#include "NVBDataSource.h"
#include <QtGui/QWidget>
#include <QtCore/QVector>
#include <QtCore/QSignalMapper>

class QToolBar;
class NVBSingle2DView;
class NVBSingleGraphView;

/*
class NVBOverridenDataSet : public NVBDataSet
{
	Q_OBJECT;
private:
	const NVBDataSet * orig;
	QVector<axisindex_t> axmap;
public:
	NVBOverridenDataSet(const NVBDataSet* parent);
	NVBOverridenDataSet(const NVBDataSet* parent, double* data, QVector< axisindex_t > axes);
	virtual ~NVBOverridenDataSet();
	
	void setData(double * data, QVector<axisindex_t> axes = QVector<axisindex_t>());
};
*/

class NVBAverageSlicingDataSet : public NVBDataSet
{
	Q_OBJECT;
private:
	const NVBDataSet * orig;
	
	double * avdata;
	
	QVector<axisindex_t> avaxes; // sorted
	QVector<axisindex_t> slaxes; // after averaging
	QVector<axisindex_t> avas; // axis map before slicing

	QVector<axissize_t> avsizes; // sizes after averaging
	
public:
	NVBAverageSlicingDataSet(const NVBDataSet* parent);
	virtual ~NVBAverageSlicingDataSet();
	
	virtual const double * data() const;
	
	/// Set axes to be averaged
	void setAveragingAxes(QVector<axisindex_t> axes = QVector<axisindex_t>());
	/// Set axes to be sliced
	void setSliceAxes(QVector<axisindex_t> axes = QVector<axisindex_t>());
	/// Set axes to be sliced using indexes after averaging
	void setSliceAxes2(QVector<axisindex_t> axes = QVector<axisindex_t>());
//	void setData(double * data, QVector<axisindex_t> axes = QVector<axisindex_t>());
	/// Set indexes on slice axes
	void setSliceIndexes(QVector< axissize_t > indexes);

public slots:
	void reset();
	
};


class NVBSingleView : public QWidget 
{
	Q_OBJECT;
public:	
	enum Type {
		Graph = 1,
		Image = 2
		};

private:
	
	const NVBDataSet * ds;
	NVBAverageSlicingDataSet * ods;
	
	QToolBar * viewTB;
	
	NVBSingle2DView * view2D;	
	NVBSingleGraphView * viewGraph;
	
	// -------------------------

	void createView(NVBSingleView::Type type);
	
public:	

	explicit NVBSingleView(const NVBDataSet * dataSet, QWidget* parent = 0);
	virtual ~NVBSingleView();

signals:
	void dismissed();
	
public slots:
	void setDataSet(const NVBDataSet * ds);
	void show3DView() { NVBOutputPMsg("3D view not implemented"); }
	void show2DView() { createView(Image); }
	void showGraphView() { createView(Graph); }
	
private slots:
	void targetsChanged(axisindex_t,axisindex_t,QVector<axisindex_t>,QVector<axisindex_t>);
	void sliceChanged(QVector<axissize_t>);
	
};

class NVBSingleViewSliders : public QWidget
{
Q_OBJECT
private:
	enum AxisOp {
		  asX = 0
		, asY = 1
		, Average = 2
		, Slice = 3
		, Keep = 4
		};
		
	NVBSingleView::Type viewType;
	
	QVector<AxisOp> axisOps;
	QVector<axissize_t> axisSlices;
		
	QSignalMapper cbs, slds;

	// -- Various cache variables
	
	axisindex_t nPlotAxes;
	
	QVector<axisindex_t> axes; // X / Y axes after averaging
	QVector<axisindex_t> av_axes; // Averaging axes
	QVector<axisindex_t> sl_axes; // Slice axes indexes after averaging
	QVector<axissize_t> sl_ixes;	// Slice indexes

	void updateCache();
	void updateWidgets();
	
public:	
	explicit NVBSingleViewSliders(const NVBDataSet* ds, NVBSingleView::Type type, QWidget* parent = 0);
	virtual ~NVBSingleViewSliders();

	inline axisindex_t xAxis() { return axes.first(); }
	inline axisindex_t yAxis() { return axes.last(); }
	inline QVector<axisindex_t> xyAxes() { return axes; }
	inline QVector<axisindex_t> averagedAxes() { return av_axes; }
	inline QVector<axisindex_t> slicedAxes() { return sl_axes; }
	// inline QVector<axisindex_t> keptAxes() { return ;}

	inline QVector<axissize_t> sliceIndexes() { return sl_ixes; }
	
signals:
	void sliceIndexesChanged(QVector<axissize_t> indexes);
	void axisTargetsChanged(QVector<AxisOp> targets);
	/// Notifies about changes to axis targets. \a x and \a y are indexes of axes selected as X and Y, respectively
	void axisTargetsChanged(axisindex_t x, axisindex_t y, QVector<axisindex_t> average, QVector<axisindex_t> slice);

private slots:
	void slicePosChanged(int);
	void axisOpChanged(int);
	
};

#endif // NVBDATASETAVERAGER_H
