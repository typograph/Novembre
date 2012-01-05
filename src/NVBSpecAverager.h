//
// C++ Interface: NVBSpecAverager
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSPECAVERAGER_H
#define NVBSPECAVERAGER_H

#include "NVBFilterDelegate.h"
#include "NVBDiscrColoring.h"
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QWidget>

class NVBSpecAveragerWidget;

class NVBSpecAverager : public NVBSpecFilterDelegate {
Q_OBJECT
public:
  enum Mode { NoAverage = 0, AllAverage, PosAverage, ColorAverage, ClassAverage};

private:
  NVBSpecDataSource * sprovider;

  QList<QwtData*> fdata;
  QList<QPointF> fpositions;
  NVBIndexedDiscrColorModel * fcolors;
  Mode mode;

  bool isAveraging;
  QList< QList<int> > aindexes; 

  void clearFData() {
    while(!fdata.isEmpty())
      delete fdata.takeLast();
    }

private slots:

  void reset(bool resetmode = false);
  void averageAll();
  void averagePos();
  void averageColors();

  void resetMode();
  void recalculateAll();
  void recalculateData();
  void recalculateColors();

  void parentDataAdjusted();
  void parentDataAboutToBeChanged();
  void parentDataChanged();

  void parentColorsAdjusted();
  void parentColorsAboutToBeChanged();
  void parentColorsChanged();


public:

  NVBSpecAverager( NVBSpecDataSource* source );
  virtual ~NVBSpecAverager() { ; }

  virtual inline double getZMin() const {return boundingRect().bottom();}
  virtual inline double getZMax() const {return boundingRect().top();}

  NVB_FORWARD_SPECDIM(sprovider);

  virtual const NVBDiscrColorModel * getColorModel() const {
    if (isAveraging)
      return fcolors;
    else
      return sprovider->getColorModel();
    }

  virtual QList<QPointF> positions() const {
    if (isAveraging)
      return fpositions;
    else
      return sprovider->positions();
    }

  virtual QList<QwtData*> getData() const {
    if (isAveraging)
      return fdata;
    else
      return sprovider->getData();
    }

  virtual QSize datasize() const { 
    if (isAveraging)
      return QSize(sprovider->datasize().width(),fdata.size());
    else
      return sprovider->datasize();
    }

  virtual inline QRectF boundingRect() const {
    if (isAveraging)
      return NVBSpecDataSource::boundingRect();
    else
      return sprovider->boundingRect();
    }
  virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

  static QAction * action();
  QWidget * widget();

public slots:
  void setMode(NVBSpecAverager::Mode);
  virtual void setSource(NVBDataSource * source);

private :
  void connectSignals();
};

Q_DECLARE_METATYPE(NVBSpecAverager::Mode);

class NVBSpecAveragerWidget : public QWidget {
Q_OBJECT
private:
  QActionGroup * actionCnt;
private slots:
  void noAverageModeActivated() { emit averageModeActivated(NVBSpecAverager::NoAverage); }
  void allAverageModeActivated() { emit averageModeActivated(NVBSpecAverager::AllAverage); }
  void posAverageModeActivated() { emit averageModeActivated(NVBSpecAverager::PosAverage); }
  void colorAverageModeActivated() { emit averageModeActivated(NVBSpecAverager::ColorAverage); }
  void classAverageModeActivated() { emit averageModeActivated(NVBSpecAverager::ClassAverage); }
public:
  NVBSpecAveragerWidget( NVBSpecAverager::Mode mode, QWidget * parent = 0);
  virtual ~NVBSpecAveragerWidget();
signals:
  void averageModeActivated( NVBSpecAverager::Mode );
  void delegateReset();
};

#endif

