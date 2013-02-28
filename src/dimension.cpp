#include "dimension.h"

#ifndef isfinite
#endif

template <>
void getMemMinMax<double>(const double* const mem, unsigned long size, double & min, double & max)
{
	unsigned long i = 0;
	while (!FINITE(mem[i]) && i < size) { i++; }
	if (i == size) { min = 0; max = 0; return;}
	min = max = mem[i++];
	for (; i<size; i++)
		if (FINITE(mem[i])) {
			if (mem[i] > max) max = mem[i];
			if (mem[i] < min) min = mem[i];
			}
}
