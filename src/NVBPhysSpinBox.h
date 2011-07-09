#ifndef NVBPHYSSPINBOX_H
#define NVBPHYSSPINBOX_H

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QValidator>
#include "NVBUnits.h"

class NVBPhysSpinBox : public QDoubleSpinBox {
Q_OBJECT
private:
// I know this is ugly, but Trolltech doesn't leave me any other choice
// I'll change it when they change the interface to QAbstractSpinBox;
  mutable NVBUnits dim;
  static void normalizeValDim(double & value, NVBUnits & dim);

private slots:
  void emitPhysValue(double v) { emit valueChanged(NVBPhysValue(v,dim)); }

protected:

  virtual StepEnabled stepEnabled () const
    { return QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled ;}

  void stepBy ( int steps );

  virtual QString textFromValue ( double value ) const;
  virtual double valueFromText ( const QString & text ) const;

  virtual QValidator::State validate( QString & input, int & pos ) const;

public:
  NVBPhysSpinBox( NVBUnits dimension, QWidget * parent = 0 );
  virtual ~NVBPhysSpinBox() {;}

  NVBPhysValue physValue() const { return NVBPhysValue(value(),dim); }

public slots:
  void setPhysValue(const NVBPhysValue & value);

signals:
//  void valueChanged(double value);
  void valueChanged(NVBPhysValue value);
};


#endif
