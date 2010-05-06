#ifndef NVB3DVIZDELEGATES_H
#define NVB3DVIZDELEGATES_H

#include "NVBDataSource.h"
#include <qwt3d_surfaceplot.h>

class NVB3DSurfaceVizDelegate : public Qwt3D::SurfacePlot {
Q_OBJECT

private:
  NVB3DDataSource * tttprovider;

private slots:
  void colorsChanged() { updateData(); updateGL(); }
  void dataChanged();
  void setSource(NVBDataSource * source);

public:
  NVB3DSurfaceVizDelegate(NVBDataSource * source);
  virtual ~NVB3DSurfaceVizDelegate() {;}

};

#endif
