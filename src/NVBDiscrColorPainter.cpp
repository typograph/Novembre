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
#include "NVBDiscrColorPainter.h"

#include "../icons/dcoloring.xpm"

#include <QAction>
#include <QToolButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QColorDialog>
#include <QBrush>
#include <QGraphicsView>
#include <QSlider>


NVBSubstDiscrColorModel::NVBSubstDiscrColorModel(NVBDiscrColorModel * source): provider(0) {
	setModel(source);
	}

QColor NVBSubstDiscrColorModel::colorize(int index) const {
	if (index >= 0 && index < colors.size() && colors.at(index).isValid())
		return colors.at(index);
	else if (provider)
		return provider->colorize(index);
	else
		return Qt::black;
	}

void NVBSubstDiscrColorModel::setModel(const NVBDiscrColorModel * model) {
	if (provider != model) {
		if (provider) disconnect(provider, 0, this, 0);

		provider = model;
		connect(provider, SIGNAL(adjusted()), this, SIGNAL(adjusted()));

		if (provider) emit adjusted();
		}
	}

void NVBSubstDiscrColorModel::setColor(int index, QColor color, bool immediateemit) {
	if (index >= 0) {
		if (index >= colors.size())
			for (int i = colors.size(); i <= index; i ++)
				colors << QColor();

		colors.replace(index, color);

		if (immediateemit) emit adjusted();
		}
	}

void NVBSubstDiscrColorModel::setColor(QList< int > indexes, QColor color) {
	foreach(int index, indexes)
	setColor(index, color, false);
	emit adjusted();
	}

// -----------------

NVBDiscrColorPainter::NVBDiscrColorPainter(NVBSpecDataSource * source, NVBViewController * wnd): QWidget(), provider(source), wparent(wnd), painter(0), ccolor(Qt::green) {
	colors = new NVBSubstDiscrColorModel();
	page = new NVBDiscrColorPainterDelegate(source, colors);
	connect(provider, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
	connect(provider, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);

	QVBoxLayout * gen = new QVBoxLayout(this);

	QGridLayout * l = new QGridLayout();
	gen->addLayout(l);

// #if QT_VERSION >= 0x040300
//   l->setHorizontalSpacing(0);
//   l->setVerticalSpacing(0);
// #else
	l->setSpacing(0);
// #endif

	tools = new QActionGroup(this);
	QToolButton * tb;

	tb = new QToolButton();
	tb->setDefaultAction(tools->addAction(QIcon(_clr_brush), QString("Brush")));
	l->addWidget(tb, 0, 0);
	tb->defaultAction()->setCheckable(true);

	connect(tb, SIGNAL(triggered(QAction*)), SLOT(activateBrushPainter()));

	tb = new QToolButton();
	tb->setDefaultAction(tools->addAction(QIcon(_clr_rect), QString("Select color")));
	l->addWidget(tb, 1, 0);
	tb->defaultAction()->setCheckable(true);

	connect(tb, SIGNAL(triggered(QAction*)), SLOT(activateRectPainter()));

	l->addItem(new QSpacerItem(8, 1), 0, 0, 1, -1);

//   tb = new QToolButton();
//   tb->setDefaultAction(new QAction(QIcon(_clr_addcolor),QString("Select color"),0));
//   l->addWidget(tb);
//
//   connect(tb,SIGNAL(triggered(QAction*)),SLOT(getColor()));

	QActionGroup * colorActions = new QActionGroup(this);

	NVBColorButton * clrbtn;

	clrbtn = new NVBColorButton(Qt::white, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 0, 2);

	clrbtn = new NVBColorButton(Qt::black, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 1, 2);

	clrbtn = new NVBColorButton(Qt::red, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 0, 3);
//   l->addWidget(new NVBColorButton(Qt::darkRed,this),0,);

	clrbtn = new NVBColorButton(Qt::green, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 0, 4);
//   l->addWidget(new NVBColorButton(Qt::darkGreen,this),0,);

	clrbtn = new NVBColorButton(Qt::blue, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 0, 5);
//   l->addWidget(new NVBColorButton(Qt::darkBlue,this),0,);

	clrbtn = new NVBColorButton(Qt::cyan, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 1, 3);
//   l->addWidget(new NVBColorButton(Qt::darkCyan,this),0,);

	clrbtn = new NVBColorButton(Qt::magenta, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 1, 4);
//   l->addWidget(new NVBColorButton(Qt::darkMagenta,this),0,);

	clrbtn = new NVBColorButton(Qt::yellow, this);
	colorActions->addAction(clrbtn->defaultAction());
	connect(clrbtn, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	l->addWidget(clrbtn, 1, 5);
//   l->addWidget(new NVBColorButton(Qt::darkYellow,this),0,);
//   l->addWidget(new NVBColorButton(Qt::gray,this),0,);
//   l->addWidget(new NVBColorButton(Qt::darkGray,this),0,);
//   l->addWidget(new NVBColorButton(Qt::lightGray,this),0,);

	QGridLayout * lslider = new QGridLayout();
	gen->addLayout(lslider);

	minSliderColor = new NVBColorButton(Qt::green, this);
	minSliderColor->setColorSelectable(true);
	colorActions->addAction(minSliderColor->defaultAction());
	connect(minSliderColor, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	lslider->addWidget(minSliderColor, 0, 0);

	lslider->addItem(new QSpacerItem(16, 1), 0, 1);

	sliderColor = new NVBColorButton(Qt::green, this);
	colorActions->addAction(sliderColor->defaultAction());
	connect(sliderColor, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));

	lslider->addWidget(sliderColor, 0, 2);

	lslider->addItem(new QSpacerItem(16, 1), 0, 3);

	maxSliderColor = new NVBColorButton(Qt::black, this);
	maxSliderColor->setColorSelectable(true);
	colorActions->addAction(maxSliderColor->defaultAction());
	connect(maxSliderColor, SIGNAL(triggered(QColor)), SLOT(setColor(QColor)));
	lslider->addWidget(maxSliderColor, 0, 4);

	QSlider * clrslider = new QSlider(Qt::Horizontal, this);
	clrslider->setMinimum(0);
	clrslider->setMaximum(255);
	clrslider->setValue(0);
	connect(clrslider, SIGNAL(valueChanged(int)), SLOT(setSlidingColor(int)));
	lslider->addWidget(clrslider, 1, 0, 1, -1);

	setLayout(gen);

	setWindowTitle("Curve paintbrush");
	}

QAction * NVBDiscrColorPainter::action() {
	return new QAction(QIcon(_clr_pbrush), QString("Color adjust"), 0);
	}

void NVBDiscrColorPainter::getColor() {
	ccolor = QColorDialog::getColor(ccolor);
	QColor acolor = ccolor;
	acolor.setAlphaF(0.5);

	if (painter) painter->setBrush(QBrush(acolor));
	}

void NVBDiscrColorPainter::colorizePoints(QList< int > points) {
	if (!points.isEmpty())
		colors->setColor(points, ccolor);
	}

void NVBDiscrColorPainter::activateBrushPainter() {
	activatePainter(new NVBDiscrBrushPainterViz(provider));
	}

void NVBDiscrColorPainter::activateRectPainter() {
	activatePainter(new NVBDiscrRectPainterViz(provider));
	}

void NVBDiscrColorPainter::activatePainter(NVBDiscrPainterViz * viz) {
	QAction * a = tools->checkedAction();

	if (!a) return;

	deactivatePainting();
	a->setChecked(true);

	if (painter) {
		NVBOutputError("Visualizer deactivation failed");
		delete painter;
		}

	painter = viz;

	QColor acolor = ccolor;
	acolor.setAlphaF(ccolor.alphaF() / 2);
	painter->setBrush(QBrush(acolor));
	connect(painter, SIGNAL(pointsTouched(QList<int>)), SLOT(colorizePoints(QList<int>)));
//   connect(painter,SIGNAL(deactivated()),SLOT(deactivatePainting()));
	NVBVizUnion u(NVB::SpecPage, painter);
	u.filter = this;
	wparent->setActiveVisualizer(u);
	}

void NVBDiscrColorPainter::deactivatePainting() {
	// The 2DView will call deactivate2DView. I hope
	if (painter) {
		emit detach2DViz();
		delete painter;
		painter = 0;
		}
	}

void NVBDiscrColorPainter::setSource(NVBDataSource * source) {
	if (provider) provider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		}
	else {
		provider = (NVBSpecDataSource*)source;
		connect(provider, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
		connect(provider, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);
		}
	}

void NVBDiscrColorPainter::vizDeactivationRequest() {
	if (tools->checkedAction())
		tools->checkedAction()->setChecked(false);

	deactivatePainting();
	}

void NVBDiscrColorPainter::setColor(QColor new_color) {
	ccolor = new_color;
	QColor acolor = ccolor;
	acolor.setAlphaF(0.5);

	if (painter) painter->setBrush(QBrush(acolor));
	}

void NVBDiscrColorPainter::setSlidingColor(int pos) {
	sliderColor->setColor(QColor::fromRgb(minSliderColor->color().rgb() + (pos * (maxSliderColor->color().rgb() - minSliderColor->color().rgb())) / 255));
	}

NVBDiscrColorPainter::~ NVBDiscrColorPainter() {
	deactivatePainting();
	}

// ------------------

void NVBDiscrPainterViz::refresh() {
	int ncurv = sprovider->datasize().height();
	touched.fill(false, ncurv);

	points.setRect(sprovider->occupiedArea());

	int i = 0;

	foreach(QPointF pt, sprovider->positions()) {
		points.insert(pt, i);
		i += 1;
		}

	}

void NVBDiscrPainterViz::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	sprovider = (NVBSpecDataSource*)source;

	connect(source, SIGNAL(dataChanged()), SLOT(refresh()));

	connect(source, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
	connect(source, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);

	if (sprovider) {
		refresh();
		}

	}

// ----------

void NVBDiscrBrushPainterViz::refresh() {
	NVBDiscrPainterViz::refresh();

	prepareGeometryChange();

	rect = points.rect();

	QPointF pt = mouserect.center();
	mouserect.setSize(QSizeF(rect.size().width(), rect.size().width()) / 10);
	mouserect.moveCenter(pt);
//  setPos(rect.topLeft());

	rect.adjust(-mouserect.width() / 2, -mouserect.width() / 2, mouserect.width() / 2, mouserect.width() / 2);

	update();
	}

void NVBDiscrRectPainterViz::refresh() {
	NVBDiscrPainterViz::refresh();

	prepareGeometryChange();

	QRectF trect = points.rect();
	brect = QRectF(QPointF(), trect.size() * 1.2);
	brect.moveCenter(trect.center());

	update();
	}

NVBDiscrBrushPainterViz::NVBDiscrBrushPainterViz(NVBSpecDataSource * source): NVBDiscrPainterViz(source) {
	showmouse = false;
	brush = QBrush(Qt::transparent);
//  setFlag(QGraphicsItem::ItemIsFocusable);
	setCursor(Qt::BlankCursor);
	setSource(source);
	}

NVBDiscrBrushPainterViz::~ NVBDiscrBrushPainterViz() {
	}

void NVBDiscrBrushPainterViz::hoverMoveEvent(QGraphicsSceneHoverEvent * event) {
	showmouse = boundingRect().contains(event->pos());
	QRectF r = mouserect;
	mouserect.moveCenter(event->scenePos());
	update(r | mouserect);
	}

void NVBDiscrBrushPainterViz::wheelEvent(QGraphicsSceneWheelEvent * event) {
	Q_UNUSED(event);
	prepareGeometryChange();
	QRectF r = mouserect;
	double sh = mouserect.size().width() * event->delta() / 800.0;
	rect.adjust(-sh, -sh, sh, sh);
	mouserect.setSize(mouserect.size() * (1 + event->delta() / 400.0));
	mouserect.moveCenter(r.center());
	update(r | mouserect);
	}

void NVBDiscrBrushPainterViz::mousePressEvent(QGraphicsSceneMouseEvent * event) {
	Q_UNUSED(event);
	}

void NVBDiscrBrushPainterViz::mouseReleaseEvent(QGraphicsSceneMouseEvent * event) {
	mouseMoveEvent(event);
	}

void NVBDiscrBrushPainterViz::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
	showmouse = boundingRect().contains(event->scenePos());
	QRectF r = mouserect;
	mouserect.moveCenter(event->scenePos());
	update(r | mouserect);

	if (showmouse) {   //rect.contains(event->scenePos())) {

		QList<int> pl;

		foreach(NVBQuadTree::PointData pt, points.pointsInCircle(mouserect))
		foreach(QVariant v, pt.second) {
			int i = v.toInt();

			if (!touched.at(i)) {
				pl << i;
				touched[i] = true;
				}
			}

		if (!pl.isEmpty())
			emit pointsTouched(pl);
		}

	/*  else if (showmouse) {
	    showmouse = false;
	    update(mouserect);
	    }*/
	}

void NVBDiscrBrushPainterViz::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget *) {
	if (showmouse) {
		painter->save();

		painter->setPen(QPen(Qt::black));
		painter->setBrush(brush);

		painter->drawEllipse(mouserect);

		painter->restore();
		}
	}

void NVBDiscrBrushPainterViz::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
//   if (!showmouse)
	update(boundingRect());
	showmouse = true;
	mouserect.moveCenter(event->scenePos());
	update(mouserect);
	}

void NVBDiscrBrushPainterViz::hoverLeaveEvent(QGraphicsSceneHoverEvent * event) {
	Q_UNUSED(event);
	showmouse = false;
	update(mouserect);
	}

NVBDiscrRectPainterViz::NVBDiscrRectPainterViz(NVBSpecDataSource * source): NVBDiscrPainterViz(source) {
	rubberBand = 0;
	brush = QBrush(Qt::transparent);
// TODO use brush
	setCursor(Qt::CrossCursor);
	setSource(source);
//  setFlag(QGraphicsItem::ItemIsFocusable);
	}

NVBDiscrRectPainterViz::~ NVBDiscrRectPainterViz() {
	if (rubberBand) delete rubberBand;
	}

void NVBDiscrRectPainterViz::mousePressEvent(QGraphicsSceneMouseEvent * event) {
	rborigin = scene()->views().at(0)->mapFromScene(event->scenePos());
	scorigin = event->scenePos();

	if (!rubberBand)
		rubberBand = new QRubberBand(QRubberBand::Rectangle, scene()->views().at(0)->viewport());

	rubberBand->setGeometry(QRect(rborigin, QSize()));
	rubberBand->show();
	}

void NVBDiscrRectPainterViz::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
	rubberBand->setGeometry(QRect(rborigin, scene()->views().at(0)->mapFromScene(event->scenePos())).normalized());
	}

void NVBDiscrRectPainterViz::mouseReleaseEvent(QGraphicsSceneMouseEvent * event) {
	rubberBand->hide();
#if QT_VERSION >= 0x040300
	QRectF rect = QRectF(scorigin, event->scenePos()).normalized();
#else
	QRectF rect = QRectF(scorigin.x(), scorigin.y(), event->scenePos().x() - scorigin.x(), event->scenePos().y() - scorigin.y()).normalized();
#endif
	QList<int> pl;

	foreach(NVBQuadTree::PointData pt, points.pointsInRect(rect))
	foreach(QVariant v, pt.second) {
		int i = v.toInt();

		if (!touched.at(i)) {
			pl << i;
			touched[i] = true;
			}
		}

	if (!pl.isEmpty())
		emit pointsTouched(pl);
	}

void NVBDiscrRectPainterViz::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget *) {
	Q_UNUSED(painter);
	}

// ---------

NVBDiscrColorPainterDelegate::NVBDiscrColorPainterDelegate(NVBSpecDataSource * source, NVBSubstDiscrColorModel * model): NVBSpecFilterDelegate(source), sprovider(0), colors(model) {
	connectSignals();
	followSource();
	connect(colors, SIGNAL(adjusted()), SIGNAL(colorsAdjusted()));
	}

void NVBDiscrColorPainterDelegate::setSource(NVBDataSource * source) {
	if (sprovider) sprovider->disconnect(this);

	if (source->type() != NVB::SpecPage) {
		emit objectPopped(source, this); // going away
		return;
		}

	emit dataAboutToBeChanged();
	parentColorsAboutToBeChanged();

	NVBSpecFilterDelegate::setSource(source);
	}

void NVBDiscrColorPainterDelegate::connectSignals() {
	sprovider = (NVBSpecDataSource*)provider;

	connect(provider, SIGNAL(dataAboutToBeChanged()), SLOT(parentColorsAboutToBeChanged()));
	connect(provider, SIGNAL(dataAboutToBeChanged()), SIGNAL(dataAboutToBeChanged()));
	connect(provider, SIGNAL(dataAdjusted()), SIGNAL(dataAdjusted()));
	connect(provider, SIGNAL(dataChanged()), SIGNAL(dataChanged()));
	connect(provider, SIGNAL(dataChanged()), SLOT(parentColorsChanged()));
	connect(provider, SIGNAL(colorsAboutToBeChanged()), SLOT(parentColorsAboutToBeChanged()));
	connect(provider, SIGNAL(colorsChanged()), SLOT(parentColorsChanged()));

	parentColorsChanged();
	emit dataChanged();
	}

void NVBDiscrColorPainterDelegate::parentColorsAboutToBeChanged() {
	colors->setModel(0);
	}

void NVBDiscrColorPainterDelegate::parentColorsChanged() {
	colors->setModel(sprovider->getColorModel());
	}
