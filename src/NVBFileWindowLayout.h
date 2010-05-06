#ifndef NVBFILEWINDOWLAYOUT_H
#define NVBFILEWINDOWLAYOUT_H

#include <QLayout>

class QWidget;
class NVBDataView;

class NVBFileWindowLayout : public QHBoxLayout {
private:
  bool active;

  int minAcceptableWidth(int h) const;
  int maxAcceptableHeight(int w) const;
#ifndef NVB_NO_FW_DOCKS
  QWidget * leftDock;
  QWidget * rightDock;
#endif
  NVBDataView * centralView;
  QLayoutItem * toolBar;

public:
  NVBFileWindowLayout(QWidget * parent);
  virtual ~NVBFileWindowLayout();

#ifndef NVB_NO_FW_DOCKS
  void setLeftDock(QWidget * dock);
  void setRightDock(QWidget * dock);
#endif
  void setToolBar(QLayoutItem * toolbar);
  void setView(NVBDataView * view);

  virtual QSize minimumSize () const { return QSize(100,100); } // TODO
  virtual QSize sizeHint () const { return QSize(minAcceptableWidth(200),200); }
  virtual void setGeometry ( const QRect & r );

  QSize closestAcceptableSize(const QSize & request, int width_correction, int height_correction) const;
};

#endif
