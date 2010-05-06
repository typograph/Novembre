//
// C++ Implementation: NVBSpecExcluder
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecExcluder.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include "NVBCurveModel.h"
#include <QIcon>


#include "../icons/excluder.xpm"

class NVBExclInclDiscrColorModel : public NVBDiscrColorModel {
private :
  int eindex;
  NVBSpecExcluder::Mode mode;
  const NVBDiscrColorModel * model;
public :
  NVBExclInclDiscrColorModel(const NVBDiscrColorModel * source = 0):NVBDiscrColorModel(),eindex(-1),mode(NVBSpecExcluder::NoExclusion),model(source) {
    if (model)
      connect(model,SIGNAL(adjusted()),SIGNAL(adjusted()));
    }
    
  ~NVBExclInclDiscrColorModel() {;}
  
  void setModel(const NVBDiscrColorModel * source) {
    if (model) QObject::disconnect(model,0,this,0);
    model = source;
    if (model) {
      connect(model,SIGNAL(adjusted()),SIGNAL(adjusted()));
      emit adjusted();
      }
    }
  virtual QColor colorize(int index) const {
    if (!model) return Qt::black;
    if ( mode == NVBSpecExcluder::NoExclusion || eindex < 0 ) return model->colorize(index);
    else if ( mode == NVBSpecExcluder::Exclude ) {
      if (index < eindex)
        return model->colorize(index);
      else
        return model->colorize(index+1);
      }
    else //  mode == NVBSpecExcluder::Include
      return model->colorize(eindex);
    }
  void setExcludedIndex(int _eindex = -1) {
    if (eindex != _eindex) {
      eindex = _eindex;
      mode = NVBSpecExcluder::Exclude;
      emit adjusted();
      }
    else if ( mode != NVBSpecExcluder::Exclude) {
      mode = NVBSpecExcluder::Exclude;
      emit adjusted();    
      }
    }
  void setIncludedIndex(int _eindex = -1) {
    if (eindex != _eindex) {
      eindex = _eindex;
      mode = NVBSpecExcluder::Include;
      emit adjusted();
      }
    else if ( mode != NVBSpecExcluder::Include) {
      mode = NVBSpecExcluder::Include;
      emit adjusted();    
      }
    }
};

NVBSpecExcluderWidget::NVBSpecExcluderWidget( NVBSpecDataSource * source ):QWidget()
{
  if (!source) throw;
  setWindowTitle("Exclude curves");

  setLayout(new QVBoxLayout(this));
  
  excl_box = new QCheckBox("Exclude",this);
  excl_box->setChecked(false);
  connect(excl_box,SIGNAL(toggled(bool)),SIGNAL(activated(bool)));
  layout()->addWidget(excl_box);
   
   
  QHBoxLayout * l = new QHBoxLayout();
  layout()->addItem(l);
  
  r_excl = new QRadioButton("Curve",this);
  r_excl->setChecked(true);
  r_excl->setEnabled(false);
  connect(excl_box,SIGNAL(toggled(bool)),r_excl,SLOT(setEnabled(bool)));
  l->addWidget(r_excl);
  
  ecurve_list = new QComboBox(this);
  ecurve_list->setEnabled(false);
  ecurve_list->setModel(new NVBCurveModel(source));
  ecurve_list->setCurrentIndex(-1); 
  connect(excl_box,SIGNAL(toggled(bool)),ecurve_list,SLOT(setEnabled(bool)));
  connect(ecurve_list,SIGNAL(currentIndexChanged(int)),SLOT(exclSelected(int)));
  l->addWidget(ecurve_list);
  
  
  l = new QHBoxLayout();
  layout()->addItem(l);
  
  r_incl = new QRadioButton("All except",this);
  r_incl->setChecked(false);
  r_incl->setEnabled(false);
  connect(excl_box,SIGNAL(toggled(bool)),r_incl,SLOT(setEnabled(bool)));
  l->addWidget(r_incl);
  
  icurve_list = new QComboBox(this);
  icurve_list->setEnabled(false);
  icurve_list->setModel(new NVBCurveModel(source));
  icurve_list->setCurrentIndex(-1); 
  connect(excl_box,SIGNAL(toggled(bool)),icurve_list,SLOT(setEnabled(bool)));
  connect(icurve_list,SIGNAL(currentIndexChanged(int)),SLOT(inclSelected(int)));  
  l->addWidget(icurve_list);

}

void NVBSpecExcluderWidget::inclSelected( int i )
{
  r_incl->setChecked(true);
  emit curveSelected(i);
}

void NVBSpecExcluderWidget::exclSelected( int i )
{
  r_excl->setChecked(true);
  emit curveAntiSelected(i);
}

void NVBSpecExcluderWidget::reset( )
{
  excl_box->setChecked(false);
}


NVBSpecExcluder::NVBSpecExcluder( NVBSpecDataSource * source ):NVBSpecFilterDelegate(source),sprovider(source),scolors(new NVBExclInclDiscrColorModel()),index(-1),mode(NoExclusion)
{
  connectSignals();
  followSource();
}

QWidget * NVBSpecExcluder::widget( )
{
  NVBSpecExcluderWidget * w = new NVBSpecExcluderWidget(sprovider);
  connect(w,SIGNAL(curveSelected(int)),SLOT(setIncludeIndex(int)));
  connect(w,SIGNAL(curveAntiSelected(int)),SLOT(setExcludeIndex(int)));
  connect(w,SIGNAL(activated(bool)),SLOT(setActive(bool)));
  connect(this,SIGNAL(delegateReset()),w,SLOT(reset()));
  return w;
}

QAction * NVBSpecExcluder::action( )
{
  return new QAction(QIcon(_excl_excluder),"Exclude curves",0);
}

void NVBSpecExcluder::setActive( bool active )
{
  if ( !active && mode != NoExclusion) {
    emit dataAboutToBeChanged();
    mode = NoExclusion;
    emit dataChanged();    
    }
}

void NVBSpecExcluder::setExcludeIndex( int i )
{
  if (mode == Exclude && index == i) return;
  
  emit dataAboutToBeChanged();
  mode = Exclude;
  index = i;
  ((NVBExclInclDiscrColorModel*)scolors)->setExcludedIndex(index);
  emit dataChanged();    
}

void NVBSpecExcluder::setIncludeIndex( int i )
{
  if (mode == Include && index == i) return;
  
  emit dataAboutToBeChanged();
  mode = Include;
  index = i;
  ((NVBExclInclDiscrColorModel*)scolors)->setIncludedIndex(index);
  emit dataChanged();    
}

void NVBSpecExcluder::setSource( NVBDataSource * source )
{
  if (source) source->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    emit objectPopped(source,this); // going away
    return;
    }

  NVBSpecFilterDelegate::setSource(source);
  
  mode = NoExclusion;
  index = -1;
}

void NVBSpecExcluder::connectSignals( )
{
  sprovider = (NVBSpecDataSource*)provider;

  connect(provider,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged()));
  connect(provider,SIGNAL(dataAdjusted()),SIGNAL(dataAdjusted()));
  connect(provider,SIGNAL(dataChanged()),SLOT(checkIndex()));
  connect(provider,SIGNAL(dataChanged()),SIGNAL(dataChanged()));

  connect(provider,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));
  connect(provider,SIGNAL(colorsChanged()),SLOT(parentColorsChanged()));
  // colorsAdjusted goes through the model

  parentColorsChanged();
}

void NVBSpecExcluder::checkIndex( )
{
  if ( sprovider->datasize().height() <= index ) {
    index = -1;
    mode = NoExclusion;
    emit delegateReset();
    }
}

void NVBSpecExcluder::parentColorsChanged()
{
  ((NVBExclInclDiscrColorModel*)scolors)->setModel(sprovider->getColorModel());
}

void NVBSpecExcluder::parentColorsAboutToBeChanged()
{
  ((NVBExclInclDiscrColorModel*)scolors)->setModel(0);
}


