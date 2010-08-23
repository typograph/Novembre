#ifndef NVBPHYSSPINBOX_H
#define NVBPHYSSPINBOX_H

#include <QSpinBox>
#include <QValidator>
#include "NVBDimension.h"
#include "NVBScaler.h"

class NVBPhysStepSpinBox : public QSpinBox {
Q_OBJECT
private:
  mutable NVBDimension dim;
  NVBValueScaler<int,double> dataScaler;

private slots:
  void emitPhysValue(int v) { emit valueChanged(NVBPhysValue(dataScaler.scale(v),dim)); }

protected:
  virtual QString textFromValue ( int value ) const;
public:
  NVBPhysStepSpinBox( NVBDimension dimension, double start, double end, int steps, QWidget * parent = 0 );
  virtual ~NVBPhysStepSpinBox() {;}

  NVBPhysValue physValue() const { return NVBPhysValue(value(),dim); }

  void setLimits(double start, double end, int steps);

signals:
  void valueChanged(NVBPhysValue value);
};


#endif
