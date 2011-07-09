//
// C++ Interface: NVBMainDocks
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBMAINDOCKS_H
#define NVBMAINDOCKS_H

#include <QtGui/QDockWidget>
#include <QtGui/QCloseEvent>
#include <QtCore/QVariant>

#include "NVBLogUtils.h"

class NVBPermanentDock : public QDockWidget {
Q_OBJECT
public:
  NVBPermanentDock(const QString& title, QWidget* parent = 0, Qt::WindowFlags flags = 0 ):QDockWidget(title,parent,flags) {;}

protected:
  virtual void closeEvent ( QCloseEvent * event ) { event->ignore(); hide(); }

};

#ifdef NVB_ENABLE_LOG

class NVBLogWidgetDock : public NVBPermanentDock {
Q_OBJECT
public:
  NVBLogWidgetDock( NVBLogWidget * widget, QWidget * parent = 0 ):NVBPermanentDock("Log",parent) {
    setWidget(widget);
    widget->show();
    hide();
    }
public slots:
  virtual void closeEvent ( QCloseEvent * event ) { event->ignore(); hide(); }
};

#endif

template <bool CW>
class NVBAutoDock : public NVBPermanentDock {
private:
  bool wanna_shown;

public:
  NVBAutoDock(const QString& title, QWidget* parent = 0, Qt::WindowFlags flags = 0 ):NVBPermanentDock(title,parent,flags),wanna_shown(false) {;}

  void setWidget ( QWidget * new_widget );

protected:
  virtual void closeEvent ( QCloseEvent * event ) {
    wanna_shown = false;
    NVBPermanentDock::closeEvent(event);
    }
};

template <>
inline void NVBAutoDock<true>::setWidget ( QWidget * new_widget ) {
  if (new_widget == widget()) return;
  if (widget())  {
      widget()->disconnect(this);
      if (!widget()->property("empty").isValid() || !widget()->property("empty").toBool())
        wanna_shown = isVisible();
    }
  NVBPermanentDock::setWidget(new_widget);
  if (new_widget) {
    // This, of course, will only work for some widgets -- e.g. stackView
    connect(new_widget,SIGNAL(empty()),this,SLOT(hide()));
    connect(new_widget,SIGNAL(contentsChanged()),this,SLOT(show()));

    if (new_widget->property("empty").isValid() && new_widget->property("empty").toBool())
      hide();
    else
      setVisible(wanna_shown);
    }
  else
    hide();
}

template <>
inline void NVBAutoDock<false>::setWidget ( QWidget * new_widget ) {
  if (new_widget == widget()) return;
  if (widget())
    wanna_shown = isVisible();

  NVBPermanentDock::setWidget(new_widget);

  if (new_widget)
    setVisible(wanna_shown);
  else
    hide();
}

#endif
