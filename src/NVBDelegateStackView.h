//
// C++ Interface: NVBDelegateStackView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBDELEGATESTACK_H
#define NVBDELEGATESTACK_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLayout>
#include "NVBWidgetStackModel.h"
#include "QCollapsibleBox.h"

class NVBDelegateStackEntry : public QCollapsibleBox {
// Q_OBJECT
public:
  NVBDelegateStackEntry( QWidget* content = 0, QWidget* parent = 0);
  virtual ~NVBDelegateStackEntry() {;}
// public slots:
//   void collapse();
//   void deactivate();
// signals:
//  void activated( NVBDelegateStackEntry* );
};

class NVBDelegateStackView : public QScrollArea {
Q_OBJECT
private:
  NVBWidgetStackModel * wmodel;
  int selection;
public:
 Q_PROPERTY(bool empty
            READ isEmpty());


  NVBDelegateStackView(NVBWidgetStackModel * widgets);
  virtual ~NVBDelegateStackView() {;}

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  bool isEmpty();

public slots:
  void setSelectedIndex(const QModelIndex& index);
  void clear();
  void refresh();

  void widgetsAboutToBeRemoved( const QModelIndex & parent, int start, int end );
  void widgetsInserted( const QModelIndex & parent, int start, int end );
  void vizwidgetInserted(const QModelIndex& index);
signals:
  void empty();
  void contentsChanged();
};

#endif
