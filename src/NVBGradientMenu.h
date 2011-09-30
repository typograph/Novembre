#ifndef NVBGRADIENTMENU_H
#define NVBGRADIENTMENU_H

#include <QtGui/QMenu>
#include <QtCore/QString>
#include <QtCore/QSignalMapper>
#include <QtGui/QWidgetAction>

class NVBColorMap;

/**
	An action for a gradient menu. The action assumes ownership of the map.
	*/
class NVBGradientAction : public QWidgetAction {
Q_OBJECT
protected :
	NVBColorMap * colors;
protected slots:
	void emitGradient();
public:
	NVBGradientAction(NVBColorMap * gradient, QObject * parent);
	virtual ~NVBGradientAction();
	virtual QWidget *	createWidget ( QWidget * parent );
signals:
	void triggered(const NVBColorMap * gradient);
};

class NVBColorSelectMenu : public QMenu {
Q_OBJECT
	QSignalMapper colormapper;
	QAction * separator;
public:
	NVBColorSelectMenu( QWidget * parent = 0 );
	virtual ~NVBColorSelectMenu() {;}

	void addColor(QColor color, QString name = QString());

private slots:
	void colorClick(int c);
	void selectExtraColor();

signals:
	void colorSelected(QColor color);
};

/**
	A menu to select a gradient.
	*/
class NVBGradientMenu : public QMenu {
Q_OBJECT
private slots:
	void selectGradient(const NVBColorMap * gradient);

public:
	NVBGradientMenu( QWidget * parent = 0) : QMenu(parent) {;}
	virtual ~NVBGradientMenu() {;}

	NVBGradientAction * addGradientAction(NVBColorMap * gradient);

	void addGradientAction(NVBGradientAction * gradientAction);
};

#endif // NVBGRADIENTMENU_H
