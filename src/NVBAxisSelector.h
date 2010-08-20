/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef NVBAXISSELECTOR_H
#define NVBAXISSELECTOR_H

#include <QtCore/QList>
#include "NVBDimension.h"

class NVBDataSet;

struct NVBSelectorAxis {
	NVBSelectorAxis & byIndex(int index);
	NVBSelectorAxis & byMinLength(int length);

	NVBSelectorAxis & byDimension(NVBDimension dimension);
	NVBSelectorAxis & byMapDimension(int dimension);
	NVBSelectorAxis & byTypeId(int typeId);
	<template class T>
	inline NVBSelectorAxis & byType<T>() { return byTypeId(qMetaTypeId<T>()); }
};

struct NVBSelectorCase {
	enum Type { Undefined, AND, OR };
	int id;
	Type t;
	union {
		QList<NVBSelectorCase> cases;
		QList<NVBSelectorAxis> axes;
	};

	NVBSelectorCase(int caseId = 0, Type caseType = Undefined)
	: id(caseId)
	, t(caseType)
	{;}

	NVBSelectorInstance instantiate(const NVBDataSet * dataSet);

	NVBSelectorCase & addCase(int caseId = 0, Type caseType = Undefined);

	NVBSelectorAxis & addAxis();
	inline NVBSelectorAxis & addAxisByIndex(int index) { return addAxis().byIndex(index); }
	inline NVBSelectorAxis & addAxisByMinLength(int length) { return addAxis().byMinLength(length); }

	inline NVBSelectorAxis & addAxisByDimension(NVBDimension dimension) { return addAxis().byDimension(dimension); }
	inline NVBSelectorAxis & addAxisByMapDimension(int dimension) { return addAxis().byMapDimension(dimension); }
	inline NVBSelectorAxis & addAxisByTypeId(int typeId) { return addAxis().byTypeId(typeId); }
	<template class T>
	inline NVBSelectorAxis & addAxisByType<T>() { return addAxisByTypeId(qMetaTypeId<T>()); }

	NVBSelectorAxis & addDependentAxis();
};

class NVBSelectorInstance {
};

class NVBAxisSelector : public NVBSelectorCase {
	public:
		NVBAxisSelector():NVBSelectorCase() {;}
		~NVBAxisSelector() {;}
};

#endif // NVBAXISSELECTOR_H
