//
// C++ Interface: NVBContColorScaler
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBCONTCOLORSCALER_H
#define NVBCONTCOLORSCALER_H

#include "NVBFilterDelegate.h"
#include "NVBContColoring.h"

#include <QPointer>
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>


class NVBContColorScalerDelegate : public NVB3DFilterDelegate {
Q_OBJECT
friend class NVBContColorScaler;
private:
  NVB3DDataSource * page;
  NVBRescaleColorModel * colors;
protected slots:
  void setModel(NVBRescaleColorModel * model);
  void parentColorsChanged();
  void parentColorsAboutToBeChanged();
public:
  NVBContColorScalerDelegate( NVB3DDataSource* source, NVBRescaleColorModel * model);
  virtual ~NVBContColorScalerDelegate() {;}

  NVB_FORWARD_TOPODATA(page);

  virtual inline const NVBContColorModel * getColorModel() const { return colors; }

private:
  void connectSignals();

public slots:
  virtual void setSource(NVBDataSource * source);
};

class NVBContColorScaler : public QWidget {
Q_OBJECT
private:
  NVBRescaleColorModel * colors;
  QPointer<NVBContColorScalerDelegate> page;
  NVB3DDataSource * provider;
  QImage * histogram;
  QImage * model;
  int z1,z2,c1,c2;
  int * movingrect;
  bool dirty;

  int cy() const;
  int zy() const;
  int maxx() const;
  int minx() const;
  int histy() const;
  int histw() const;
  int histh() const;

public:
  NVBContColorScaler(NVB3DDataSource* source);
  virtual ~NVBContColorScaler() { /* if (page) delete page; */ }
  NVBDataSource * filter();
  static QAction * action();
  virtual QSize sizeHint () const;
  virtual QSize minimumSizeHint () const;
  virtual int heightForWidth ( int w ) const;
protected:
  virtual void paintEvent ( QPaintEvent * event );
  virtual void mousePressEvent( QMouseEvent * event );
  virtual void mouseMoveEvent( QMouseEvent * event );
  virtual void mouseReleaseEvent( QMouseEvent * event );
  virtual void resizeEvent ( QResizeEvent * event );
protected slots:
  void paintHistogram(QPainter * painter, int w, int h);
  void paintModel(QPainter * painter, double zmin, double zmax, int w, int h);
  void parentDataChanged();
public slots:
  void setSource(NVBDataSource * source);
  void invalidateModel();
};

#endif
