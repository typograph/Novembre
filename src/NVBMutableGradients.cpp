#include "NVBMutableGradients.h"
#include "NVBColorMaps.h"

NVBMutableGradientAction::NVBMutableGradientAction(NVBColorMap *gradient, QObject *parent)
: NVBGradientAction(gradient,parent)
{
	colorMenu = new NVBColorSelectMenu();
	setMenu(colorMenu);
	connect(colorMenu,SIGNAL(colorSelected(QColor)),this,SLOT(setColor(QColor)));
}

void NVBMutableGradientAction::setColor(QColor) {
	emitGradient();
	}

NVBBlackToColorGradientAction::NVBBlackToColorGradientAction(QObject *parent)
	: NVBMutableGradientAction(new NVBRGBRampColorMap(0xFF000000,0xFFFFFFFF),parent)
	, pcolors(0)
{
	pcolors = dynamic_cast<NVBRGBRampColorMap*>(colors);
	if (!pcolors) {
		NVBOutputError("NVBRGBRampColorMap doesn't want to map back");
		pcolors = new NVBRGBRampColorMap(0xFF000000,0xFFFFFFFF);
		}
}

void NVBBlackToColorGradientAction::setColor(QColor color) {
	if (pcolors)
		pcolors->setEnd(color.rgba());
	NVBMutableGradientAction::setColor(color);
}

NVBBlackToColorToWhiteGradientAction::NVBBlackToColorToWhiteGradientAction(QObject *parent)
	: NVBMutableGradientAction(
			new NVBRGBMixColorMap(
				new NVBGrayStepColorMap(0,1),
				new NVBGrayStepColorMap(0,1),
				new NVBGrayStepColorMap(0,1)
				),
			parent
			)
	, red(0)
	, green(0)
	, blue(0)
{
	NVBRGBMixColorMap * m = dynamic_cast<NVBRGBMixColorMap*>(colors);
	if (!m) {
		NVBOutputError("NVBRGBMixColorMap doesn't want to map back");
		m = new NVBRGBMixColorMap(
					red = new NVBGrayStepColorMap(0,1),
					green = new NVBGrayStepColorMap(0,1),
					blue = new NVBGrayStepColorMap(0,1)
					);
		}
	else {
		red = dynamic_cast<NVBGrayStepColorMap*>(m->red());
		green = dynamic_cast<NVBGrayStepColorMap*>(m->green());
		blue = dynamic_cast<NVBGrayStepColorMap*>(m->blue());
		}
}

void NVBBlackToColorToWhiteGradientAction::setColor(QColor color) {
	if (red) red->addStep(0.5,color.redF());
	if (green) green->addStep(0.5,color.greenF());
	if (blue) blue->addStep(0.5,color.blueF());
	NVBMutableGradientAction::setColor(color);
}
