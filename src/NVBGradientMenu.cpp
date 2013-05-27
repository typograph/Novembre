//
// Copyright 2011 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBGradientMenu.h"
#include "NVBMap.h"
#include <QtGui/QColorDialog>
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionMenuItem>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include "NVBColorWidgets.h"

class NVBGradientMenuWidget : public NVBGradientWidget {
// Q_OBJECT
	private:
		QAction * action;
		bool mouseInWidget;
		bool mouseDown;
	public:
		NVBGradientMenuWidget(QAction * a, NVBColorMap * gradient, QWidget * parent)
			: NVBGradientWidget(gradient, parent)
			, action(a)
			, mouseInWidget(false)
			, mouseDown(false) {
			setMargin(4);
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			setMinimumSize(10, style()->pixelMetric(QStyle::PM_TitleBarHeight));
//		setMouseTracking(true);
			}

		virtual ~NVBGradientMenuWidget() {;}

		QSize sizeHint() const {
			return QSize(10, style()->pixelMetric(QStyle::PM_TitleBarHeight));
			}
	protected:
		virtual void paintEvent(QPaintEvent *e) {
			QStyleOptionMenuItem option;
			option.initFrom(this);
			option.palette = palette();
			option.state = QStyle::State_None;

			if (window()->isActiveWindow())
				option.state |= QStyle::State_Active;

			if (isEnabled() && action->isEnabled()
			    && (!action->menu() || action->menu()->isEnabled()))
				option.state |= QStyle::State_Enabled;
			else
				option.palette.setCurrentColorGroup(QPalette::Disabled);

			option.font = action->font().resolve(font());
			option.fontMetrics = QFontMetrics(option.font);

			if (mouseInWidget) {
				option.state |= QStyle::State_Selected | (mouseDown ? QStyle::State_Sunken : QStyle::State_None);
				}

			option.menuHasCheckableItems = false;
			option.checkType = QStyleOptionMenuItem::NotCheckable;

			if (action->menu())
				option.menuItemType = QStyleOptionMenuItem::SubMenu;
			else
				option.menuItemType = QStyleOptionMenuItem::Normal;

			option.text = QString();
			option.tabWidth = 1;
			option.maxIconWidth = 0;
			option.menuRect = qobject_cast<QWidget*>(parent())->rect();
			option.rect = rect();

			QPainter p(this);

			// paint selection/background as in the style
			style()->drawControl(QStyle::CE_MenuItem, &option, &p, this);

			// paint the gradient
			paint(&p, e->rect());

			// If has a menu - paint the arrow
			// The arrow was painted by drawControl before, but now it's behind the gradient.
			if (option.menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
				option.rect = QRect(width() - margin(), option.rect.top() + margin(), margin(), height() - 2 * margin());
//			QStyle::PrimitiveElement arrow;
//			arrow = (option.direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
				style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &option, &p, this);
				}
			}

		virtual void enterEvent(QEvent * e) {
			e->accept();
			mouseInWidget = true;
//		action->hover();
			update();
			}
		virtual void leaveEvent(QEvent * e) {
			e->accept();
			mouseInWidget = false;
			mouseDown = false;
			update();
			}
		virtual void mousePressEvent(QMouseEvent * e) {
			e->accept();
			mouseDown = true;

			if (e->button() == Qt::RightButton && action->menu())
				action->menu()->popup(e->globalPos());
			}
		virtual void mouseReleaseEvent(QMouseEvent * e) {
			e->accept();
			mouseDown = false;

			if (e->button() == Qt::LeftButton)
				action->trigger();
			}

	};


NVBGradientAction::NVBGradientAction(NVBColorMap *gradient, QObject *parent)
	: QWidgetAction(parent)
	, colors(gradient) {
	connect(this, SIGNAL(triggered()), this, SLOT(emitGradient()));
	}

NVBGradientAction::~NVBGradientAction() { if (colors) delete colors; }

QWidget * NVBGradientAction::createWidget(QWidget *parent) {
	NVBGradientMenuWidget* w = new NVBGradientMenuWidget(this, colors, parent);
	return w;
	}

void NVBGradientAction::emitGradient() {
	emit triggered(colors);
	}

NVBColorSelectMenu::NVBColorSelectMenu(QWidget *parent)
	: QMenu(parent) {
	connect(&colormapper, SIGNAL(mapped(int)), this, SLOT(colorClick(int)));
	addColor(QColor(Qt::red), "Red");
	addColor(QColor(Qt::green), "Green");
	addColor(QColor(Qt::blue), "Blue");
	addColor(QColor(Qt::white), "White");
	addColor(QColor(Qt::lightGray), "Light gray");
	addColor(QColor(Qt::gray), "Gray");
	addColor(QColor(Qt::darkGray), "Dark gray");
	addColor(QColor(Qt::black), "Black");
	addColor(QColor(Qt::cyan), "Cyan");
	addColor(QColor(Qt::magenta), "Magenta");
	addColor(QColor(Qt::yellow), "Yellow");
	addColor(QColor(Qt::darkRed), "Dark red");
	addColor(QColor(Qt::darkGreen), "Dark green");
	addColor(QColor(Qt::darkBlue), "Dark blue");
	addColor(QColor(Qt::darkCyan), "Dark cyan");
	addColor(QColor(Qt::darkMagenta), "Dark magenta");
	addColor(QColor(Qt::darkYellow), "Dark yellow");
	separator = addSeparator();
	connect(addAction("Other..."), SIGNAL(triggered()), this, SLOT(selectExtraColor()));
	}

void NVBColorSelectMenu::addColor(QColor color, QString name) {
	QAction * action = addAction(QIcon(new NVBSolidIconEngine(color)), name, &colormapper, SLOT(map()));
	colormapper.setMapping(action, color.rgba());
	}

void NVBColorSelectMenu::colorClick(int c) {
	emit colorSelected(QColor((QRgb)c));
	}

void NVBColorSelectMenu::selectExtraColor() {
	emit colorSelected(QColorDialog::getColor());
	}

NVBGradientAction * NVBGradientMenu::addGradientAction(NVBColorMap * gradient) {
	NVBGradientAction * action = new NVBGradientAction(gradient, this);
	addAction(action);
	connect(action, SIGNAL(triggered(const NVBColorMap*)), SLOT(selectGradient(const NVBColorMap*)));
	return action;
	}

void NVBGradientMenu::addGradientAction(NVBGradientAction * gradientAction) {
	QMenu::addAction(gradientAction);
	connect(gradientAction, SIGNAL(triggered(const NVBColorMap*)), SLOT(selectGradient(const NVBColorMap*)));
	}

void NVBGradientMenu::selectGradient(const NVBColorMap *gradient) {
	hide();
	qApp->setProperty("DefaultGradient", QVariant::fromValue(gradient));

	// Technically, this might be a memory leak.
	// On the other hand, there's only one original colormap,
	// and the rest can only be set through this menu.

	// The colormaps are deleted together with actions,
	// i.e. when the browser gets destroyed. This is after
	// the models using these colormaps are deleted.

	}
