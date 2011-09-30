#ifndef NVBMUTABLEGRADIENTS_H
#define NVBMUTABLEGRADIENTS_H

#include "NVBColorWidgets.h"
#include "NVBGradientMenu.h"

class NVBMutableGradientAction : public NVBGradientAction {
Q_OBJECT
//protected:
//	NVBColorSelectMenu * menu;
public:
	NVBMutableGradientAction(NVBColorMap * gradient, QObject * parent );
	virtual ~NVBMutableGradientAction() {;}
public slots:
	virtual void setColor(QColor color);
};

class NVBBlackToColorGradientAction : public NVBMutableGradientAction {
Q_OBJECT
public:
	NVBBlackToColorGradientAction(QObject * parent);
	virtual ~NVBBlackToColorGradientAction() {;}
public slots:
	virtual void setColor(QColor color);
};

class NVBGrayStepColorMap;

class NVBBlackToColorToWhiteGradientAction : public NVBMutableGradientAction {
Q_OBJECT
private:
	NVBGrayStepColorMap * red;
	NVBGrayStepColorMap * green;
	NVBGrayStepColorMap * blue;
public:
	NVBBlackToColorToWhiteGradientAction(QObject * parent);
	virtual ~NVBBlackToColorToWhiteGradientAction() {;}
public slots:
	virtual void setColor(QColor color);
};

#endif // NVBMUTABLEGRADIENTS_H
