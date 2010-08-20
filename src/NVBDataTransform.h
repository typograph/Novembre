#ifndef NVBDATATRANSFORM_H
#define NVBDATATRANSFORM_H

#include <QVector>

class NVBDataSet;

class NVBDataTransform
{
public:
		NVBDataSet * transformDataSet(const NVBDataSet * data, QVector<int> sliceaxes, QVector<int> targetaxes = QVector<int>());

		virtual void operator*() (const double * data, int n, const int * sizes, int m, const int * slice, double * target) const = 0;
};


#endif // NVBDATATRANSFORM_H
