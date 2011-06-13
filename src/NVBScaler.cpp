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
	asm(
		"bswap %%eax;"
		"bswap %%ebx;"
		: "=a"(*((quint32*)ptr)), "=b"(*((quint32*)(ptr+4)))
		: "0"(*((quint32*)(ptr+4))), "1"(*((quint32*)ptr))
		);
}
