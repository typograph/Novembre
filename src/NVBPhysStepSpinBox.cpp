
#include "NVBPhysStepSpinBox.h"
#include <QLineEdit>

QString NVBPhysStepSpinBox::textFromValue(int value) const
{
  return NVBPhysValue(dataScaler.scale(value),dim).toString(0);
}

NVBPhysStepSpinBox::NVBPhysStepSpinBox( NVBDimension dimension, double start, double end, int steps, QWidget * parent) : QSpinBox(parent), dim(dimension)
{
  lineEdit()->setReadOnly(true);

  setMinimumWidth(fontMetrics().boundingRect("0.000 ZM").width());
  setLimits(start,end,steps);
  setValue(0);

  connect(this,SIGNAL(valueChanged(int)),this,SLOT(emitPhysValue(int)));
}

void NVBPhysStepSpinBox::setLimits(double start, double end, int steps)
{
  dataScaler = scaler<int,double>(0,steps-1,start,end);
  setMinimum(0);
  setMaximum(steps-1); // == 99.99999(9)
}

