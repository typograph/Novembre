//
// C++ Interface: NVBProgress
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBPROGRESS_H
#define NVBPROGRESS_H

#include <QEvent>
#include <QProgressBar>
#include <QTime>
#include <QStack>
#include "NVBLogger.h"

#define NVBStartOperation 10117
#define NVBStepOperation 11117
#define NVBStopOperation 12117

class NVBProgressStartEvent : public QEvent
{
private:
  int steps;
  QString opName;
  
public:
  NVBProgressStartEvent(QString operation_name, int _steps = 100):QEvent((QEvent::Type)NVBStartOperation),steps(_steps),opName(operation_name) {;}

  int nSteps() { return steps; }
  QString operationName() { return opName;}
};

class NVBProgressContinueEvent : public QEvent
{
private:
  QString opName;
  
public:
  NVBProgressContinueEvent(QString operation_name = QString()):
    QEvent((QEvent::Type)NVBStepOperation),opName(operation_name) {;}

  QString operationName() { return opName;}
};

class NVBProgressStopEvent : public QEvent
{
private:
  QString opName;
  
public:
  NVBProgressStopEvent(QString operation_name = QString()):
    QEvent((QEvent::Type)NVBStopOperation),opName(operation_name) {;}

  QString operationName() { return opName;}
};

struct NVBLongOperation{
  QString opName;
  int value;
  int maximum;
  QTime starttime;
  
  NVBLongOperation(QString name, int max):opName(name),value(0),maximum(max) { starttime.start(); }
};

class NVBProgressBar : public QProgressBar
{
Q_OBJECT
private:
  bool shown;
  bool active;
  QStack<NVBLongOperation*> operations;
  
  void initFromStack() {
    shown = false;
    if (operations.isEmpty()) {
//       setValue(0);
      setFormat("%p%");
      active = false;
      }
    else {
      setMaximum(operations.top()->maximum);
      setValue(operations.top()->value);
      setFormat(operations.top()->opName + " : %p%");
      operations.top()->starttime.start();
      active = true;
      }
    }
  
  void updateStack() {
    if (!operations.isEmpty()) {
      operations.top()->value = value();
      }
    }

public:
  
  NVBProgressBar( QWidget * parent = 0 ):QProgressBar(parent),shown(false),active(false) {
    setVisible(false);
    setTextVisible(true);
    }
 ~NVBProgressBar() {
    while (!operations.isEmpty()) delete operations.pop();
   }


  virtual bool event( QEvent * e ) {
    switch (e->type()) {
      case NVBStartOperation : {
        updateStack();
        NVBProgressStartEvent * se = dynamic_cast<NVBProgressStartEvent*>(e);
        operations.push(new NVBLongOperation(se->operationName(),se->nSteps()));
        initFromStack();
        e->accept();
        return true;
        }
      case NVBStepOperation : {
        if (active) {
          setValue(value() + 1);
//           NVBOutputDMsg("Step");
          if (!shown) {
            if (operations.top()->starttime.elapsed()*maximum() > 50*value()) {
              shown = true;
//               NVBOutputDMsg("I wanna be visible");
              emit wannaBeVisible(true);
              }
//             else
//               NVBOutputDMsg("I decided notta get visible");
            }
          e->accept();
          return true;
          }
        else {
          NVBOutputDMsg("Don't send stupid messages to me!");
          e->ignore();
          return false;
          }
        }
      case NVBStopOperation : {
//         NVBOutputDMsg("I don wanna be visible");
        delete operations.pop();
        initFromStack();
        emit wannaBeVisible(shown);
        e->accept();
        return true;
        }
      default : {
        return QProgressBar::event(e);
        }
      }
    }

signals:
  void wannaBeVisible(bool);
};

#endif // NVBPROGRESS_H
