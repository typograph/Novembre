#include "NVB3DVizDelegates.h"

class NVBProjectionColor3D : public Qwt3D::Color {
private:
  NVB3DDataSource * cprovider;

public:
  NVBProjectionColor3D(NVB3DDataSource * source):Qwt3D::Color(),cprovider(source) {;}
  virtual Qwt3D::RGBA   operator() (double x, double y, double z) const {
    Qwt3D::RGBA color;
    QColor(cprovider->getColorModel()->colorize(z)).getRgbF(&color.r,&color.g,&color.b,&color.a);
    return color;
    }
};

NVB3DSurfaceVizDelegate::NVB3DSurfaceVizDelegate(NVBDataSource * source):Qwt3D::SurfacePlot(),tttprovider(0)
{
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  setPlotStyle(Qwt3D::FILLED);
  setSource(source);

  enableMouse();
}

void NVB3DSurfaceVizDelegate::dataChanged()
{

//   double ** refactorData = (double**)malloc(tttprovider->resolution().width()*sizeof(double*));
// 
//   // I don't understand why this thing can't be constant
// 
//   double * tdata = const_cast<double*>();
// 
// // FIXME This code swaps x and y. To be redone in 0.1
// 
//   for (int i = tttprovider->resolution().height()-1; i>=0 ; i--) {
//     refactorData[i] = tdata + i*tttprovider->resolution().width();
//     }

  loadFromData( tttprovider->getData(),
                tttprovider->resolution().height(), tttprovider->resolution().width(),
                tttprovider->position().top(), tttprovider->position().bottom(),
                tttprovider->position().left(), tttprovider->position().right() );

  updateGL();
}

void NVB3DSurfaceVizDelegate::setSource(NVBDataSource * source)
{
  if (tttprovider)
    tttprovider->disconnect(this);

  if (source->type() != NVB::TopoPage) {
    tttprovider = 0;
    loadFromData(0,0,0,0,0,0,0);
    setDataColor(0);
    }
  else {
    tttprovider = (NVB3DDataSource*)source;
    setDataColor(new NVBProjectionColor3D(tttprovider));
  
    dataChanged();
  
    connect(tttprovider,SIGNAL(colorsChanged()),this,SLOT(colorsChanged()));
    connect(tttprovider,SIGNAL(colorsAdjusted()),this,SLOT(colorsChanged()));
    connect(tttprovider,SIGNAL(dataAdjusted()),this,SLOT(dataChanged()));
    connect(tttprovider,SIGNAL(dataChanged()),this,SLOT(dataChanged()));
    connect(tttprovider,SIGNAL(objectPushed(NVBDataSource *, NVBDataSource* )), this,SLOT(setSource(NVBDataSource*)));
    connect(tttprovider,SIGNAL(objectPopped(NVBDataSource *, NVBDataSource* )), this,SLOT(setSource(NVBDataSource*)),Qt::QueuedConnection);
    }
}