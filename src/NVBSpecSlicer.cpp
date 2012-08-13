//
// C++ Implementation: NVBSpecSlicer
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecSlicer.h"
#include "dimension.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>
#include <QSlider>
#include <qwt_symbol.h>

#include "../icons/slice.xpm"

//NVBSpecSlicerSpinBox::NVBSpecSlicerSpinBox(NVBSpecDataSource * source,  QWidget * parent):QSpinBox(parent),dim(source->tDim()),x0(0),xstep(0),nsteps(0)
//{
////  setReadOnly(true);
//  QRectF r = source->boundingRect();
//  x0 = r.left();
//  nsteps = source->datasize().width()-1;
//  xstep = r.width()/nsteps;
//	setRange(0,nsteps);
//}

//NVBSpecSlicerSpinBox::~ NVBSpecSlicerSpinBox()
//{
//}

//QString NVBSpecSlicerSpinBox::textFromValue(int value) const
//{
//  return NVBPhysValue(x0 + value*xstep,dim).toString();
//}

//int NVBSpecSlicerSpinBox::valueFromText(const QString & text) const
//{
//	NVBPhysValue v(text);
//	if (v.getDimension().isComparableWith(dim))
//		return int((v.getValue(dim)-x0)/xstep);
//  return 0;
//}

// -------------------

NVBSpecSlicerWidget::NVBSpecSlicerWidget(NVBSpecDataSource * source)
	: QWidget(), sprovider(0)
{
	QVBoxLayout * lv = new QVBoxLayout(this);

	valueBox = new NVBSpecSlicerLabel(source,this);
	lv->addWidget(valueBox);
	slider = new QSlider(Qt::Horizontal,this);
	slider->setRange(0,source->datasize().width()-1);
	slider->setValue(0);
	lv->addWidget(slider);
	connect(slider,SIGNAL(valueChanged(int)),SIGNAL(posChanged(int)));
	connect(slider,SIGNAL(valueChanged(int)),valueBox,SLOT(setPos(int)));

	setLayout(lv);

	resetParam(source);
}

void NVBSpecSlicerWidget::resetParam(NVBDataSource * page) {

	if (!page) page = qobject_cast<NVBDataSource*>(QObject::sender());
	NVBSpecDataSource * source = qobject_cast<NVBSpecDataSource*>(page);

	if (!source) {
		if (sprovider) sprovider->disconnect(this);
		return;
		}

	int pos = slider->value();

	valueBox->resetParam(source);
	slider->setRange(0,source->datasize().width()-1);

	if (pos > source->datasize().width()-1)
		slider->setValue(0);

	valueBox->setPos(slider->value());

	if (source != sprovider) {
		if (sprovider) sprovider->disconnect(this);
		sprovider = source;
		connect(source,SIGNAL(dataChanged()),this,SLOT(resetParam()));
		connect(source,SIGNAL(dataAdjusted()),this,SLOT(resetParam()));
		connect(source,SIGNAL(objectPopped(NVBDataSource*,NVBDataSource*)),this,SLOT(resetParam(NVBDataSource*)));
		connect(source,SIGNAL(objectPushed(NVBDataSource*,NVBDataSource*)),this,SLOT(resetParam(NVBDataSource*)));
		}
}

// -------------

NVBSpecSlicerDelegate::NVBSpecSlicerDelegate(NVBSpecDataSource * source):NVB3DFilterDelegate(source),slicepos(0)
{
	colors = new NVBColoring::NVBGrayRampContColorModel(0,1,0,1);
	scolors = new NVBColoring::NVBRescaleColorModel(colors);

	connectSignals();
	followSource();
}

NVBSpecSlicerDelegate::~NVBSpecSlicerDelegate()
{
 invalidateCache();
 if (scolors) delete scolors;
 if (colors) delete colors;
}

const NVBContColorModel *NVBSpecSlicerDelegate::getColorModel() const
{
	return scolors;
}

bool NVBSpecSlicerDelegate::setColorModel(NVBContColorModel * colorModel)
{
  if (colorModel) {
    colors = colorModel;
    emit colorsChanged();
    return true;
    }
  return false;
}

void NVBSpecSlicerDelegate::invalidateCache()
{
  foreach(double * data, cache) {
    if(data) free(data);
    }
  cache.fill(NULL,page->datasize().width());
	zlimits.fill(QPair<double,double>(),page->datasize().width());
	fillCache();

	emit dataChanged();
}

void NVBSpecSlicerDelegate::fillCache()
{
	if (!page) return;
	if (slicepos < 0 || slicepos >= cache.size()) slicepos = 0;
  if (cache.at(slicepos)) return;

	int nx = resolution().width();
	int ny = resolution().height();

	double * sdata = (double*)calloc(sizeof(double),nx*ny);
	if (!sdata) return;

	cache[slicepos] = sdata;

	double vmin = 1e+300;
	double vmax = -1e+300;

	for (int x=0; x < nx; x++)
		for (int y=0; y < ny; y++)	{
			double value = 0;
			for (int p=0; p < mpp; p++)
				value += page->getData().at(p + x*mpp + y*nx*mpp)->y(slicepos);
			if (vmin > value) vmin = value;
			if (vmax < value) vmax = value;
			sdata[x+y*nx] = value;
			}

	zlimits[slicepos] = QPair<double,double>(vmin,vmax);
}

void NVBSpecSlicerDelegate::initSize()
{
	Q_ASSERT(hasGrid(page,&mpp,&datasize.rwidth(),&datasize.rheight()));
	rect = QRectF(page->positions().first(),page->positions().last());

	double dx = rect.width()/(datasize.width()-1)/2;
	double dy = rect.height()/(datasize.height()-1)/2;

	rect.adjust(-dx,-dy,dx,dy);
}

QAction * NVBSpecSlicerDelegate::action() {
return new QAction(QIcon(_slice_icon),QString("Slice"),0);
}

bool NVBSpecSlicerDelegate::hasGrid(NVBSpecDataSource * source, int * np, int * nx, int * ny)
{
	// Assume there are at most 3 axes - x, y and measurement;

	if (!source) return false;

	int nxl,nyl,npl;

	if (!np) np = &npl;
	if (!nx) nx = &nxl;
	if (!ny) ny = &nyl;

	int ncurves = source->datasize().height();

	for(*np = 1; *np < ncurves && source->positions().at(*np).x() == source->positions().at(0).x(); (*np)++) {;}
	for(*nx = 1; (*nx)*(*np) < ncurves && source->positions().at((*nx)*(*np)).y() == source->positions().at(0).y(); (*nx)++) {;}
	for(*ny = 1; (*ny)*(*nx)*(*np) < ncurves && source->positions().at((*ny)*(*nx)*(*np)).x() == source->positions().at(0).x(); (*ny)++) {;}

	if ((*nx > 1 || *ny > 1) && (*ny)*(*nx)*(*np) == ncurves)
		return true;
	return false;
}

void NVBSpecSlicerDelegate::connectSignals()
{
	emit dataAboutToBeChanged();

//	bool oldvalue = false;
//	double pos = 0;
//	if (page) {
//		oldvalue = true;
//		pos = page->getData().first()->x(slicepos);
//		}

	page = (NVBSpecDataSource*)provider;

//	if (oldvalue) {
//		int npoints = page->datasize().width();
//		int ncurves = page->datasize().height();
//		if (ncurves == 0) slicepos = 0;
//		else {
//			QwtData * ndata = page->getData().first();
//			if (slicepos >= npoints || ndata->x(slicepos) != pos) {
//				if (ndata->x(npoints-1) < pos) slicepos = npoints-1;
//				else if  (ndata->x(0) > pos) slicepos = 0;
//				else {
//					int xpos = npoints/2;
//					int dxpos = npoints/4;
//					while (dxpos > 0) {
//						if (ndata->x(xpos) < pos) xpos += dxpos;
//						else if (ndata->x(xpos) > pos) xpos -= dxpos;
//						dxpos /= 2;
//						}
//					if (abs(data->x(xpos) - pos) > abs(data->x(xpos-1) - pos)) slicepos = xpos-1;
//					else if (abs(data->x(xpos) - pos) > abs(data->x(xpos+1) - pos)) slicepos = xpos+1;
//					else slicepos = xpos;
//					}
//				}
//			}
//		}
//	else
//		slicepos = 0;

	initSize();

	invalidateCache();
//	fillCache();
if (scolors) scolors->setLimits(getZMin(), getZMax());

	connect(provider,SIGNAL(dataAdjusted()),SLOT(invalidateCache()));
	connect(page,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged()));
	connect(page,SIGNAL(dataChanged()),SLOT(invalidateCache()));

	NVB3DFilterDelegate::connectSignals();

	emit dataChanged();

}

//  ------

NVBSpecSlicerPosTracker::NVBSpecSlicerPosTracker(NVBSpecDataSource * source):QObject(),QwtPlotMarker(),sprovider(0)
{
//	setSymbol(QwtSymbol::VLine);
	setLineStyle(QwtPlotMarker::VLine);
	setZ(10000);

	resetParam(source);
	setPos(0);
}

//void NVBSpecSlicerPosTracker::draw(QPainter * painter, const QwtScaleMap & xMap, const QwtScaleMap & yMap, const QRect & canvasRect) const
//{
//  Q_UNUSED(painter);
//  Q_UNUSED(xMap);
//  Q_UNUSED(yMap);
//	Q_UNUSED(canvasRect);
//	int xcanvas = xMap.transform(pos);
//	if (xcanvas < canvasRect.left() || xcanvas > canvasRect.right()) return;
//	painter->save();
//	painter->setPen(Qt::black);
//	painter->drawLine(xcanvas,canvasRect.top(),xcanvas,canvasRect.bottom());
//	painter->restore();
//}

void NVBSpecSlicerPosTracker::setPos(int value)
{
//	pos = x0 + xstep*value;
//	itemChanged();
	setXValue(x0 + xstep*value);
}

void NVBSpecSlicerPosTracker::resetParam(NVBDataSource *page)
{
	if (!page) page = qobject_cast<NVBDataSource*>(QObject::sender());
	NVBSpecDataSource * source = qobject_cast<NVBSpecDataSource*>(page);
	if (!source) {
		if (sprovider) sprovider->disconnect(this);
		return;
		}

	QRectF r = source->boundingRect();
	x0 = r.left();
	xstep = r.width()/(source->datasize().width()-1);

	if (source != sprovider) {
		if (sprovider) sprovider->disconnect(this);
		sprovider = source;
		connect(source,SIGNAL(dataChanged()),this,SLOT(resetParam()));
		connect(source,SIGNAL(dataAdjusted()),this,SLOT(resetParam()));
		connect(source,SIGNAL(objectPopped(NVBDataSource*,NVBDataSource*)),this,SLOT(resetParam(NVBDataSource*)));
		connect(source,SIGNAL(objectPushed(NVBDataSource*,NVBDataSource*)),this,SLOT(resetParam(NVBDataSource*)));
		}

}


NVBSpecSlicerLabel::NVBSpecSlicerLabel(NVBSpecDataSource *source, QWidget *parent)
	: QLabel(parent)
	, x0(0)
	, xstep(0)
{
	resetParam(source);
}

void NVBSpecSlicerLabel::setPos(int value) {
	setText(templt.arg(NVBPhysValue(x0 + value*xstep,dim).toString()));
}

void NVBSpecSlicerLabel::resetParam(NVBSpecDataSource * source) {
	if (!source) return;

	dim = source->tDim();

	QString xlbl = source->getComment("X axis label");

	if (xlbl.isEmpty())
		templt = "Slice at %1";
	else
		templt = xlbl + " %1";

	QRectF r = source->boundingRect();
	x0 = r.left();
	int nsteps = source->datasize().width()-1;
	xstep = r.width()/nsteps;
	if (text().isEmpty()) setPos(0);
}
