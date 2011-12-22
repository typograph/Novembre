#include "NVBVisibleColorsMap.h"

/*

   Main code taken from  RGB VALUES FOR VISIBLE WAVELENGTHS by Dan Bruton (astro@tamu.edu)
   Version dated February 20, 1996.
   
   The original FORTRAN code can be found at 
   http://www.physics.sfasu.edu/astro/color.html
   
*/

QRgb NVBVisibleColorsMap::colorize(double z) const {
	if (z < 0.15)
		return (unsigned char)round(0xFF * (1 - z/0.15)) << 16 + 0xFF;
	if (z < 0.225)
		return (unsigned char)round(0xFF * (z/0.125 - 1.2)) << 8 + 0xFF;
	if (z < 0.325)
		return (unsigned char)round(0xFF * (6.5 - z/0.05)) + 0xFF00;
	if (z < 0.5)
		return (unsigned char)round(0xFF * (z/0.175 - 1.857143)) << 16 + 0xFF00;
	if (z < 0.6625)
		return (unsigned char)round(0xFF * (4.076923 - z/0.1625)) << 16 + 0xFF0000;
	if (z <= 1)
		return 0xFF0000;
	return 0;
}