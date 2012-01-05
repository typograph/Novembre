#ifndef NVBSLICEPAINTER_H
#define NVBSLICEPAINTER_H

#include <QWidget>
#include "NVBDataSource.h"
#include "NVBFilterDelegate.h"
#include "NVBContColoring.h"
#include "NVBDiscrColorModel.h"
#include "NVBContColorModel.h"
#include "NVBPhysStepSpinBox.h"

class NVBSliceColorModel : public NVBDiscrColorModel {
Q_OBJECT
private:
  const NVBContColorModel * model;
  const NVBSpecDataSource * sprovider;
  int xindex;
  bool active;
public:
  NVBSliceColorModel( const NVBContColorModel * model = 0, const NVBSpecDataSource * source = 0);
  virtual ~NVBSliceColorModel();

  virtual QColor colorize(int) const;

public slots:
  void setSource(const NVBSpecDataSource * source);
  void setModel(const NVBContColorModel * model);
  void setIndex(int index);

private slots:
  void activate(bool newactive = true) {
    if (active != newactive) {
      active = newactive;
      emit adjusted(); 
      }
    }
  void deactivate() { active = false; }
};

class NVBSlicePainterWidget : public QWidget {
Q_OBJECT
private:
  const NVBSpecDataSource * sprovider;
  NVBPhysStepSpinBox * sbox;
private slots:
  void updateLimits();
public:
  NVBSlicePainterWidget(const NVBSpecDataSource * source);
  virtual ~NVBSlicePainterWidget() {;}
public slots:
  void setSource(const NVBSpecDataSource * source);
signals:
  void indexChanged(int);
};

class NVBSlicePainter : public NVBSpecFilterDelegate {
Q_OBJECT

private:
  NVBSpecDataSource * sprovider;
  NVBSliceColorModel * scolors;
  NVBRescaleColorModel * ccolors;
  NVBSlicePainterWidget * swidget;
public:
  NVBSlicePainter(NVBSpecDataSource * source, NVBContColorModel * model);
  virtual ~NVBSlicePainter() { if (scolors) delete scolors; }

  NVB_FORWARD_SPECDATA(sprovider);

  virtual inline const NVBDiscrColorModel * getColorModel()  const  { return scolors; }

  static QAction * action();
  QWidget * widget();

protected slots:
  void parentColorsAboutToBeChanged() {}
  void parentColorsChanged() {}
  virtual void setSource(NVBDataSource * source);

private :
  void connectSignals();

};

#endif
