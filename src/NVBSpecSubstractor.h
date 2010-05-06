//
// C++ Interface: NVBSpecSubstractor
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSPECSUBSTRACTOR_H
#define NVBSPECSUBSTRACTOR_H

#include "NVBFilterDelegate.h"
#include <QWidget>
#include <QAction>

class QCheckBox;
class QComboBox;

class NVBSpecSubstractorWidget : public QWidget {
Q_OBJECT
private:
  QCheckBox *subst_box, *orig_box;
  QComboBox *curve_list;
public:
  NVBSpecSubstractorWidget(NVBSpecDataSource * source);
signals :
  void activated(bool);
  void curveSelected(int);
  void showOriginal(bool);
public slots:
  void reset();
};

class NVBSpecSubstractor : public NVBSpecFilterDelegate {
Q_OBJECT
private :
  NVBSpecDataSource * sprovider;
  int bindex;
  QColor bcolor;
  bool active;
  bool showoriginal;
  QwtData * excluded;
  QList<QwtData*> sdata;
  NVBDiscrColorModel * scolors;
public  :
  NVBSpecSubstractor(NVBSpecDataSource * source);
  virtual ~NVBSpecSubstractor() {;}

  NVB_FORWARD_SPECDIM(sprovider);

  virtual const NVBDiscrColorModel * getColorModel()  const  {
    if (active)
      return scolors;
    else
      return sprovider->getColorModel();
      }

  /// \returns a list of position if=n real space where curves were taken
  virtual QList<QPointF> positions() const {
    if (active && bindex >= 0) {
      QList<QPointF> p = sprovider->positions();
      p.takeAt(bindex);
      if (showoriginal) {
        QList<QPointF> pp;
        pp << sprovider->positions() << p;
        return pp;
        }
      else
        return p;
      }
    else 
      return sprovider->positions();
    }

  /// \returns a list of pointers to the curves of the page in form of \c QwtData .
  virtual QList<QwtData*> getData() const;
  /// \returns \b true if the page data can be changed

  /// \returns number of points per curve as QSize::width() and number of curves in page as QSize::height()
  virtual QSize datasize() const;

  QWidget * widget();
  static QAction * action();

public slots:
  void setActive(bool);
  void setBackgroundIndex(int);
  void setShowOriginal(bool);
  virtual void setSource(NVBDataSource * source);

private slots:
  void clearData();
  void rebuildData();
  void parentColorsChanged();
  void parentColorsAboutToBeChanged();

private:
  void connectSignals();

signals:
  void delegateReset();
};

#endif
