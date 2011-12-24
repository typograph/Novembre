#ifndef NVBSTANDARDICONS_H
#define NVBSTANDARDICONS_H

#include <QtGui/QIcon>

#define loadIcon(var,name) \
	var.addPixmap(name ## _16); \
	var.addPixmap(name ## _24); \
	var.addPixmap(name ## _32)

#define newIcon(var,name) \
	QIcon var; \
	loadIcon(var)

namespace NVBStandardIcon {
	enum IconType { Novembre, Browser, Open };
}

QIcon getStandardIcon(NVBStandardIcon::IconType icon);

#endif
