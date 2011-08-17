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


#ifndef NVBSINGLEGRAPHVIEW_H
#define NVBSINGLEGRAPHVIEW_H

#include <qwt_plot.h>
#include "NVBDataGlobals.h"

class NVBPlotCurves;
class NVBDataSet;
class QWidget;

/// A widget to display a single dataset on a graph
class NVBSingleGraphView : public QwtPlot
{
	Q_OBJECT;
	private:
		NVBDataSet * ds;
		NVBPlotCurves * curves;
	
	public:
		/// Construct an @a NVBSingleGraphView with the specified @a parent
		explicit NVBSingleGraphView(NVBDataSet * dataset, QWidget* parent = 0);
		virtual ~NVBSingleGraphView();

		/// Show the supplied @a dataset
		void setDataSet(NVBDataSet * dataset);
		
	public slots:
		void setXAxis(axisindex_t x);
};

#endif // NVBSINGLEGRAPHVIEW_H
