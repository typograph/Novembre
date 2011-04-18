/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov                                *
 *   Timofey.Balashov@pi.uka.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/
#ifndef NVBSCALER_H
#define NVBSCALER_H

#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

template<class fromT, class toT>
class NVBValueScaler;

template <typename T>
void arrayCopyRowtoCol( T* dest, const T* src, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
  if ( row >= height || col >= height ) throw;
  for (unsigned int i = 0; i < width; i++)
    dest[col + i*height] = src[row*width + i];
}

template <typename T>
void arrayTranslateRow( T* dest, const T* src, unsigned int row, unsigned int width, unsigned int height) {
arrayCopyRowtoCol<T>(dest,src,row,row,width,height);
}

template <typename T>
void arrayTranslate( T* dest, const T* src, unsigned int width, unsigned int height) {
for (uint row = 0; row<height; row++)
 arrayCopyRowtoCol<T>(dest,src,row,row,width,height);
}

template <typename T>
void getMemMinMax(T* mem, unsigned long size, T & min, T & max) {
  min = max = mem[0];
  for (unsigned long i = 1; i<size; i++) {
    if (mem[i] > max) max = mem[i];
    if (mem[i] < min) min = mem[i];
    }
}

template <typename fromT, typename toT>
void scaleMem(toT * dest, const NVBValueScaler<fromT,toT> & _scaler, const fromT* src, unsigned long size) {
  for (unsigned long i = 0; i<size; i++)
    dest[i] = _scaler.scale(src[i]);
}

template<typename T>
void flipMem(T * dest, const T * source, int width, int height, bool flw, bool flh) {
  int fi;
  if (!flw && !flh)
    memcpy(dest,source,width*height*sizeof(T));
  else {
    int wflip = flw ? -1 : 1;
    int hflip = flh ? -1 : 1;
    for (int i = 0; i<height; i++) {
      fi = (height - 1 - hflip*(height-1))/2 + hflip*i;
      for (int j = 0; j<width; j++)
        dest[j+i*width] = source[(width-1 - wflip*(width-1))/2 + wflip*j+fi*width];
      }
    }
}

template<typename T>
void flipMem(T * data, int width, int height, bool flw, bool flh) {
	T t;
  if (!flw && !flh)
   return;
	else if (flw && !flh) {
    for (int i = (height-1)*width; i>=0; i -= width)
      for (int j = (int)floor(width/2)-1; j>=0; j--) {
				t = data[j+i];
				data[j+i] = data[width-j-1+i];
				data[width-j-1+i] = t;
			}
		}
	else if (!flw && flh) {
    for (int i =(int)floor(height/2)-1; i>=0; i--) {
			memcpy(data+i*width,data+(height-i-1)*width,sizeof(T)*width);
			}		
		}
  else {
    for (int i = 0; i<height; i++) {
      int fi = height-1-i;
      for (int j = 0; j<width; j++) {
				t = data[j+i*width];
        data[j+i*width] = data[width - 1 - j + fi*width];
				data[width-1-j+fi*width] = t;
				}
      }
    }
}

template<class T>
void calcOM(T & offset, T & mult, T imin, T imax, T omin, T omax) {
  mult = (omax-omin)/(imax-imin);
  offset = omax - imax*mult;
}

/*!
	\class NVBValueScaler
  \brief Provides scaling for different data

  A scaler is an object that provides linear scaling for data.
  Both input and output types can be set to an arbitrary
  integer or floating-point number type.

  Scaling of the data is governed by two numbers: the offset and the multiplier
  in such a way that
		OUTPUT = offset + INPUT*multiplier;
 */

template<class fromT, class toT>
class NVBValueScaler {
private:
  double scale_offset;
  double scale_multiplier;

public:
/**
	\fn NVBValueScaler( fromT from_min, fromT from_max, toT to_min, toT to_max)
	Constructs a scaler that will map \a from_min to \a to_min and \a from_max to \a to_max linearly.
	*/
  NVBValueScaler( fromT from_min, fromT from_max, toT to_min, toT to_max);
/**
	\fn NVBValueScaler( double offset, double multiplier)
	Constructs a scaler with given \a offset and \a multiplier.
	*/
	NVBValueScaler( double offset, double multiplier)
		: scale_offset(offset)
		, scale_multiplier(multiplier)
		{;}
		
/**
	\fn 	NVBValueScaler()
	Constructs an identity scaler (INPUT == OUTPUT).
	*/
  NVBValueScaler()
		: scale_offset(0)
		, scale_multiplier(1)
		{;}
	
  ~NVBValueScaler() {;}

  // Input _was_ multiplied and shifted;
  void shift_input(double offset) { scale_offset += offset*scale_multiplier; }
  void multiply_input(double mult) { scale_multiplier *= mult; }

/**
	\fn void change_input(fromT omin, fromT omax, fromT nmin, fromT nmax)

	The scaler will change is such a way that \a nmin will map to the same value \a omin was before the call.
	\a nmax will map to the value that corresponded to \a omax.
	*/

  void change_input(fromT omin, fromT omax, fromT nmin, fromT nmax) {
		double em, eo;
		calcOM<double>(eo,em,nmin,nmax,omin,omax);
		shift_input(eo);
		multiply_input(em);
		}

/**
	\fn void shift_output(double offset)

	The scaler offset will be increased by \a offset. All output values will be larger by this amount.
	*/
  void shift_output(double offset) { scale_offset += offset; }

/**
	\fn void multiply_output(double mult)

	All output values will be larger by a factor of \a mult
	*/
  void multiply_output(double mult) {
		scale_offset *= mult;
		scale_multiplier *= mult;
		}
/**
	\fn void change_output(toT omin, toT omax, toT nmin, toT nmax)

	The scaler will change in such a way that the input value that mapped to \a omin will map to \a nmin.
	The value that mapped to \a omax will map to \a nmax instead.
	*/

  void change_output(toT omin, toT omax, toT nmin, toT nmax) {
		double em, eo;
		calcOM<double>(eo,em,omin,omax,nmin,nmax);
		multiply_output(em);
		shift_output(eo);
		}

  inline toT scale(fromT value) const { return (toT)(value*scale_multiplier + scale_offset); }
  inline toT scaleInt(fromT value) const { return (toT)round(value*scale_multiplier + scale_offset); }
  inline toT scaleLength(fromT value) const { return (toT)(value*scale_multiplier); }

	toT * scaleMem(const fromT * data, unsigned long size) {
		toT * dest = (toT*)malloc(size*sizeof(toT));
		if (dest)
			scaleMem(dest,data,size);
		return dest;
		}
		
	inline void scaleMem(toT * dest, const fromT * data, unsigned long size) {
		::scaleMem<fromT,toT>(dest, *this, data, size);
		}
};

template <class fromT, class toT>
NVBValueScaler<fromT,toT>::NVBValueScaler( fromT from_min, fromT from_max, toT to_min, toT to_max)
{
  if (from_max != from_min) {
    calcOM<double>(scale_offset,scale_multiplier,from_min, from_max, to_min, to_max);
    }
  else {
    scale_multiplier = 0;
    scale_offset = (to_max-to_min)/2.;
    }
}

#endif
