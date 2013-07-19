//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBDATATRANSFORMS_H
#define NVBDATATRANSFORMS_H

#include <QtCore/QVector>
#include "NVBDataGlobals.h"

class NVBDataSet;
/*
class NVBMaxMinTransform : public NVBDataTransform {
	private:
		virtual double singleValueTransform(double) const { return 0; }
		static double min (const double * data, axisindex_t n, const axissize_t * sizes);
		static double max (const double * data, axisindex_t n, const axissize_t * sizes);
		static void minmax (const double * data, axisindex_t n, const axissize_t * sizes, double & dmin, double & dmax);
	public:
		NVBMaxMinTransform();

		static double findMinimum(const NVBDataSet * data);
		static double findMinimum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> targetaxes);
		static double * findMinima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes);
		static double findMaximum(const NVBDataSet * data);
		static double findMaximum(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axissize_t> targetaxes);
		static double * findMaxima(const NVBDataSet * data, QVector<axisindex_t> sliceaxes);

		virtual void operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const;
};
*/

namespace NVBMaxMinTransform {
double min (const double * data, axisindex_t n, const axissize_t * sizes);
double max (const double * data, axisindex_t n, const axissize_t * sizes);
axissize_t min_index (const double * data, axisindex_t n, const axissize_t * sizes);
axissize_t max_index (const double * data, axisindex_t n, const axissize_t * sizes);
void minmax (const double * data, axisindex_t n, const axissize_t * sizes, double & dmin, double & dmax);
double findMinimum(const NVBDataSet * data);
double findMaximum(const NVBDataSet * data);
void findLimits(const NVBDataSet * data, double & dmin, double & dmax);
}

namespace NVBAverageTransform {
double average (const double * data, axisindex_t n, const axissize_t * sizes);
}
#endif // NVBDATATRANSFORMS_H
