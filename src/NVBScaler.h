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

template<class T>
void calcOM(T & offset, T & mult, T imin, T imax, T omin, T omax) {
  mult = (omax-omin)/(imax-imin);
  offset = omax - imax*mult;
}

/*!
	\class NVBScaler
  \brief Provides scaling for different data

  A scaler is an object that provides linear scaling for data.
  Both input and output types can be set to an arbitrary
  integer or floating-point number type.

  Scaling of the data is governed by two numbers: the offset and the multiplier
  in such a way that
		OUTPUT = offset + INPUT*multiplier;
 */

template<class fromT, class toT>
class NVBScaler {
private:
  double scale_offset;
  double scale_multiplier;

public:
/**
	\fn NVBScaler( fromT from_min, fromT from_max, toT to_min, toT to_max)
	Constructs a scaler that will map \a from_min to \a to_min and \a from_max to \a to_max linearly.
	*/
  NVBScaler(fromT,fromT,toT,toT);
/**
	\fn NVBScaler( double offset, double multiplier)
	Constructs a scaler with given \a offset and \a multiplier.
	*/
	NVBScaler( double offset, double multiplier)
		: scale_offset(offset)
		, scale_multiplier(multiplier)
		{;}
		
/**
	\fn 	NVBScaler()
	Constructs an identity scaler (INPUT == OUTPUT).
	*/
  NVBScaler()
		: scale_offset(0)
		, scale_multiplier(1)
		{;}
	
  ~NVBScaler() {;}

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

  toT scale(fromT value) const { return (toT)(value*scale_multiplier + scale_offset); }
  toT scaleInt(fromT value) const { return (toT)round(value*scale_multiplier + scale_offset); }
  toT scaleLength(fromT value) const { return (toT)(value*scale_multiplier); }
};

template <class fromT, class toT>
NVBScaler<fromT,toT>::NVBScaler( fromT from_min, fromT from_max, toT to_min, toT to_max)
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
