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


#ifndef DIMENSION_H
#define DIMENSION_H 1

#include <sys/types.h>
#include <math.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef isfinite
#define FINITE isfinite
#else
#include <float.h>
#ifdef _finite
#define FINITE _finite
#else
#error "NaN values determination impossible"
#endif
#endif

#ifndef uint
typedef unsigned int uint;
#endif
#ifndef round
inline double round(double val) { return floor(val + 0.5); }
#endif

#include "NVBLogger.h"

/*
typedef char dimension;

void getDimension(float & factor, wchar_t * & dim, char * str);
void getDimension(float & factor, wchar_t * & dim, mychar_t * str);
void getDimension(float & factor, wchar_t * & dim, wchar_t * str);
*/

template<typename fromT, typename toT> class scaler;

/*!
  \brief Copies row number @param row of @param src to column number @param col of @param dest
*/

template<typename T>
void arrayTranslate(T* dest, const T* src, uint width, uint height);

template<typename T>
void arrayCopyRowtoCol(T* dest, const T* src, uint row, uint col, uint width, uint height);

template<typename T>
void arrayTranslateRow(T* dest, const T* src, uint row, uint width, uint height);

template<typename T>
void getMemMinMax(const T* const mem, unsigned long size, T & min, T & max);

template<typename fromT, typename toT>
void scaleMem(toT * dest, const scaler<fromT, toT> &_scaler, const fromT* src, unsigned long size);

template<typename T>
void flipMem(T * dest, const T * source, int width, int height, bool flw, bool flh);

template<typename T>
void calcOM(T & offset, T & mult, T imin, T imax, T omin, T omax) {
	mult = (omax - omin) / (imax - imin);
	offset = omax - imax * mult;
	}

/*!
  \brief Provides scaling for different data
 */
template<typename fromT, typename toT>
class scaler {
	private:
		double scale_offset;
		double scale_multiplier;

	public:
//  scaler(long,long,long,long);
//  scaler(long,long);
		scaler(fromT, fromT, toT, toT);
		scaler(double, double);
		scaler();
		~scaler();

		// Input _was_ multiplied and shifted;
		void shift_input(double offset) { scale_offset += offset * scale_multiplier; }
		void multiply_input(double mult) { scale_multiplier *= mult; }
		void change_input(fromT omin, fromT omax, fromT nmin, fromT nmax);

		// Output _will be_ multiplied and shifted
		void shift_output(double offset) { scale_offset += offset; }
		void multiply_output(double mult) {
			scale_offset *= mult;
			scale_multiplier *= mult;
			}
		void change_output(toT omin, toT omax, toT nmin, toT nmax);

		toT scale(fromT value) const { return (toT)(value * scale_multiplier + scale_offset); }
		toT scaleInt(fromT value) const { return (toT)round(value * scale_multiplier + scale_offset); }
		toT scaleLength(fromT value) const { return (toT)(value * scale_multiplier); }
	};

template <typename fromT, typename toT>
scaler<fromT, toT>::~scaler() {
	}

template <typename fromT, typename toT>
scaler<fromT, toT>::scaler() {
	scale_offset = 0;
	scale_multiplier = 1;
	}

template <typename fromT, typename toT>
scaler<fromT, toT>::scaler(double offset, double factor) {
	scale_offset = offset;
	scale_multiplier = factor;
	}

template <typename fromT, typename toT>
scaler<fromT, toT>::scaler(fromT from_min, fromT from_max, toT to_min, toT to_max) {
	if (from_max != from_min) {
		calcOM<double>(scale_offset, scale_multiplier, from_min, from_max, to_min, to_max);
		}
	else {
		scale_multiplier = 0;
		scale_offset = (to_max - to_min) / 2.;
		}
	}

template <typename fromT, typename toT>
void scaler<fromT, toT>::change_input(fromT omin, fromT omax, fromT nmin, fromT nmax) {
	double em, eo;
	calcOM<double>(eo, em, nmin, nmax, omin, omax);
	shift_input(eo);
	multiply_input(em);
	}

template <typename fromT, typename toT>
void scaler<fromT, toT>::change_output(toT omin, toT omax, toT nmin, toT nmax) {
	double em, eo;
	calcOM<double>(eo, em, omin, omax, nmin, nmax);
	multiply_output(em);
	shift_output(eo);
	}

template <typename T>
void arrayTranslateRow(T* dest, const T* src, unsigned int row, unsigned int width, unsigned int height) {
	arrayCopyRowtoCol<T>(dest, src, row, row, width, height);
	}

template <typename T>
void arrayTranslate(T* dest, const T* src, unsigned int width, unsigned int height) {
	for (uint row = 0; row < height; row++)
		arrayCopyRowtoCol<T>(dest, src, row, row, width, height);
	}

template <typename T>
void arrayCopyRowtoCol(T* dest, const T* src, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
	if (row >= height || col >= height) {
		NVBOutputError("Row/Column out of bounds");
		return;
		}

	for (unsigned int i = 0; i < width; i++)
		dest[col + i * height] = src[row * width + i];
	}

template <typename T>
void getMemMinMax(const T* const mem, unsigned long size, T & min, T & max) {
	min = max = mem[0];

	for (unsigned long i = 1; i < size; i++) {
		if (mem[i] > max) max = mem[i];

		if (mem[i] < min) min = mem[i];
		}
	}

template <>
void getMemMinMax<double>(const double* const mem, unsigned long size, double & min, double & max);

template <typename fromT, typename toT>
void scaleMem(toT * dest, const scaler<fromT, toT> &_scaler, const fromT* src, unsigned long size) {
	for (unsigned long i = 0; i < size; i++)
		dest[i] = _scaler.scale(src[i]);
	}

template<typename T>
void flipMem(T * dest, const T * source, int width, int height, bool flw, bool flh) {
	int fi;

	if (!flw && !flh)
		memcpy(dest, source, width * height * sizeof(T));
	else {
		int wflip = flw ? -1 : 1;
		int hflip = flh ? -1 : 1;

		for (int i = 0; i < height; i++) {
			fi = (height - 1 - hflip * (height - 1)) / 2 + hflip * i;

			for (int j = 0; j < width; j++)
				dest[j + i * width] = source[(width - 1 - wflip * (width - 1)) / 2 + wflip * j + fi * width];
			}
		}
	}

#endif
