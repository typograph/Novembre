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
#include <QSlider>
#include <QLabel>
#include <QString>
#include <QVector>
#include <qwt_plot_marker.h>
#include "NVBContColoring.h"

class QAction;

//class NVBSpecSlicerSpinBox : public QSpinBox {
//private:
//  NVBDimension dim;
//  double x0,xstep;
//  int nsteps;
//protected:
//  virtual QString textFromValue ( int value ) const ;
//  virtual int valueFromText ( const QString & text ) const ;
//public:
//  NVBSpecSlicerSpinBox( NVBSpecDataSource * source, QWidget * parent = 0 );
//  virtual ~NVBSpecSlicerSpinBox();
//};

class NVBSpecSlicerLabel : public QLabel {
	Q_OBJECT
private:
	NVBDimension dim;
	double x0,xstep;
	QString templt;
public:
	NVBSpecSlicerLabel( NVBSpecDataSource * source, QWidget * parent = 0 );
	virtual ~NVBSpecSlicerLabel() {;}
public slots:
	void resetParam(NVBSpecDataSource * source);
	void setPos(int value);
};

class NVBSpecSlicerWidget : public QWidget {
Q_OBJECT
private:
//  NVBSpecSlicerSpinBox * valueBox;
	NVBSpecSlicerLabel * valueBox;
	QSlider * slider;
	NVBSpecDataSource * sprovider;
public:
  NVBSpecSlicerWidget( NVBSpecDataSource * source );
  virtual ~NVBSpecSlicerWidget() {;}
public slots:
	inline void setPos(int value) { slider->setValue(value); } // QSpinBox emits valueChanged if it is
	void resetParam(NVBDataSource * source = 0);
signals:
	void posChanged(int value);
};

class NVBSpecSlicerDelegate : public NVB3DFilterDelegate {
Q_OBJECT
protected:
  NVBSpecDataSource * page;
	NVBContColorModel * colors;
	NVBColoring::NVBRescaleColorModel * scolors;
  int slicepos;
	mutable QVector<double*> cache;
	mutable QVector<QPair<double,double> > zlimits;
	int mpp; // Measurements per point
  QSize datasize;
  QRectF rect;
protected slots:
  void invalidateCache();
	void fillCache();
  void initSize();
public:
  NVBSpecSlicerDelegate( NVBSpecDataSource * source );
	virtual ~NVBSpecSlicerDelegate();

  virtual inline QString name() { return provider->name().trimmed() + QString(" sliced"); }
//  virtual QString file() { return provider->file(); }

	virtual inline double getZMin() { return zlimits.at(slicepos).first; }
	virtual inline double getZMax() { return zlimits.at(slicepos).second; }

	virtual inline NVBDimension xDim() const { return page->xDim();}
	virtual inline NVBDimension yDim() const { return page->yDim();}
	virtual inline NVBDimension zDim() const { return page->zDim();}

	virtual inline QSize resolution() const { return datasize; }
	virtual QRectF position() const { return rect;}
	virtual double rotation() const { return 0; }

	virtual const NVBContColorModel * getColorModel() const;
  virtual inline bool canSetColorModel()  { return true; }
  virtual bool setColorModel(NVBContColorModel * colorModel);

	virtual inline const double * getData() const {
		if (!cache.at(slicepos)) return 0;
    return cache.at(slicepos);
    }
	virtual inline double getData(int x, int y) const { return getData()[x+y*datasize.width()]; }
  virtual inline bool canSetData()  { return false; }
  virtual inline bool setData(double *)  { return false; }

	static QAction * action();
	static bool hasGrid(NVBSpecDataSource *, int * =0, int * =0, int * =0);
private :
	void connectSignals();
public slots:
  inline void setPos(int value) {
    if (slicepos != value) {
      slicepos = value;
      fillCache();
			if (scolors) scolors->setLimits(getZMin(), getZMax());
      emit dataChanged();
      }
    }
};

class NVBSpecSlicerPosTracker : public QObject, public QwtPlotMarker {
	Q_OBJECT
private:
	double pos;
	double x0,xstep;
	NVBSpecDataSource * sprovider;
public:
  NVBSpecSlicerPosTracker( NVBSpecDataSource * source );
  virtual ~NVBSpecSlicerPosTracker() {;}
//  virtual void draw(QPainter *painter,
//      const QwtScaleMap &xMap, const QwtScaleMap &yMap,
//      const QRect &canvasRect) const;
public slots:
  void setPos(int value);
	void resetParam(NVBDataSource * source = 0);
signals:
  void posChanged(int value);
};

#endif
