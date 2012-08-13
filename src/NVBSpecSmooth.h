//
// C++ Interface: NVBSpecSmooth
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBSPECSMOOTH_H
#define NVBSPECSMOOTH_H

#include "NVBFilterDelegate.h"

class QAction;

class NVBSpecSmoother : public NVBSpecFilterDelegate {
Q_OBJECT
private:
  NVBSpecDataSource * sprovider;
  double env_width;
  bool equalSpacing;
  int ptsFactor;
  QwtArray<double> tdata;

  QwtArray<double> xPoints();
  double wrap(double center, const QwtData * data);
	double envelope(double x);

  QList<QwtData*> fdata;

public:
  NVBSpecSmoother(NVBSpecDataSource * data);
  virtual ~NVBSpecSmoother();

  virtual inline double getZMin() const {return boundingRect().bottom();}
  virtual inline double getZMax() const {return boundingRect().top();}

  NVB_FORWARD_SPECDIM(sprovider);
  NVB_FORWARD_SPECCOLORS(sprovider);
  NVB_FORWARD_COMMENTS(sprovider);

  virtual inline QList<QPointF> positions() const { return sprovider->positions(); }

  virtual QList<QwtData*> getData() const { return fdata; }

  virtual QSize datasize() const;

  virtual inline QRectF boundingRect() const { return NVBSpecDataSource::boundingRect(); }
  virtual inline QRectF occupiedArea() const { return sprovider->occupiedArea(); }

  static QAction * action();
  QWidget * widget();

public slots:
  virtual void setSource(NVBDataSource * source);

  void setWidth(const NVBPhysValue & width);
  void setEqualSpacing(bool equal);
  void setSubdivisions(int div);

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
  
  void calculate();

//   void parentDataAdjusted();
//   void parentDataAboutToBeChanged();
//   void parentDataChanged() ;

private :
  void connectSignals();

friend class NVBSpecSmoothWidget;

};

#include <QWidget>

class NVBSpecSmoothWidget : public QWidget {
Q_OBJECT

public:
  NVBSpecSmoothWidget(NVBSpecDataSource* source, NVBSpecSmoother * parent);

signals:
  void widthChanged(NVBPhysValue);
  void subdivChanged(int);
  void homogenityChanged(bool);

};

#endif
