//
// C++ Interface: NVBSpecSlicer
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSPECSLICER_H
#define NVBSPECSLICER_H

#include "NVBDataSource.h"
#include "NVBContColorModel.h"
#include "NVBFilterDelegate.h"
#include <QWidget>
#include <QSpinBox>
#include <QString>
#include <QVector>


class NVBSpecSlicerSpinBox : public QSpinBox {
private:
  NVBDimension dim;
  double x0,xstep;
  int nsteps;
protected:
  virtual QString textFromValue ( int value ) const ;
  virtual int valueFromText ( const QString & text ) const ;
public:
  NVBSpecSlicerSpinBox( NVBSpecDataSource * source, QWidget * parent = 0 );
  virtual ~NVBSpecSlicerSpinBox();
};

class NVBSpecSlicerWidget : public QWidget {
Q_OBJECT
private:
  NVBSpecSlicerSpinBox * valueBox;
public:
  NVBSpecSlicerWidget( NVBSpecDataSource * source );
  virtual ~NVBSpecSlicerWidget() {;}
public slots:
  inline void setPos(int value) { valueBox->setValue(value); } // QSpinBox emits valueChanged if it is
signals:
  void posChanged(int value);
};

class NVBSpecSlicerDelegate : public NVB3DFilterDelegate {
Q_OBJECT
protected:
  NVBSpecDataSource * page;
  NVBContColorModel * colors;
  int slicepos;
  QVector<double*> cache;
  double zmin,zmax;
  QSize datasize;
  QRectF rect;
  void getMinMax();
protected slots:
  void invalidateCache();
  void fillCache();
  void initSize();
public:
  NVBSpecSlicerDelegate( NVBSpecDataSource * source );
  virtual ~NVBSpecSlicerDelegate() { invalidateCache(); }

  virtual inline QString name() { return provider->name().trimmed() + QString(" sliced"); }
//  virtual QString file() { return provider->file(); }

  virtual inline double getZMin() { return zmin; }
  virtual inline double getZMax() { return zmax; }

  virtual inline NVBDimension xDim() { return page->xDim();}
  virtual inline NVBDimension yDim() { return page->yDim();}
  virtual inline NVBDimension zDim() { return page->zDim();}

  virtual inline QSize resolution() { return datasize; }
  virtual QRectF position() { return rect;}

  virtual inline const NVBContColorModel * getColorModel() const  { return colors; }
  virtual inline bool canSetColorModel()  { return true; }
  virtual bool setColorModel(NVBContColorModel * colorModel);

  virtual inline const double * getData() {
    if (!cache.at(slicepos))
      fillCache();
    return cache.at(slicepos);
    }
  virtual inline double getData(int x, int y) { return getData()[x+y*datasize.width()]; }
  virtual inline bool canSetData()  { return false; }
  virtual inline bool setData(double *)  { return false; }


public slots:
  inline void setPos(int value) {
    if (slicepos != value) {
      invalidateCache();
      slicepos = value;
      fillCache();
      emit dataChanged();
      }
    }
};

class NVBSpecSlicerPosTracker : public QObject, public QwtPlotItem {
Q_OBJECT
public:
  NVBSpecSlicerPosTracker( NVBSpecDataSource * source );
  virtual ~NVBSpecSlicerPosTracker() {;}
  virtual void draw(QPainter *painter, 
      const QwtScaleMap &xMap, const QwtScaleMap &yMap,
      const QRect &canvasRect) const;
public slots:
  void setPos(int value);
signals:
  void posChanged(int value);
};

#endif
