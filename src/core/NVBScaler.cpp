//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
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

#include "NVBScaler.h"
#include <QtCore/qglobal.h>

template<>
void reverse_byte_order<1>(char *) {;}

template<>
void reverse_byte_order<2>(char * ptr) {
	asm(
	  "xchg %%ah, %%al"
	  : "=a"(*((quint16*)ptr))
	  : "0"(*((quint16*)ptr))
	);
	}

template<>
void reverse_byte_order<4>(char * ptr) {
	asm(
	  "bswap %%eax"
	  : "=a"(*((quint32*)ptr))
	  : "0"(*((quint32*)ptr))
	);
	}

template<>
void reverse_byte_order<8>(char * ptr) {
	// Here for some reason %%ebx is occupied
	// g++ complains about hot it can't find BREG
	asm(
	  "bswap %%eax;"
	  "bswap %%ecx;"
	  : "=a"(*((quint32*)ptr)), "=c"(*((quint32*)(ptr+4)))
	  : "0"(*((quint32*)(ptr+4))), "1"(*((quint32*)ptr))
	);
	}
