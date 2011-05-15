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

class NVBSingle2DView;

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

class NVBAveragingDataSet : public NVBDataSet
{
	Q_OBJECT;
private:
	const NVBDataSet * orig;
	QVector<axisindex_t> avaxes;	
	QVector<axisindex_t> axmap;
public:
	NVBAveragingDataSet(const NVBDataSet* parent);
	virtual ~NVBAveragingDataSet();
	
	virtual const double * data() const;
	
	void setAveragingAxes(QVector<axisindex_t> axes = QVector<axisindex_t>());
//	void setData(double * data, QVector<axisindex_t> axes = QVector<axisindex_t>());
};

class NVBSingleView : public QWidget 
{
	Q_OBJECT;
private:
	enum AxisOp {
		  asX = 0
		, asY = 1
		, Average = 2
		, Slice = 3
		};
	
	QVector<AxisOp> axisOps;
	QVector<axissize_t> axisSlices;
		
	QSignalMapper cbs, slds;
	
	const NVBDataSet * ds;
	NVBAveragingDataSet * ods;
	
	NVBSingle2DView * view;
	
	// -- Various cache variables
	
	QVector<axisindex_t> axes; // X / Y axes after averaging
	QVector<axisindex_t> av_axes; // Averaging axes
	QVector<axisindex_t> sl_axes; // Slice axes indexes after averaging
	QVector<axissize_t> sl_ixes;	// Slice indexes
	
	// -------------------------

	void updateAverage();
	void updateWidgets();
	
public:	
	explicit NVBSingleView(const NVBDataSet * dataSet, QWidget* parent = 0);
	virtual ~NVBSingleView();
	
private slots:
	void slicePosChanged(int);
	void axisOpChanged(int);
};

#endif // NVBDATASETAVERAGER_H
