#include "NVBFileWindowLayout.h"

#include "NVBDataView.h"
#include <QWidget>
#include <QDebug>

/**
 We assume the central widget has to take as much space as possible

 */

int NVBFileWindowLayout::minAcceptableWidth(int h) const
{
  int minw = (toolBar ? toolBar->minimumSize().width() + spacing() : 0) + (centralView ? centralView->widthForHeight(h) : 0);
#ifndef NVB_NO_FW_DOCKS
  if (leftDock && leftDock->isVisible())
    minw += leftDock->minimumSizeHint().width() + spacing();
  if (rightDock && rightDock->isVisible())
    minw += rightDock->minimumSizeHint().width() + spacing();
#endif
  return minw;
}

int NVBFileWindowLayout::maxAcceptableHeight(int w) const
{
  if (centralView)
    return centralView->w()->heightForWidth(
        w
        - (toolBar ? toolBar->minimumSize().width() + spacing(): 0 )
#ifndef NVB_NO_FW_DOCKS
        - ((leftDock && leftDock->isVisible()) ? leftDock->minimumSizeHint().width()+spacing() : 0)
        - ((rightDock && rightDock->isVisible()) ? rightDock->minimumSizeHint().width()+spacing() : 0)
#endif
        );
  else
#ifndef NVB_NO_FW_DOCKS
    return qMin( qMin(
        (leftDock && leftDock->isVisible()) ? leftDock->maximumHeight() : 0,
        (rightDock && rightDock->isVisible()) ? rightDock->maximumHeight() : 0) ,
                 toolBar ? toolBar->maximumSize().height() : 0
                 );
#else
  return (toolBar ? toolBar->maximumSize().height() : 0);
#endif
}

QSize NVBFileWindowLayout::closestAcceptableSize(const QSize & request, int width_correction, int height_correction) const
{
  if (!active) return request;

  int ml,mr,mt,mb;
  getContentsMargins(&ml,&mt,&mr,&mb);
  height_correction += mt + mb;
  width_correction  += ml + mr;

  int wfh = minAcceptableWidth(request.height()-height_correction)+width_correction;
  if (wfh > request.width())
    return QSize(wfh,request.height());
  int hfw = maxAcceptableHeight(request.width()-width_correction)+height_correction;
#ifndef NVB_NO_FW_DOCKS
  if (hfw < request.height())
    return QSize(request.width(),hfw);
  if ((!leftDock || leftDock->isHidden()) &&  (!rightDock || rightDock->isHidden()))
#endif
    return QSize(request.width(),hfw);
#ifndef NVB_NO_FW_DOCKS
  return request;
#endif
}

NVBFileWindowLayout::NVBFileWindowLayout(QWidget * parent)
    : QHBoxLayout(parent)
    , active(false)
#ifndef NVB_NO_FW_DOCKS
    , leftDock(0)
    , rightDock(0)
#endif
    , centralView(0)
    , toolBar(0)
{
  setSpacing(2);
  setContentsMargins(1,1,1,1);
}

NVBFileWindowLayout::~ NVBFileWindowLayout()
{
}

void NVBFileWindowLayout::setGeometry(const QRect & r)
{
  if (!active) {
    QHBoxLayout::setGeometry(r);
    return;
  }

  int ml,mr,mt,mb;
  getContentsMargins(&ml,&mt,&mr,&mb);

  //  qDebug() << r;

  int topc = r.y() + mt;
  int minleft = r.x() + ml;
  //  qDebug() << minleft;
  int width_leftover = r.width()-ml-mr;
  int gen_height = r.height() - mt - mb;
  if (toolBar) {
    int w = toolBar->minimumSize().width();
    toolBar->setGeometry(QRect(minleft,topc,w,gen_height));
    width_leftover -= w + spacing();
    minleft += w + spacing();
    //  qDebug() << minleft;
  }
	int newCWidth = centralView && centralView->hasWidthForHeight() ? centralView->widthForHeight(gen_height) : 0;
  int extra_dock_width = width_leftover - newCWidth;
#ifndef NVB_NO_FW_DOCKS
  int ndocks = 0;
  bool haveleft = leftDock && leftDock->isVisible();
  int wLeft = haveleft ? leftDock->minimumSizeHint().width() : 0;
  bool haveright = rightDock && rightDock->isVisible();
  int wRight = haveright ? rightDock->minimumSizeHint().width() : 0;
  if (haveleft) {
    extra_dock_width -= wLeft + spacing();
    ndocks += 1;
  }
  if (haveright) {
    extra_dock_width -= wRight + spacing();
    ndocks += 1;
  }
  if (ndocks != 0) {
    extra_dock_width /= ndocks;
    wLeft += extra_dock_width;
    wRight += extra_dock_width;
    if (haveleft) {
      leftDock->setGeometry(QRect(minleft,topc,wLeft,gen_height));
      minleft += wLeft + spacing();
//      qDebug() << minleft;
    }
  }
#endif
  if (centralView) {
    centralView->w()->setGeometry(QRect(minleft,topc,newCWidth,gen_height));
    minleft += newCWidth + spacing();
//    qDebug() << minleft;
  }
#ifndef NVB_NO_FW_DOCKS
  if (haveright) {
    rightDock->setGeometry(QRect(minleft,topc,wRight,gen_height));
    minleft += wRight;
//    qDebug() << minleft;
  }
#endif
}

#ifndef NVB_NO_FW_DOCKS
void NVBFileWindowLayout::setLeftDock(QWidget * dock)
{
  leftDock = dock;
  insertWidget(1,dock);
}

void NVBFileWindowLayout::setRightDock(QWidget * dock)
{
  rightDock = dock;
  insertWidget(3,dock);
}
#endif

void NVBFileWindowLayout::setToolBar(QLayoutItem * toolbar)
{
  toolBar = toolbar;
  insertItem(0,toolbar);
}

void NVBFileWindowLayout::setView(NVBDataView * view)
{
  centralView = view;
  active = centralView->hasWidthForHeight();
	insertWidget(2,view->w(),1);
}
