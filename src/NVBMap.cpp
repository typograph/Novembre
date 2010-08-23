#include "NVBMap.h"

QImage * NVBColorInstance::colorize(const double * zs, QSize d_wxh, QSize i_wxh = QSize()) const {
	if (!zs) return 0;

	if (!i_wxh.isValid()) i_wxh = d_wxh;

	QImage * result = new QImage(i_wxh,QImage::Format_ARGB32);
	if (!result) return 0;
	result->fill(0x00FFFFFF);

	if (i_wxh != d_wxh) {
		NVBValueScaler<int,int> w(0,i_wxh.width(),0,d_wxh.width());
		NVBValueScaler<int,int> h(0,i_wxh.height(),0,d_wxh.height());
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++) {
					result->setPixel(i,j,colorize(zs[w.scaleInt(i)+ d_wxh.width()*h.scaleInt(j)]));
				}
		}
	else {
		for (int i = 0; i<i_wxh.width(); i++)
			for (int j = 0; j<i_wxh.height(); j++) {
					result->setPixel(i,j,colorize(zs[i+i_wxh.width()*j]));
				}
		}

	return result;
}
