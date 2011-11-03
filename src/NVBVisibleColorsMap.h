#ifndef NVB_VISIBLECOLORSMAP_H
#define NVB_VISIBLECOLORSMAP_H

#include "NVBColorMap.h"

class NVBVisibleColorsMap : public NVBColorMap {
	public:
		NVBVisibleColorsMap() {;}
		virtual ~NVBVisibleColorsMap() {;}
		virtual QRgb colorize(double z) const;
};

#endif
