#include "qwt3d_surfaceplot.h"

using namespace std;
using namespace Qwt3D;

/**
Initializes with dataNormals()==false, NOFLOOR, resolution() == 1
*/
SurfacePlot::SurfacePlot( QWidget * parent, const QGLWidget * shareWidget)
	: Plot3D( parent, shareWidget) {

	resolution_p = 1;
	actualDataG_ = new GridData();
	actualDataC_ = new CellData();

	actualData_p = actualDataG_;

	floorstyle_ = NOFLOOR;
	}

SurfacePlot::~SurfacePlot() {
	delete actualDataG_;
	delete actualDataC_;
	}

/**
  Calculates the smallest x-y-z parallelepiped enclosing the data.
  It can be accessed by hull();
*/
void SurfacePlot::calculateHull() {
	if (actualData_p->empty())
		return;

	setHull(actualData_p->hull());
	}

/*!
  Sets data resolution (res == 1 original resolution) and updates widget
  If res < 1, the function does nothing
*/
void SurfacePlot::setResolution( int res ) {
	if (!actualData_p || actualData_p->datatype == Qwt3D::POLYGON)
		return;

	if ((resolution_p == res) || res < 1)
		return;

	resolution_p = res;
	updateData();

	if (initializedGL())
		updateGL();

	emit resolutionChanged(res);
	}

void SurfacePlot::createData() {
	if (!actualData_p)
		return;

	if (actualData_p->datatype == Qwt3D::POLYGON)
		createDataC();
	else if (actualData_p->datatype == Qwt3D::GRID)
		createDataG();
	}


void SurfacePlot::createFloorData() {
	if (!actualData_p)
		return;

	if (actualData_p->datatype == Qwt3D::POLYGON)
		createFloorDataC();
	else if (actualData_p->datatype == Qwt3D::GRID)
		createFloorDataG();
	}

/**
  The returned value is not affected by resolution(). The pair gives (columns,rows) for grid data
, (number of cells,1) for free formed data (datatype() == POLYGON) and (0,0) else
*/
pair<int, int> SurfacePlot::facets() const {
	if (!hasData())
		return pair<int, int>(0, 0);

	if (actualData_p->datatype == Qwt3D::POLYGON)
		return pair<int, int>(int(actualDataC_->cells.size()), 1);
	else if (actualData_p->datatype == Qwt3D::GRID)
		return pair<int, int>(actualDataG_->columns(), actualDataG_->rows());
	else
		return pair<int, int>(0, 0);
	}

#define LOWRES_LEN 32
#include <math.h>

int SurfacePlot::calcLowResolution() {
	float datasize = 0;

	if (actualData_p->datatype == Qwt3D::POLYGON)
		datasize = sqrt(actualDataC_->cells.size());
	else if (actualData_p->datatype == Qwt3D::GRID)
		datasize = qMax(actualDataG_->rows(), actualDataG_->columns());

	return (int)(log(datasize / LOWRES_LEN) / log(2));
	}

/**
  Overloaded mouse button Function. Prepares the call to mouseMoveEvent,
  degrading the plot
  \see mouseMoveEvent()
*/
void SurfacePlot::mousePressEvent( QMouseEvent *e ) {
	Plot3D::mousePressEvent(e);
	setResolution(calcLowResolution());
	}

/**
  Standard mouse button Function. Completes the call to mouseMoveEvent
  \see mouseMoveEvent()
*/
void SurfacePlot::mouseReleaseEvent( QMouseEvent * e) {
	Plot3D::mouseReleaseEvent(e);
	setResolution(1);
	}
