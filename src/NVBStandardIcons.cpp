#include "NVBStandardIcons.h"
#include <QtGui/QIcon>

#include "../icons/novembre_v1.xpm"
#include "../icons/mainwindow.xpm"


class NVBStandardIconFactory {
	QIcon Novembre_icon, Browser_icon, Open_icon;

public:
	NVBStandardIconFactory()
	: Novembre_icon(_main_novembre)
	{
		loadIcon(Browser_icon,_main_browse);
		loadIcon(Open_icon,_main_open);
	}
	
	QIcon getIcon(NVBStandardIcon::IconType icon) {
		switch(icon) {
			case NVBStandardIcon::Novembre:
				return Novembre_icon;
			case NVBStandardIcon::Browser:
				return Browser_icon;
			case NVBStandardIcon::Open:
				return Open_icon;
			default:
				return QIcon();
			}
		}
};

QIcon getStandardIcon(NVBStandardIcon::IconType icon)
{
	static NVBStandardIconFactory factory;
	return factory.getIcon(icon);
}
