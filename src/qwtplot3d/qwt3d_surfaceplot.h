#ifndef QWT3D_SURFACEPLOT_H
#define QWT3D_SURFACEPLOT_H

#include "qwt3d_plot.h"

namespace Qwt3D
{
//! A class representing  Surfaces
/**
  A SurfacePlot ...

*/
class QWT3D_EXPORT SurfacePlot : public Plot3D {
Q_OBJECT

public:
  SurfacePlot( QWidget * parent = 0, const QGLWidget * shareWidget = 0 );
  ~SurfacePlot();
  int	resolution() const {return resolution_p;} //!< Returns data resolution (1 means all data)
  std::pair<int,int> facets() const; //!< Returns the number of mesh cells for the ORIGINAL data
  bool loadFromData(Qwt3D::Triple** data, unsigned int columns, unsigned int rows, bool uperiodic = false, bool vperiodic = false);
//   bool loadFromData(const double** data, unsigned int columns, unsigned int rows, double minx, double maxx, double miny, double maxy);
  bool loadFromData(const double* data, unsigned int columns, unsigned int rows, double minx, double maxx, double miny, double maxy);
  bool loadFromData(Qwt3D::TripleField const& data, Qwt3D::CellField const& poly);

  Qwt3D::FLOORSTYLE floorStyle() const { return floorstyle_;} //!< Return floor style
  void	setFloorStyle( Qwt3D::FLOORSTYLE val ) {floorstyle_ = val;} //!< Sets floor style

signals:
  void resolutionChanged(int);

public slots:
  void setResolution( int );

protected:

  virtual	void calculateHull();
  virtual void createData();
  virtual void createFloorData();

  int resolution_p;

  void readIn(Qwt3D::GridData& gdata, Triple** data, unsigned int columns, unsigned int rows);
  void readIn(Qwt3D::GridData& gdata, const double* data, unsigned int columns, unsigned int rows,
              double minx, double maxx, double miny, double maxy);
  void calcNormals(GridData& gdata);
  void sewPeriodic(GridData& gdata);

  int calcLowResolution();

  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );

private:

  void Data2Floor();
  void Isolines2Floor();

  Qwt3D::FLOORSTYLE floorstyle_;

  // grid plot

  Qwt3D::GridData* actualDataG_;
  virtual void createDataG();
  virtual void createFloorDataG();
  void Data2FloorG();
  void Isolines2FloorG();
  void setColorFromVertexG(int ix, int iy, bool skip = false);


  // mesh plot

  Qwt3D::CellData* actualDataC_;
  virtual void createDataC();
  virtual void createFloorDataC();
  void Data2FloorC();
  void Isolines2FloorC();
  void setColorFromVertexC(int node, bool skip = false);
};

} // ns


#endif
