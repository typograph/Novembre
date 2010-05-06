//
// C++ Implementation: NVBTopoCurver
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBTopoCurver.h"
#include "qwt_data.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

class NVBLinkedGraphData : public QwtData {
protected :
  int npts;
  double x0;
  double xmult;
  const double * ydata;

public:
  NVBLinkedGraphData(int n, double xoffset, double xmultiplier, const double * ys)
    :QwtData(),npts(n),x0(xoffset),xmult(xmultiplier),ydata(ys) {;}
  virtual QwtData * copy () const { return new NVBLinkedGraphData(npts,x0,xmult,ydata);}
  virtual size_t size () const { return npts; }
  virtual double x (size_t i) const { return x0 + i*xmult;}
  virtual double y (size_t i) const { return ydata[i];}
};

class NVBCrossLinkedGraphData : public NVBLinkedGraphData {
protected:
  int yleap;
public:
  NVBCrossLinkedGraphData(int n, int ycycle, double xoffset, double xmultiplier, const double * ys)
    :NVBLinkedGraphData(n,xoffset,xmultiplier,ys),yleap(ycycle) {;}
  virtual QwtData * copy () const { return new NVBCrossLinkedGraphData(npts,yleap,x0,xmult,ydata);}
  virtual double y (size_t i) const { return ydata[i*yleap];}
};

NVBTopoCurver::NVBTopoCurver(NVB3DDataSource * source):NVBSpecFilterDelegate(source),tprovider(source),scolors(0),mode(XT)
{
  connectSignals();
  invalidateColors();
}

void NVBTopoCurver::setSource(NVBDataSource * source)
{
  if (tprovider) tprovider->disconnect(this);

  if (source->type() != NVB::TopoPage) {
    emit objectPopped(source, this); // going away
    return;
    }

  NVBSpecFilterDelegate::setSource(source);

  tprovider = (NVB3DDataSource*)source;

  connectSignals();

}

void NVBTopoCurver::connectSignals()
{
  connect(tprovider,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged()));
  connect(tprovider,SIGNAL(dataAdjusted()),SIGNAL(dataAdjusted()));
  connect(tprovider,SIGNAL(dataChanged()),SLOT(generateCurves()));
  connect(tprovider,SIGNAL(colorsChanged()),SLOT(invalidateColors())); // Color adjustment is not important ### I doubt that

  generateCurves();
}

void NVBTopoCurver::generateCurves()
{
  emit dataAboutToBeChanged();

  while (!sdata.isEmpty())
    delete sdata.takeFirst();
  spositions.clear();

  const double * pdata = tprovider->getData();

  switch (mode) {
    case XT : {
      for (int i=0;i<tprovider->resolution().height();i++) {
        sdata << new NVBLinkedGraphData(
                  tprovider->resolution().width(),
                  tprovider->position().x(),
                  tprovider->position().width()/tprovider->resolution().width(),
                  pdata + i*tprovider->resolution().width()
                  );
        spositions << QPointF(tprovider->position().center().x(),tprovider->position().y() + i*tprovider->position().height()/tprovider->resolution().height());
        }
      if (scolors) scolors->setNCurves(tprovider->resolution().height());
      break;
      }
    case YT : {
      for (int i=0;i<tprovider->resolution().width();i++) {
        sdata << new NVBCrossLinkedGraphData(
                  tprovider->resolution().height(),
                  tprovider->resolution().width(),
                  tprovider->position().y(),
                  tprovider->position().height()/tprovider->resolution().height(),
                  pdata + i
                  );
        spositions << QPointF(tprovider->position().x() + i*tprovider->position().width()/tprovider->resolution().width(),tprovider->position().center().y());
        }
      if (scolors) scolors->setNCurves(tprovider->resolution().width());
      break;
      }
    }

  emit dataChanged();
}

void NVBTopoCurver::setMode(Mode newmode)
{
  if (mode == newmode) return;
  mode = newmode;
  generateCurves();
}

NVBDimension NVBTopoCurver::tDim() const
{
  switch (mode) {
    case XT : return tprovider->xDim();
    case YT : return tprovider->yDim();
    }
  return NVBDimension();
}

QSize NVBTopoCurver::datasize() const
{
  switch (mode) {
    case XT : return tprovider->resolution();
    case YT : return QSize(tprovider->resolution().height(),tprovider->resolution().width());
    }
  return QSize();
}

NVBTopoCurverWidget * NVBTopoCurver::widget()
{
  NVBTopoCurverWidget * w = new NVBTopoCurverWidget();
  connect(w,SIGNAL(curvingModeActivated(NVBTopoCurver::Mode)),SLOT(setMode(NVBTopoCurver::Mode)));
  return w;
}

NVBTopoCurverWidget::NVBTopoCurverWidget():QWidget()
{
  setWindowTitle("Slice direction");
  setLayout(new QVBoxLayout(this));
  QButtonGroup * mode = new QButtonGroup(this);
  QRadioButton * b = new QRadioButton("X",this);
  b->setChecked(true);
  mode->addButton(b,NVBTopoCurver::XT);
  layout()->addWidget(b);
  b = new QRadioButton("Y",this);
  mode->addButton(b,NVBTopoCurver::YT);
  layout()->addWidget(b);
  connect(mode,SIGNAL(buttonClicked(int)),SLOT(remapMode(int)));
//   layout()->addWidget(mode);
}

NVBTopoCurver::~ NVBTopoCurver()
{
  if (scolors) delete scolors;
  while (!sdata.isEmpty())
    delete sdata.takeFirst();
  spositions.clear();
}

void NVBTopoCurverWidget::remapMode(int mode)
{
  emit curvingModeActivated((NVBTopoCurver::Mode)mode);
}

void NVBTopoCurver::invalidateColors()
{
  if (tprovider) {
    emit colorsAboutToBeChanged();
    if (scolors) delete scolors;
    const NVBContColorModel * tc = tprovider->getColorModel();
    scolors = new NVBRGBRampDiscrColorModel(1,tc->colorize(tc->zMin()),tc->colorize(tc->zMax()));
    connect(scolors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));
    emit colorsChanged();
    }
}

