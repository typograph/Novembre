//
// Copyright 2006 Timofey <typograph@elec.ru>
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


#include "dimension.h"

#ifndef isfinite
#endif

template <>
void getMemMinMax<double>(const double* const mem, unsigned long size, double & min, double & max) {
	unsigned long i = 0;

	while (i < size && !FINITE(mem[i])) { i++; }

	if (i == size) { min = 0; max = 0; return;}

	min = max = mem[i++];

	for (; i < size; i++)
		if (FINITE(mem[i])) {
			if (mem[i] > max) max = mem[i];

			if (mem[i] < min) min = mem[i];
			}
	}
