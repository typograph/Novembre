//
// C++ Interface: NVBIconVizDelegates
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBICONVIZ_H
#define NVBICONVIZ_H

#include "dimension.h"
#include <QColor>
#include <qwt_data.h>
#include <QIconEngine>
#include <QIcon>
#include <QPainter>
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

/**
 * Visualisation as plain image of some kind. Used by the browser.
 */

#if QT_VERSION >= 0x040300
class NVBIconVizDelegate : public QObject, public QIconEngineV2 {
#else
class NVBIconVizDelegate : public QObject, public QIconEngine {
#endif
Q_OBJECT
protected:
//  NVBDataSource * provider;
  QImage* cache;
  void  connectSource(NVBDataSource * source) {
          connect(source, SIGNAL( dataChanged() ), SLOT( redrawCache() ) );
          connect(source, SIGNAL( colorsChanged() ), SLOT( redrawCache() ) );
//           connect(source, SIGNAL(  objectChanged(NVBDataSource*, NVBDataSource* ) ), SLOT( setSource(NVBDataSource*)) );
          }
public :
#if QT_VERSION >= 0x040300
  NVBIconVizDelegate(NVBDataSource* source):QIconEngineV2(),cache(0) {
#else
  NVBIconVizDelegate(NVBDataSource* source):QIconEngine(),cache(0) {
#endif
		if (!source)
			NVBOutputError("NULL page");
		else
			connectSource(source);
    }
  virtual ~NVBIconVizDelegate() { if (cache) delete (cache);}

  virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) {
    if (cache) {
      QSize z = cache->size();
      z.scale(rect.size(),Qt::KeepAspectRatio);
      QRect nrect = QRect(QPoint(),z);
      nrect.moveCenter(rect.center());
      painter->drawImage(nrect,*cache);
    }
  }

protected slots:
  virtual void redrawCache() = 0;
public slots:
  virtual void setSource( NVBDataSource * ) = 0;
};

class NVBTopoIconDelegate : public NVBIconVizDelegate {
Q_OBJECT
private:
  NVB3DDataSource * page;
public:
  NVBTopoIconDelegate(NVBDataSource * source);
  virtual ~NVBTopoIconDelegate() {;}
protected slots:
  virtual void redrawCache();
public slots:
  virtual void setSource( NVBDataSource * );
};

class NVBSpecIconDelegate : public NVBIconVizDelegate {
Q_OBJECT
protected:
  virtual void redrawCache(const QSize & size);
  NVBSpecDataSource * page;
public:
  NVBSpecIconDelegate(NVBDataSource * source);
  virtual ~NVBSpecIconDelegate() {;}

  virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
protected slots:
  virtual void redrawCache() {
    if (cache)
      redrawCache(cache->size());
    }
public slots:
  virtual void setSource( NVBDataSource * source);
};

#endif
