#include "NVBSingle2DView.h"
#include "NVBDataSource.h"
#include "NVBMap.h"
#include "NVBAxisMaps.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QResizeEvent>
#include <QtCore/QDebug>

class NVBAxisWidget : public QWidget {
	private:
		Qt::Orientation o;
//		axissize_t n;
//		NVBAxisPhysMap * amap;
		NVBUnits d;
		NVBPhysValue min, max;
	public:
		NVBAxisWidget(/*axissize_t npts,*/ QWidget * parent, Qt::Orientation orientation)
		: QWidget(parent)
		, o(orientation)
//		, n(npts)
		{
			if (orientation == Qt::Horizontal){
				setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
				}
			else {
				setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
				}
		}
		
		NVBPhysValue value(int t) {
			int sz = (o == Qt::Horizontal ? width() : height());
			return NVBPhysValue((max-min).getValue(d)*t/(sz-1) - min.getValue(d),d);
		}
		
		virtual QSize sizeHint() const {
			return QSize(10,10);
		}
		
		void setRange(NVBPhysValue start, NVBPhysValue end) {
			min = start;
			max = end;
			d = start.getDimension();
			}
			
		const NVBPhysValue & minimum() const { return min; }
		const NVBPhysValue & maximum() const { return max; }
};

NVBSingle2DView::NVBSingle2DView(NVBDataSet* data, QWidget * parent)
: QFrame(parent)
, plotData(0)
, colors(0)
{
	xAxis = new NVBAxisWidget(this,Qt::Horizontal);
	yAxis = new NVBAxisWidget(this,Qt::Vertical);
	
	QGridLayout * gl = new QGridLayout(this);
	setLayout(gl);
	
	gl->addWidget(yAxis,0,0);
	gl->addWidget(xAxis,1,1);
	
	setDataSet(data);
}

NVBSingle2DView::~NVBSingle2DView()
{
	if (colors)
		delete colors;
}

void NVBSingle2DView::dataSetDestoyed()
{
	NVBOutputError("Dataset destroyed during the lifecycle of the view");
	if (colors) {
		delete colors;
		colors = 0;
		}
	plotData = 0;
}

void NVBSingle2DView::paintEvent(QPaintEvent* event)
{
	QFrame::paintEvent(event);

	
	// FIXME the rendering of unequally spaced axes not implemented
	
	if (colors) {		
		QPainter p(this);
		QRect rw = event->rect();
		p.fillRect(rw,palette().background());
		p.setTransform(transform,true);
		p.drawPixmap(QPoint(0,0),cache);
/*
		const double * data = plotData->data();
		axissize_t xi,yi;
		for(int xw = qMax(rw.left(),xAxis->x()); xw <= rw.right(); xw++)
			for(int yw = qMin(yAxis->height(),rw.bottom()); yw >= rw.top(); yw--) {
				if (map(xw,yw,xi,yi)) {
					p.setPen(QColor(colors->colorize(data[xi + yi*nx])));
					p.drawPoint(xw,yw);
					}
				}
*/
		}
	else
		NVBOutputError("Cannot draw");

}

void NVBSingle2DView::setDataSet(NVBDataSet* data)
{
	map2D = 0;
	mapX = 0;
	mapY = 0;
	
	if (colors) {
		delete colors;
		colors = 0;
		}
	
	cache = QPixmap();
	xi = 0;
	yi = 1;

	if (plotData)
		plotData->disconnect(this);
		
	plotData = data;
	
	if (plotData) {
		colors = plotData->colorMap()->instantiate(plotData);
		colors->setImageAxes(0,1);
		
		/* NVBDataSet::dataChanged is connected to NVBColotInstace, and its slot
		 * has to be called before regenerating the cache
		 */
		connect(plotData, SIGNAL(dataChanged()), SLOT(regenerateImage()));
		connect(plotData, SIGNAL(dataReformed()), SLOT(parentDataReformed()));
		}
		
	regenerateImage();
}

void NVBSingle2DView::parentDataReformed()
{
	if (colors) {
		delete colors;
		colors = 0;
		}
	if (plotData) {
		colors = plotData->colorMap()->instantiate(plotData);
		colors->setImageAxes(0,1);
		}
//	slice.clear();
}


void NVBSingle2DView::regenerateImage()
{

	if (!plotData) {
		return;
		}

	if (plotData->nAxes() < 2) {
		NVBOutputError("Dataset has less than 2 axes");
		return;
		}
/*
	if (xi >= plotData->nAxes()) {
		xi = yi ? 0 : 1;
		slice.clear();
		}
	if (yi >= plotData->nAxes()) {
		yi = xi ? 0 : 1;
		slice.clear();
		}
*/
	NVBAxis ax = plotData->axisAt(xi);
	nx = ax.length();
	NVBAxis ay = plotData->axisAt(yi);
	ny = ay.length();
	
	NVBAxisPhysMap * mx, * my;
	bool simple = true;

	mx = ax.physMap();
	if (mx) {
		switch(mx->mapType()) {
			case NVBAxisMap::Linear : // affects axis only
				mapX = mx;
				mx = 0; // in case my is not simple
				xAxis->setRange(mapX->value(0),mapX->value(nx-1));
				break;
/*			case NVBAxisMap::Linear2D :
				transform = static_cast<NVBAxes2DGridMap*>(mx)->transformation();
				map2D = mx;
				return;
				break;
*/
			default:
				mapX = mx;
				simple = false;
				xAxis->setRange(mapX->value(0),mapX->value(nx-1));
				break;
			}
		}

	my = ay.physMap();
	if (my) {
		switch(my->mapType()) {
			case NVBAxisMap::Linear : // affects axis only
				mapY = my;
				yAxis->setRange(mapY->value(0),mapY->value(ny-1));
				my = 0; // in case mx was not simple
				break;
			case NVBAxisMap::Linear2D :
				NVBOutputError("Axis #2 has a 2D map, while axis #1 doesn't");
				return;
				break;
			default:
				mapY = my;
				yAxis->setRange(mapY->value(0),mapY->value(ny-1));
				simple = false;
				break;
			}
		}

	colors->setImageAxes(xi,yi);
//	if (slice.isEmpty())
	cache = colors->colorize();	
//	else
//		cache = colors->colorize(slice);	

	resizeEvent(0);
}

void NVBSingle2DView::resizeEvent(QResizeEvent* e)
{		
	if (map2D) {
		transform = map2D->transformation();
		return;
		}
	
	transform.reset();

	QSizeF s;
	
	// If the units are the same, we should honour the ratio
	if (mapX && mapY && mapX->units().isComparableWith(mapY->units())) {
		double xrange, yrange;
		xrange = (xAxis->maximum() - xAxis->minimum()).getValue(mapX->units());
		yrange = (yAxis->maximum() - yAxis->minimum()).getValue(mapX->units());
		s = QSizeF(xrange,yrange);
		}
	else { // we try to make the pixels square // FIXME should be a user setting
		s = size(); // cache.size();
		}

	s.scale(size(),Qt::KeepAspectRatio);
	transform.translate((width() - s.width())/2.0,(height() - s.height())/2.0);
	transform.scale(s.width()/cache.width(), s.height()/cache.height());		

	if (e) QFrame::resizeEvent(e); // Hopefully,, resizeEvent does delayedUpdate()

	update();
}

/*
void NVBSingle2DView::setSliceIndexes(QVector< axissize_t > indexes)
{
	slice = indexes;
	cache = colors->colorize(slice);
	update();
}
*/

void NVBSingle2DView::setXYAxes(axisindex_t x, axisindex_t y)
{
	xi = x;
	yi = y;
//	slice.clear();
	regenerateImage();
}


bool NVBSingle2DView::map(int x_widget, int y_widget, axissize_t& x_data, axissize_t& y_data)
{
	x_widget -= xAxis->x();
	
	double xd,yd;
	
	if (map2D) {
		NVBPhysPoint p(xAxis->value(x_widget),yAxis->value(y_widget));
		QTransform tr = map2D->transformation().inverted();
		QPointF pd = tr.map(map2D->origin().vectorTo(p));
		xd = round(pd.x());
		yd = round(pd.y());
		}
	else {
		if (mapX && mapX->mapType() != NVBAxisMap::Linear)
			xd = mapX->closestValueIndex(xAxis->value(x_widget));
		else
			xd = round(nx*(x_widget+0.5)/xAxis->width() - 0.5);
		
		if (mapY && mapY->mapType() != NVBAxisMap::Linear)
			yd = mapY->closestValueIndex(yAxis->value(y_widget));
		else
			yd = round(ny*(y_widget+0.5)*1.0/yAxis->height() - 0.5);
	}
	
	x_data = (axissize_t)xd;
	y_data = (axissize_t)yd;
	
	return (xd >= 0 && xd < nx && yd >= 0 && yd < ny);
}


/*
 * 	// TODO this code should be moved first to the dataset averaging,
	// and then to NVBSelectorInstance
	
	foreach(NVBAxisMapping mp, ax.maps()) {
		if (mp.axes.count() == 1) {
			mx = mp.map;
			break;
			}
		else if (mp.axes.count() == 2) {
			if (mp.axes.at(0) == ax.parentIndex() && mp.axes.at(1) == ay.parentIndex()) {
				my = mx = mp.map;
				break;
				}
			if (mp.axes.at(0) == ay.parentIndex() && mp.axes.at(1) == ax.parentIndex()) {
				my = mx = mp.map;
				break;
				}
			}
		}
	if (!my) {
		foreach(NVBAxisMapping mp, ay.maps()) {
			if (mp.axes.count() == 1) {
				my = mp.map;
				break;
				}
			}
		}
*/