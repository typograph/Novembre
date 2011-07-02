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
