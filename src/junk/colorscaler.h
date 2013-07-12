//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
 *  colorscaler.h
 *  novembre
 *
 *  Created by Timofey Balashov on 8/5/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef COLORSCALER_H
#define COLORSCALER_H 1

#include "dimension.h"

template<class T>
class HSVscaler {
	private:
		scaler<T, float> *h, *s, *v;

		bool  hdir;
		float hmin, hmax;
		float smin, smax;
		float vmin, vmax;
		T nmin, nmax;

		void clearscalers();
		void createscalers();
		void setnMinMax(T, T);
		void sethMinMax(float, float, bool);
		void setsMinMax(float, float);
		void setvMinMax(float, float);

	public:
		HSVscaler();
		HSVscaler(unsigned int, unsigned int, bool, unsigned int, unsigned int, unsigned int, unsigned int, T, T);
		HSVscaler(float, float, bool, float, float, float, float, T, T);
		~HSVscaler() ;

		void setNMinMax(T, T);
		void setHMinMax(unsigned int, unsigned int, bool);
		void setHMinMax(float, float, bool);
		void setSMinMax(unsigned int, unsigned int);
		void setSMinMax(float, float);
		void setVMinMax(unsigned int, unsigned int);
		void setVMinMax(float, float);

		int32_t scale(T number);
	};

template<class T>
HSVscaler<T>::HSVscaler() {
	h = NULL;
	s = NULL;
	v = NULL;
	}

template<class T>
HSVscaler<T>::HSVscaler(unsigned int h1, unsigned int h2, bool fwd, unsigned int s1, unsigned int s2, unsigned int v1, unsigned int v2, T d1, T d2) {
	h = NULL;
	s = NULL;
	v = NULL;
	setnMinMax(d1, d2);
	sethMinMax(h1 / 360.0, h2 / 360.0, fwd);
	setsMinMax(s1 / 255.0, s2 / 255.0);
	setvMinMax(v1 / 255.0, v2 / 255.0);

	createscalers();
	}

template<class T>
HSVscaler<T>::HSVscaler(float h1, float h2, bool fwd, float s1, float s2, float v1, float v2, T d1, T d2) {
	h = NULL;
	s = NULL;
	v = NULL;
	setnMinMax(d1, d2);
	sethMinMax(h1, h2, fwd);
	setsMinMax(s1, s2);
	setvMinMax(v1, v2);

	createscalers();
	}

template<class T>
HSVscaler<T>::~HSVscaler() {
	clearscalers();
	}

template<class T>
void HSVscaler<T>::clearscalers() {
	if (h) { delete h; h = NULL;}

	if (s) { delete s; s = NULL;}

	if (v) { delete v; v = NULL;}
	}

template<class T>
void HSVscaler<T>::createscalers() {
	if (!h) {
		if ((hdir && hmin < hmax) || (!hdir && hmin > hmax))
			h = new scaler<T, float>(nmin, nmax, hmin, hmax);
		else
			h = new scaler<T, float>(nmin, nmax, hmax, hmin + 1);
		}

	if (!s)
		s = new scaler<T, float>(nmin, nmax, smin, smax);

	if (!v)
		v = new scaler<T, float>(nmin, nmax, vmin, vmax);
	}

template<class T>
void HSVscaler<T>::setNMinMax(T d1, T d2) {
	clearscalers();
	setnMinMax(d1, d2);
	createscalers();
	}

template<class T>
void HSVscaler<T>::setnMinMax(T n1, T n2) {
	nmin = n1;
	nmax = n2;
	}

template<class T>
void HSVscaler<T>::setHMinMax(unsigned int h1, unsigned int h2, bool dir) {
	setHMinMax((float)(h1 / 360.0), (float)(h2 / 360.0), dir);
	}

template<class T>
void HSVscaler<T>::setHMinMax(float h1, float h2, bool fwd) {
	if (h) { delete h; h = NULL;}

	sethMinMax(h1, h2, fwd);
	createscalers();
	}

template<class T>
void HSVscaler<T>::sethMinMax(float h1, float h2, bool fwd) {
	hmin = h1;
	hmax = h2;
	hdir = fwd;
	}

template<class T>
void HSVscaler<T>::setSMinMax(unsigned int s1, unsigned int s2) {
	setVMinMax((float)(s1 / 255.0), (float)(s2 / 255.0));
	}

template<class T>
void HSVscaler<T>::setSMinMax(float s1, float s2) {
	if (s) { delete s; s = NULL; }

	setsMinMax(s1, s2);
	createscalers();
	}

template<class T>
void HSVscaler<T>::setsMinMax(float s1, float s2) {
	smin = s1;
	smax = s2;
	}

template<class T>
void HSVscaler<T>::setVMinMax(unsigned int v1, unsigned int v2) {
	setVMinMax((float)(v1 / 255.0), (float)(v2 / 255.0));
	}

template<class T>
void HSVscaler<T>::setVMinMax(float v1, float v2) {
	if (v) { delete v; v = NULL; }

	setvMinMax(v1, v2);
	createscalers();
	}

template<class T>
void HSVscaler<T>::setvMinMax(float v1, float v2) {
	vmin = v1;
	vmax = v2;
	}

template<class T>
int32_t HSVscaler<T>::scale(T number) {
	if (h && s && v) {
		if (number > nmax) number = nmax;
		else if (number < nmin) number = nmin;

		float hf, sf, vf;

		sf = s->scale(number);
		vf = v->scale(number);

		if (sf <= 0.00196) {
			// achromatic (grey)
			return 0xFF000000 + 0x00FFFFFF * vf;
			}

		float f;
		unsigned char i, p, q, v, t;

		hf = 6 * h->scale(number);    // sector 0 to 5
//    printf("%g %g %g\n",hf,sf,vf);
		i = ((int)floor(hf)) % 6;
		f = hf - i;      // factorial part of h

		vf *= 0xFF;
		sf *= vf;
		p = vf - sf;  // v*(1-s)
		sf *= f;
		q = vf - sf;  // v*(1-f*s)
		t = p + sf;   // v*(1-s*(1-f))
		v = vf;

//  printf("%hhd %02hhX %02hhX %02hhX %02hhX\n",i,p,t,q,v);

		switch (i) {
			case 0:
				return 0xFF000000 + (v << 16) + (t << 8) + p;

			case 1:
				return 0xFF000000 + (q << 16) + (v << 8) + p;

			case 2:
				return 0xFF000000 + (p << 16) + (v << 8) + t;

			case 3:
				return 0xFF000000 + (p << 16) + (q << 8) + v;

			case 4:
				return 0xFF000000 + (t << 16) + (p << 8) + v;

			case 5:
				return 0xFF000000 + (v << 16) + (p << 8) + q;

			default: {
				NVBOutputError("HSVscaler<T>::color", "Color conversion error");
				NVBOutputError("HSVscaler<T>::color", "%d (%d,%d)\nwas converted to\n(%3g->%hhu) -- your computer is crazy!", number, nmin, nmax, hf, i);
				return 0;
				}
			}
		}
	else
		return 0;
	}

#endif
