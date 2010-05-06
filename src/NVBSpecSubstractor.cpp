//
// C++ Implementation: NVBSpecSubstractor
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBSpecSubstractor.h"

#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include "NVBCurveModel.h"
#include <QIcon>

#include "../icons/substractor.xpm"

class NVBSpecSubstractorData : public QwtData {
private:
  QwtData *a, *b;
public :
  NVBSpecSubstractorData(QwtData* curve,QwtData* substragent= 0):QwtData(),a(curve),b(substragent){;}

  virtual QwtData * copy () const {
    return new NVBSpecSubstractorData(a,b);
    }
  virtual size_t   size () const { if (a) return a->size(); else return 0;}
  virtual double   x (size_t i) const { if (a) return a->x(i); else return 0;}
  virtual double   y (size_t i) const {
    if (a) {
      if (b)
        return a->y(i) - b->y(i);
      else
        return a->y(i);
     }
    return 0;
    }

  void setSubstragent(QwtData* s) { if ((b) != s) b = s;}
};

class NVBExcludingDiscrColorModel : public NVBDiscrColorModel {
private :
  int s_i;
  int orig;
  const NVBDiscrColorModel * model;
public :
  NVBExcludingDiscrColorModel(const NVBDiscrColorModel * source):NVBDiscrColorModel(),s_i(-1),orig(0),model(source) {
    connect(model,SIGNAL(adjusted()),SIGNAL(adjusted()));
    }
  ~NVBExcludingDiscrColorModel() {;}
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
    if ( s_i < 0 ) return model->colorize(index);
    if ( index < orig ) return model->colorize(index);
    if ( index - orig < s_i ) return model->colorize(index-orig);
    else return model->colorize(index-orig+1);
    }
  void setExcludedIndex(int eindex = -1) {
    if (s_i != eindex) {
      s_i = eindex;
      emit adjusted();
      }
    }
  void setShowOriginal(int n_orig = 0) {
    orig = n_orig;
    }
};

NVBSpecSubstractorWidget::NVBSpecSubstractorWidget(NVBSpecDataSource * source):QWidget()
{
  if (!source) throw;

  setLayout(new QVBoxLayout(this));
  QHBoxLayout * l = new QHBoxLayout();
  layout()->addItem(l);
  subst_box = new QCheckBox("Substract",this);
  subst_box->setChecked(false);
  connect(subst_box,SIGNAL(toggled(bool)),SIGNAL(activated(bool)));
  l->addWidget(subst_box);
  curve_list = new QComboBox(this);
  curve_list->setEnabled(false);
  curve_list->setModel(new NVBCurveModel(source));
  curve_list->setCurrentIndex(-1);
  connect(curve_list,SIGNAL(currentIndexChanged(int)),SIGNAL(curveSelected(int)));
  connect(subst_box,SIGNAL(toggled(bool)),curve_list,SLOT(setEnabled(bool)));
  l->addWidget(curve_list);
  orig_box = new QCheckBox("Show original curves",this);
  orig_box->setChecked(false);
  connect(orig_box,SIGNAL(toggled(bool)),SIGNAL(showOriginal(bool)));
  layout()->addWidget(orig_box);
}

void NVBSpecSubstractorWidget::reset()
{
  subst_box->setChecked(false);
  curve_list->setCurrentIndex(-1);

// Don't reset the "show original" state
}

QWidget * NVBSpecSubstractor::widget()
{
  NVBSpecSubstractorWidget * w = new NVBSpecSubstractorWidget(sprovider);
  connect(w,SIGNAL(curveSelected(int)),SLOT(setBackgroundIndex(int)));
  connect(w,SIGNAL(activated(bool)),SLOT(setActive(bool)));
  connect(w,SIGNAL(showOriginal(bool)),SLOT(setShowOriginal(bool)));
  connect(this,SIGNAL(delegateReset()),w,SLOT(reset()));
  return w;
}

QList< QwtData * > NVBSpecSubstractor::getData() const
{
  if (active && bindex >= 0 )
    if (showoriginal) {
      QList< QwtData * > crv;
      crv << sprovider->getData() << sdata;
      return crv;
      }
    else
      return sdata;
  else
    return sprovider->getData();
}

void NVBSpecSubstractor::setActive(bool new_active)
{
  if (active != new_active) {
    if (active || (!active && bindex >= 0))
      emit dataAboutToBeChanged();
      active = new_active;
      emit dataChanged();
    active = new_active;
    }
}

void NVBSpecSubstractor::setBackgroundIndex(int index)
{
  if (bindex == index) return;
  if (sprovider->datasize().height() == 1) return;

  if (scolors) ((NVBExcludingDiscrColorModel*)scolors)->setExcludedIndex(index);

  if (index < 0) {
    setActive(false);
    bindex = index;
    }
  else {
    emit dataAboutToBeChanged();
    if (bindex >= 0 && excluded != 0)
      sdata.insert(bindex,excluded);
    excluded = 0;
    QwtData * back = sprovider->getData().at(index);
    foreach(QwtData* d, sdata) {
      ((NVBSpecSubstractorData*)d)->setSubstragent(back);
      }
    excluded = sdata.takeAt(index);
    bindex = index;
    emit dataChanged();
    }
  if (bindex >= 0)
    bcolor = sprovider->colors().at(bindex);
  else
    bcolor = QColor();
}

NVBSpecSubstractor::NVBSpecSubstractor(NVBSpecDataSource * source):NVBSpecFilterDelegate(source),sprovider(0),bindex(-1),active(false),showoriginal(false),excluded(0)
{

  connectSignals();
  followSource();

}

void NVBSpecSubstractor::setSource(NVBDataSource * source)
{
  if (source) source->disconnect(this);

  if (source->type() != NVB::SpecPage) {
    emit objectPopped(source,this); // going away
    return;
    }

  bcolor = QColor();
  clearData();

  NVBSpecFilterDelegate::setSource(source);

}

void NVBSpecSubstractor::connectSignals()
{
    sprovider = (NVBSpecDataSource*)provider;

    connect(provider,SIGNAL(dataAboutToBeChanged()),SLOT(clearData()));
    connect(provider,SIGNAL(dataAdjusted()),SIGNAL(dataAdjusted()));
    connect(provider,SIGNAL(dataChanged()),SLOT(rebuildData()));

    connect(provider,SIGNAL(colorsAboutToBeChanged()),SLOT(parentColorsAboutToBeChanged()));
    connect(provider,SIGNAL(colorsChanged()),SLOT(parentColorsChanged()));
    // colorsAdjusted goes through the model

    parentColorsChanged();
    rebuildData();

}

void NVBSpecSubstractor::rebuildData()
{
  foreach(QwtData * d, sprovider->getData()) {
    sdata << new NVBSpecSubstractorData(d);
    }

  ((NVBExcludingDiscrColorModel*)scolors)->setModel(sprovider->getColorModel());
  if (showoriginal)
    ((NVBExcludingDiscrColorModel*)scolors)->setShowOriginal( sprovider->datasize().height() );

  
  int cindex = -1; 

  if (bcolor.isValid()) cindex = sprovider->colors().indexOf(bcolor);

  if (cindex >= 0) {
    setBackgroundIndex(cindex);  
    }
  else 
    emit delegateReset();
  emit dataChanged();
}

void NVBSpecSubstractor::clearData()
{
  emit dataAboutToBeChanged();
//   setActive(false);
//   setBackgroundIndex(-1);
  if (excluded != 0) {
    sdata.insert(bindex,excluded);
    excluded = 0;
    }
  bindex = -1;
  ((NVBExcludingDiscrColorModel*)scolors)->setModel(0);
  ((NVBExcludingDiscrColorModel*)scolors)->setExcludedIndex();
  ((NVBExcludingDiscrColorModel*)scolors)->setShowOriginal();
//  if (active) emit dataAboutToBeChanged();
  while (!sdata.isEmpty()) delete sdata.takeFirst();
}

QAction * NVBSpecSubstractor::action()
{
  return new QAction(QIcon(_subst_substractor),"Substract background",0);
}

QSize NVBSpecSubstractor::datasize() const
{
  if (active && bindex >= 0 ) {
    if (showoriginal)
      return QSize(sprovider->datasize().width(),2*sprovider->datasize().height()-1);
    else
      return QSize(sprovider->datasize().width(),sprovider->datasize().height()-1);
    }
  else
    return sprovider->datasize();
}

void NVBSpecSubstractor::parentColorsChanged()
{
  scolors = new NVBExcludingDiscrColorModel(sprovider->getColorModel());
  ((NVBExcludingDiscrColorModel*)scolors)->setExcludedIndex(bindex);
  connect(scolors,SIGNAL(adjusted()),SIGNAL(colorsAdjusted()));
  emit colorsChanged();
}

void NVBSpecSubstractor::parentColorsAboutToBeChanged()
{
  emit colorsAboutToBeChanged();
  if (scolors) {
    delete scolors;
    scolors = 0;
    }
}

void NVBSpecSubstractor::setShowOriginal(bool n_sorig)
{
  emit dataAboutToBeChanged();
  showoriginal = n_sorig;
  ((NVBExcludingDiscrColorModel*)scolors)->setShowOriginal(showoriginal ? sprovider->datasize().height() : 0);
  emit dataChanged();
}


