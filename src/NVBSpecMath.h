//
// C++ Interface: NVBSpecMath
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSPECMATH_H
#define NVBSPECMATH_H

#include "NVBFilterDelegate.h"
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QWidget>

class QButtonGroup;
class NVBSpecMathWidget;

class NVBSpecMath : public NVBSpecFilterDelegate {
Q_OBJECT
public:
  enum Mode { Normal = 0, FirstDerivative, NormalizedFirstDerivative };

private:
  NVBSpecDataSource * sprovider;

  QList<QwtData*> fdata;
  Mode mode;

private slots:

//   void reset(bool resetmode = false);
//   void averageAll();
//   void averagePos();
//   void averageColors();
// 
//   void resetMode();
  void clearFData() {
    while(!fdata.isEmpty())
      delete fdata.takeLast();
    }
  
  void calculate1D();
  void calculate1Dn();
//   void recalculateColors();

  void parentDataAdjusted();
  void parentDataAboutToBeChanged();
  void parentDataChanged();

public:

  NVBSpecMath( NVBSpecDataSource* source );
  virtual ~NVBSpecMath() { ; }

  virtual inline double getZMin() const {return boundingRect().bottom();}
  virtual inline double getZMax() const {return boundingRect().top();}

  virtual inline NVBDimension xDim() const { return sprovider->xDim();}
  virtual inline NVBDimension yDim() const { return sprovider->yDim();}
  virtual inline NVBDimension tDim() const { return sprovider->tDim();}
  
  virtual NVBDimension zDim() const {
    switch (mode) {
      case Normal: return sprovider->zDim();
      case FirstDerivative: return NVBDimension(sprovider->zDim().toStr() + "/" + sprovider->tDim().toStr());
      case NormalizedFirstDerivative: return NVBDimension();
      default : return NVBDimension();
      }
    }
  
  NVB_FORWARD_SPECCOLORS(sprovider);

  virtual inline QList<QPointF> positions() const { return sprovider->positions(); }

  virtual QList<QwtData*> getData() const {
    if (mode != Normal)
      return fdata;
    else
      return sprovider->getData();
    }
  
  virtual inline QSize datasize() const { return sprovider->datasize(); }

  virtual inline QRectF boundingRect() const {
    if (mode != Normal)
      return NVBSpecDataSource::boundingRect();
    else
      return sprovider->boundingRect();
    }
  virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

  static QAction * action();
  QWidget * widget();

public slots:
  void setMode( NVBSpecMath::Mode );
  void setMode( int );
  virtual void setSource(NVBDataSource * source);

private :
  void connectSignals();
};

Q_DECLARE_METATYPE(NVBSpecMath::Mode);

class NVBSpecMathWidget : public QWidget {
Q_OBJECT
private:
  QButtonGroup * buttons;
private slots:
public:
  NVBSpecMathWidget( NVBSpecMath::Mode mode, QWidget * parent = 0);
  virtual ~NVBSpecMathWidget();
signals:
  void mathModeActivated( int );
  void delegateReset();
};

#endif

