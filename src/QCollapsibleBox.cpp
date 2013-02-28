//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#include "QCollapsibleBox.h"
#include <QVBoxLayout>
#include <QStyle>
#include <QPainter>
//#include <>

static const char * const indicator_downarrow[] = {
	"9 9 2 1",
	"  c None",
	"# c #000000",
	"         ",
	" ####### ",
	"  #####  ",
	"  #####  ",
	"   ###   ",
	"   ###   ",
	"    #    ",
	"    #    ",
	"         "
	};

static const char * const indicator_rightarrow[] = {
	"9 9 2 1",
	"  c None",
	"# c #000000",
	"         ",
	" #       ",
	" ###     ",
	" #####   ",
	" ####### ",
	" #####   ",
	" ###     ",
	" #       ",
	"         "
	};

static const char * const indicator_plus[] = {
	"9 9 2 1",
	"  c None",
	"# c #000000",
	"#########",
	"#       #",
	"#   #   #",
	"#   #   #",
	"# ##### #",
	"#   #   #",
	"#   #   #",
	"#       #",
	"#########"
	};

static const char * const indicator_minus[] = {
	"9 9 2 1",
	"  c None",
	"# c #000000",
	"#########",
	"#       #",
	"#       #",
	"#       #",
	"# ##### #",
	"#       #",
	"#       #",
	"#       #",
	"#########"
	};

QCollapsibleBox::QCollapsibleBox(QString title, QWidget * parent):
	QWidget(parent), cwidget(0),
	marg_l(8), marg_r(8), marg_t(8), marg_b(8),
	t_text(title), t_alignment(Qt::AlignLeft), ind_at(LeftWithText), h_style(None),
	checked(false), flat(false), owner(true), expanded(0), collapsed(0) {
	setPlusMinusIndicators();
	setHeaderStyle(Flat);
	setLayout(new QVBoxLayout(this));
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//   connect(this,SIGNAL(clicked(bool)),SLOT(expand(bool)));
	}

QCollapsibleBox::~ QCollapsibleBox() {
	setCentralWidget(0);
	}

void QCollapsibleBox::setCentralWidget(QWidget * w) {
	if (cwidget) {
		if (owner) delete cwidget;
		else cwidget->setParent(0);
		}

	if ((cwidget = w)) {
		cwidget->setParent(this);
		cwidget->setGeometry(widgetRect());
		cwidget->setHidden(!checked);
		setSizePolicy(QSizePolicy::Expanding, cwidget->sizePolicy().verticalPolicy());
		}
	else
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	updateGeometry();
	update();
	}

void QCollapsibleBox::expand(bool do_expand) {
	if (!do_expand) collapse();
	else {
		if (checked) return;

		if (!flat) setFlat(false);

		checked = true;

		if (cwidget) cwidget->show();

		updateGeometry();
		update();
		}
	}

void QCollapsibleBox::collapse(bool do_collapse) {
	if (!do_collapse)expand();
	else {
		if (!checked) return;

		checked = false;

		if (cwidget) cwidget->hide();

		updateGeometry();
		update();
		}
	}

void QCollapsibleBox::paintEvent(QPaintEvent * event) {
	Q_UNUSED(event);
	QPainter p(this);
//   p.fillRect(rect(),palette().brush(QPalette::Background));

	QRect fullTitleRect = titleRect();
	QRect usableTitleRect = fullTitleRect.adjusted(imarg_l, imarg_t, -imarg_r, -imarg_b);
	int checkBoxSize = usableTitleRect.height();

	// Draw frame
	if (checked && !flat) {
		QStyleOptionFrameV2 frame;
		frame.lineWidth = 1;
		frame.midLineWidth = 0;
		frame.rect = centralRect();
		style()->drawPrimitive(QStyle::PE_FrameGroupBox, &frame, &p, this);
		}

	// Draw title
	int checkControlSize = (checkBoxSize + pixsize) / 2 + style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
	QRect textRect = QRect(usableTitleRect.x(), usableTitleRect.y(), fontMetrics().width(t_text + QString(QLatin1Char(' '))), fontMetrics().height());
	textRect.moveCenter(QPoint(textRect.center().x(), usableTitleRect.center().y()));
//   QRect checkBoxRect = isExpanded() ? collapsed->rect() : expanded->rect();
	QRect checkBoxRect = QRect(QPoint(), QSize(checkBoxSize, checkBoxSize));

	if (ind_at == Right || ind_at == RightWithText)
		checkBoxRect.moveCenter(QPoint(usableTitleRect.x() + usableTitleRect.width() - checkBoxSize / 2, usableTitleRect.center().y()));
	else
		checkBoxRect.moveCenter(QPoint(checkBoxSize / 2 + usableTitleRect.x(), usableTitleRect.center().y()));

	Qt::Alignment alignment_tmp = t_alignment;

	if (t_alignment == Qt::AlignHCenter && fullTitleRect.width() < textRect.width() + 2 * checkControlSize)
		alignment_tmp = (ind_at == Left || ind_at == LeftWithText) ? Qt::AlignLeft : Qt::AlignRight;

	switch (alignment_tmp) {
		case Qt::AlignLeft : {
			if (ind_at == Left || ind_at == LeftWithText) {
				textRect.moveLeft(checkControlSize + usableTitleRect.x());
				}
			else if (ind_at == RightWithText) {
				checkBoxRect.moveLeft(textRect.x() + textRect.width() + checkControlSize - checkBoxSize);
				}

			break;
			}

		case Qt::AlignRight : {
			if (ind_at == Right || ind_at == RightWithText)
				textRect.moveLeft(usableTitleRect.x() + usableTitleRect.width() - textRect.width() - checkControlSize);
			else
				textRect.moveLeft(usableTitleRect.x() + usableTitleRect.width() - textRect.width() - imarg_l - imarg_r);

			if (ind_at == LeftWithText) {
				checkBoxRect.moveLeft(textRect.left() - checkControlSize);
				}

			break;
			}

		case Qt::AlignHCenter: {
			textRect.moveCenter(usableTitleRect.center());

			if (ind_at == LeftWithText) {
				textRect.translate(checkControlSize / 2, 0);
				checkBoxRect.moveLeft(textRect.left() - checkControlSize);
				}
			else if (ind_at == RightWithText) {
				textRect.translate(-checkControlSize / 2, 0);
				checkBoxRect.moveLeft(textRect.right() + checkControlSize - checkBoxSize);
				}

			break;
			}

		default :
			break;
		}

	switch (h_style) {
		case None :
			break;

		case Flat : {
			if (!checked || flat) {
				QStyleOptionFrameV2 frame;
				frame.lineWidth = 1;
				frame.midLineWidth = 0;
				frame.rect = fullTitleRect.adjusted(1, (fullTitleRect.height() - 1) / 2, -1, -(fullTitleRect.height() - 1) / 2);
				style()->drawPrimitive(QStyle::PE_Frame, &frame, &p, this);
				}

//       p.fillRect(fullTitleRect,QBrush(Qt::green));
			if (ind_at == LeftWithText || ind_at == RightWithText) {
				p.fillRect(textRect.united(checkBoxRect), palette().brush(QPalette::Background));
				}
			else {
				p.fillRect(textRect, palette().brush(QPalette::Background));
				p.fillRect(checkBoxRect, palette().brush(QPalette::Background));
				}

			break;
			}

		case Horizontal : {
			QStyleOptionButton button;
//       button.features = QStyleOptionButton::None;
			button.state = checked ? QStyle::State_Sunken : QStyle::State_Raised;
			button.rect = fullTitleRect.adjusted(-100, 0, 100, 0);
			style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &button, &p, this);
			break;
			}

		case Button : {
			QStyleOptionButton button;
//       button.features = QStyleOptionButton::None;
			button.state = checked ? QStyle::State_Sunken : QStyle::State_Raised;
			button.rect = fullTitleRect;
			style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &button, &p, this);
			break;
			}
		}

	style()->drawItemText(&p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter, palette(), isEnabled(), t_text, QPalette::WindowText);

	if (expanded && collapsed) {
		QRect chbr = isExpanded() ? collapsed->rect() : expanded->rect();
		chbr.moveCenter(checkBoxRect.center());
		p.drawPixmap(chbr, isExpanded() ? *expanded : *collapsed);
		}

	p.end();
	}

void QCollapsibleBox::setFlat(bool newflat) {
	if (flat == newflat) return;

	flat = newflat;

//   if (cwidget) cwidget->move(QPoint(flat?pixsize/2:0,titleRect.height()));
	if (cwidget) cwidget->setGeometry(widgetRect());

	if (checked) update();
	}

void QCollapsibleBox::setHeaderStyle(HeaderStyle newstyle) {
	if (h_style != newstyle) {
		h_style = newstyle;

		switch (h_style) {
			case None :
				break;

			case Flat : {
				imarg_l = imarg_r = imarg_t = imarg_b = 0;
				marg_f = sqsize() / 2;
				break;
				}

			case Horizontal : { //QStyle::PM_ButtonMargin
				imarg_l = imarg_r = style()->pixelMetric(QStyle::PM_ButtonMargin) / 2;
				imarg_t = imarg_b = style()->pixelMetric(QStyle::PM_ButtonMargin);
				marg_f = imarg_b / 2;
				break;
				}

			case Button : {
				imarg_l = imarg_r = imarg_t = imarg_b = style()->pixelMetric(QStyle::PM_ButtonMargin);
				marg_f = imarg_b / 2;
				break;
				}
			}

		update();
		}
	}

void QCollapsibleBox::setIndicatorPosition(IndicatorPosition pos) {
	if (ind_at != pos) {
		ind_at = pos;
		update();
		}
	}

void QCollapsibleBox::setExpandedStatePixmap(QPixmap * pixmap) {
	if (expanded) delete expanded;

	expanded = pixmap;
	pixsize = qMax(expanded ? expanded->width() : 0, collapsed ? collapsed->width() : 0);
	updateGeometry();
	update();
	}

void QCollapsibleBox::setCollapsedStatePixmap(QPixmap * pixmap) {
	if (collapsed) delete collapsed;

	collapsed = pixmap;
	pixsize = qMax(expanded ? expanded->width() : 0, collapsed ? collapsed->width() : 0);
	updateGeometry();
	update();
	}

void QCollapsibleBox::setArrowIndicators() {
	setExpandedStatePixmap(new QPixmap(indicator_downarrow));
	setCollapsedStatePixmap(new QPixmap(indicator_rightarrow));
	}

void QCollapsibleBox::setPlusMinusIndicators() {
	setExpandedStatePixmap(new QPixmap(indicator_minus));
	setCollapsedStatePixmap(new QPixmap(indicator_plus));
	}

QRect QCollapsibleBox::titleRect() const {
	return QRect(QPoint(), QSize(width(), qMax(fontMetrics().height(), pixsize) + imarg_t + imarg_b));
	}

void QCollapsibleBox::setTitle(const QString & title) {
	if (t_text == title) return;

	t_text = title;
	updateGeometry();
	update();
	}

void QCollapsibleBox::setAlignment(Qt::Alignment alignment) {
	if (t_alignment == alignment) return;

	t_alignment = alignment;
	update();
	}

void QCollapsibleBox::mouseReleaseEvent(QMouseEvent * event) {
	if (event->button() != Qt::LeftButton) return;

	if (!titleRect().contains(event->pos())) return;

	collapse(checked);
	}

void QCollapsibleBox::resizeEvent(QResizeEvent * event) {
	Q_UNUSED(event);

	if (checked && cwidget) {
		QRect r = widgetRect();
		QSize m;

		if ((m = cwidget->maximumSize()).isValid()) {
			QPoint cnt = r.center();
			r.setSize(r.size().boundedTo(m));
			r.moveCenter(cnt);
			}

		cwidget->setGeometry(r);
		}
	}

int QCollapsibleBox::heightForWidth(int w) const {
	int height = titleRect().height();

	if (cwidget) {
		int h = cwidget->heightForWidth(w - marg_r - marg_l - imarg_l - imarg_r - flat ? 0 : height);

		if (h >= 0)
			height += h;
		else
			return -1;
		}

	return height;
	}

void QCollapsibleBox::setMargins(int ml, int mt, int mr, int mb) {
	marg_l = ml;
	marg_t = mt;
	marg_r = mr;
	marg_b = mb;

	if (cwidget) cwidget->setGeometry(widgetRect());

	updateGeometry();
	update();
	}

QRect QCollapsibleBox::widgetRect() const {
	return centralRect().adjusted(marg_l, marg_t, -marg_r, -marg_b);
	}

QRect QCollapsibleBox::centralRect() const {
	int th = sqsize();
	QRect r = QRect(QPoint(imarg_l, imarg_t + imarg_b + th - marg_f), QSize(width() - imarg_l - imarg_r, height() - th + marg_f - imarg_t - imarg_b));

	// margins
	if (!flat) {
//    r.adjust(,0,-th/2,-th/2);
		if (ind_at == Left || (ind_at == LeftWithText && t_alignment == Qt::AlignLeft))
			r.adjust(th / 2, 0, 0, 0);
		else if (ind_at == Right || (ind_at == RightWithText && t_alignment == Qt::AlignRight))
			r.adjust(0, 0, -th / 2, 0);
		}

	return r;
	}

int QCollapsibleBox::sqsize() const {
	return qMax(fontMetrics().height(), pixsize);
	}

QSize QCollapsibleBox::minimumSizeHint() const {
	return sizeHintFromWidgetSize(cwidget ? cwidget->minimumSizeHint() : QSize()).expandedTo(QWidget::minimumSizeHint());
	}

QSize QCollapsibleBox::sizeHint() const {
//  if (!cwidget || !checked) return QSize();
	return sizeHintFromWidgetSize(cwidget ? cwidget->sizeHint() : QSize()).expandedTo(QWidget::sizeHint());
	}

QSize QCollapsibleBox::sizeHintFromWidgetSize(QSize size, bool statesensitive) const {
	int baseWidth = fontMetrics().width(t_text + QString(QLatin1Char(' ')));
	int baseHeight = sqsize() + imarg_t + imarg_b;
	baseWidth += sqsize() + style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);

	if (!statesensitive || checked) {
		if (!flat) {
			size.setWidth(size.width() + sqsize());
//       size.setHeight(size.height());
			}

		baseWidth = qMax(baseWidth, size.width() + marg_l + marg_r);
		baseHeight += size.height() + marg_b + marg_t - marg_f;
		}

	baseWidth += imarg_l + imarg_r;

	return QSize(baseWidth, baseHeight).expandedTo(QWidget::minimumSizeHint());
	}


