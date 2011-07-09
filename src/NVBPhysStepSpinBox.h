#ifndef NVBPHYSSPINBOX_H
#define NVBPHYSSPINBOX_H

#include <QtGui/QSpinBox>
#include "NVBUnits.h"
#include "NVBScaler.h"

class NVBPhysStepSpinBox : public QSpinBox {
Q_OBJECT
private:
  mutable NVBUnits dim;
  NVBValueScaler<int,double> dataScaler;

private slots:
  void emitPhysValue(int v) { emit valueChanged(NVBPhysValue(dataScaler.scale(v),dim)); }

protected:
  virtual QString textFromValue ( int value ) const;
public:
  NVBPhysStepSpinBox( NVBUnits dimension, double start, double end, int steps, QWidget * parent = 0 );
  virtual ~NVBPhysStepSpinBox() {;}

  NVBPhysValue physValue() const { return NVBPhysValue(value(),dim); }

  void setLimits(double start, double end, int steps);

signals:
  void valueChanged(NVBPhysValue value);
};


#endif
