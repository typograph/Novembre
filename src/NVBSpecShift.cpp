//
// C++ Implementation: NVBSpecShift
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecShift.h"
// #include "NVBAverageColor.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QAbstractSpinBox>
#include <QPushButton>

#include "../icons/shift.xpm"

char sign(double x) {
  return x == 0 ? 0 : ( x > 0 ? 1 : -1 );
  }

class NVBShiftCPtData : public QwtData {
public:
     NVBShiftCPtData(QwtData * data, double * x_shift, double * y_shift):d_xshift(x_shift),d_yshift(y_shift),d_data(data) {;}
     ~NVBShiftCPtData() {;}

     virtual QwtData *copy() const {
        return new NVBShiftCPtData(d_data,d_xshift,d_yshift);
       }   
     
     virtual size_t size() const { return d_data->size();}
     virtual double x(size_t i) const { return *d_xshift + d_data->x(i); };
     virtual double y(size_t i) const { return *d_yshift + d_data->y(i); };
 
 private:
     double *d_xshift, *d_yshift;
     QwtData *d_data;
};

// class NVBDimensionedSpinBox : public QAbstractSpinBox {
// Q_OBJECT
// private:
//   NVBDimension dim;
// //   double x0,xstep;
// //  int nsteps;
//   NVBPhysValue my_value;
// protected:
// 
//   void renderValue() {
//     lineEdit()->setText(my_value.toString());
//     }
// 
//   virtual QValidator::State validate ( QString & input, int & pos ) const {
//     return QValidator::Acceptable;
//     }
//     
//   virtual StepEnabled stepEnabled () const
//     { return QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled ;}
//     
//   void stepBy ( int steps ) {
//     if (my_value.getValue() == 0)
//       myValue = NVBPhysValue(steps,dim);
//     else
//       my_value *= (1+steps*0.1);
//     renderValue();
//     emit valueChanged(my_value.getValue(dim));
//     emit valueChanged(my_value);
//     }
//     
// public:
//   NVBDimensionedSpinBox( NVBDimension dimension, QWidget * parent = 0 )
//     : QDoubleSpinBox(parent)
//     , dim(dimension)
//     , my_value(NVBPhysValue(0,dim))
//     {
// //       connect(this,SIGNAL(editingFinished),this,SLOT());
//     renderValue();
//     }
//   virtual ~NVBDimensionedSpinBox() {;}
// signals:
//   void valueChanged(double value);
//   void valueChanged(NVBPhysValue value);
// };



NVBSpecShiftWidget::NVBSpecShiftWidget( QWidget * parent ):QWidget(parent)
{
  setWindowTitle("Shifting");

//  QGridLayout * l = new QGridLayout(this);
  QVBoxLayout * l = new QVBoxLayout(this);
  
//   NVBDimensionedSpinBox * sb = new NVBDimensionedSpinBox();
  
  QPushButton * btn = new QPushButton("y -> 0",this);
  connect(btn,SIGNAL(clicked(bool)),this,SIGNAL(smashY()));
  
  l->addWidget(btn);
  
  btn = new QPushButton("x -> 0",this);
  connect(btn,SIGNAL(clicked(bool)),this,SIGNAL(smashX()));
  
  l->addWidget(btn);

  btn = new QPushButton("Clear",this);
  connect(btn,SIGNAL(clicked(bool)),this,SIGNAL(delegateReset()));
  
  l->addWidget(btn);

  
//   btn = new QRadioButton("Normalized 1st derivative",this);
//   buttons->addButton(btn);
//   buttons->setId(btn,NVBSpecShift::NormalizedFirstDerivative);
//   l->addWidget(btn);

  l->setSizeConstraint(QLayout::SetFixedSize);

  setLayout(l);
//  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

}

NVBSpecShiftWidget::~ NVBSpecShiftWidget()
{
}

NVBSpecShift::NVBSpecShift(NVBSpecDataSource * source):
    NVBSpecFilterDelegate(source),sprovider(0),xshift(0),yshift(0)
{
  connectSignals();
  followSource();
}

QAction * NVBSpecShift::action()
{
  return new QAction(QIcon(_shift_icon),QString("Shift"),0);
}

QWidget * NVBSpecShift::widget()
{
  NVBSpecShiftWidget * widget = new NVBSpecShiftWidget( );
  connect(widget,SIGNAL(smashY()),this,SLOT(moveYtoZero()));
  connect(widget,SIGNAL(smashX()),this,SLOT(moveXtoZero()));
  connect(widget,SIGNAL(delegateReset()),this,SLOT(resetShifts()));
//   connect(widget,SIGNAL(mathModeActivated( int )),SLOT(setMode( int )));
  return widget;
}

void NVBSpecShift::setSource(NVBDataSource * source)
{
  if (sprovider) sprovider->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    emit objectPopped(source,this); // going away
    return;
    }

  NVBSpecFilterDelegate::setSource(source);
}

void NVBSpecShift::connectSignals()
{
//### emit delegateReset();
    
    emit dataAboutToBeChanged();
    
    clearFData();
    
    sprovider = (NVBSpecDataSource*)provider;
    
    buildFData();

    emit dataChanged();

    connect(provider,SIGNAL(dataAdjusted()),SIGNAL(dataAdjusted()));
    connect(provider,SIGNAL(dataAboutToBeChanged()),SLOT(parentDataAboutToBeChanged()));
    connect(provider,SIGNAL(dataChanged()),SLOT(parentDataChanged()));
    
    NVB_FORWARD_COLORSIGNALS(sprovider);

}

void NVBSpecShift::parentDataChanged()
{
  buildFData();
  emit dataChanged();
}

void NVBSpecShift::parentDataAboutToBeChanged()
{
  emit dataAboutToBeChanged();
  clearFData();
}

void NVBSpecShift::buildFData( )
{
  foreach(QwtData * d, sprovider->getData())
    fdata << new NVBShiftCPtData(d,&xshift,&yshift);
}

void NVBSpecShift::moveYtoZero( )
{
  QwtData * fs = sprovider->getData().first();
  double px = fs->x(0);
  size_t i;
  for (i = 1; i < fs->size(); i++) {
    if (sign(px) != sign(fs->x(i))) break;
    px = fs->x(i);
    }
  if (px != fs->x(fs->size()-1)) {
    yshift = - fs->y(i-1) + px*(fs->y(i)-fs->y(i-1))/(fs->x(i)-px);
    emit dataAdjusted();
    }   
}

void NVBSpecShift::moveXtoZero( )
{
  QwtData * fs = sprovider->getData().first();
  double py = fs->y(0);
  size_t i;
  for (i = 1; i < fs->size(); i++) {
    if (sign(py) != sign(fs->y(i))) break;
    py = fs->y(i);
    }
  if (py != fs->y(fs->size()-1)) {
    xshift = - fs->x(i-1) + py*(fs->x(i)-fs->x(i-1))/(fs->y(i)-py);
    emit dataAdjusted();
    }   
}

void NVBSpecShift::resetShifts( )
{
  xshift = 0;
  yshift = 0;
  emit dataAdjusted();
}
