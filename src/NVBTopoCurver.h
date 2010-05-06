//
// C++ Interface: NVBTopoCurver
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBTOPOCURVER_H
#define NVBTOPOCURVER_H

#include <QWidget>
#include "NVBFilterDelegate.h"
#include "NVBDiscrColoring.h"

class NVBTopoCurverWidget;

class NVBTopoCurver : public NVBSpecFilterDelegate {
Q_OBJECT
private:
  enum Mode { XT, YT };

  NVB3DDataSource * tprovider;
  QList<QPointF> spositions; 
  QList<QwtData*> sdata; 
  NVBRGBRampDiscrColorModel * scolors;

  Mode mode;

  friend class NVBTopoCurverWidget;

public :
  NVBTopoCurver(NVB3DDataSource* source);
  ~NVBTopoCurver();

  virtual inline double getZMin() const {return tprovider->getZMin();}
  virtual inline double getZMax() const {return tprovider->getZMax();}

  NVB_FORWARD_TOPODIM(tprovider);

  virtual NVBDimension tDim() const;

  virtual const NVBDiscrColorModel * getColorModel() const { return scolors; }

  virtual QList<QPointF> positions() const { return spositions; }

  virtual QList<QwtData*> getData() const { return sdata; }

  virtual QSize datasize() const;

  virtual inline QRectF occupiedArea() const { return tprovider->position(); }

  NVBTopoCurverWidget * widget();

public slots:
  virtual void setSource(NVBDataSource * source);
  void setMode(NVBTopoCurver::Mode mode);

private:
  void connectSignals();

protected slots:
  void generateCurves();
  void invalidateColors();
};

class NVBTopoCurverWidget : public QWidget {
Q_OBJECT
public :
  NVBTopoCurverWidget();
  virtual ~NVBTopoCurverWidget() {;}
private slots:
  void remapMode(int mode);
signals:
  void curvingModeActivated( NVBTopoCurver::Mode );
};

#endif
