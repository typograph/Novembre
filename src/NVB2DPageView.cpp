#include "NVB2DPageView.h"
#include <QIcon>
#include <QFileDialog>
#include <QImageWriter>
#include "../icons/icons_2Dview.xpm"

NVB2DPageView::NVB2DPageView(NVBVizModel* model, QWidget * parent): QGraphicsView(parent),keepRatio(true),activeFilter(0),vizmodel(model),currentIndex(-1),activeViz(NVBVizUnion())
{
//  connect(this,SIGNAL(scalerChanged(Q2DScaler*)),&debugger,SLOT(printScalerInfo(Q2DScaler*)));
//  connect(this,SIGNAL(scalerChanged(Q2DScaler*)),this,SLOT(normalizeScaler()));

  theScene = new QGraphicsScene(this);
  setScene(theScene);

// By default QGraphicsView accepts drops, and thus does not accept NVBMimeData. Here we stop that.
  setAcceptDrops(false);

  theScene->addItem(eventCatcher = new NVBFullGraphicsItem());
  eventCatcher->setVisible(true);

//   connect(theScene,SIGNAL(changed(const QList<QRectF> &)),SLOT(checkForDesactivatedVizs()));
  connect(theScene,SIGNAL(changed(const QList<QRectF> &)),SLOT(rebuildRect()));

//  theScene->installEventFilter(this);

  setVerticalScrollBarPolicy(  Qt::ScrollBarAlwaysOff );
  setHorizontalScrollBarPolicy(  Qt::ScrollBarAlwaysOff );

  if (vizmodel->rowCount() > 1)
    rowsInserted(QModelIndex(),1,vizmodel->rowCount()-1);

  connect(vizmodel,SIGNAL(rowsRemoved(const QModelIndex&,int,int)),SLOT(rowsRemoved(const QModelIndex&,int,int)));
  connect(vizmodel,SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,int,int)),SLOT(rowsAboutToBeRemoved(const QModelIndex&,int,int)));
  connect(vizmodel,SIGNAL(rowsInserted(const QModelIndex&,int,int)),SLOT(rowsInserted(const QModelIndex&,int,int)));
  connect(vizmodel,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),SLOT(updateVizs(const QModelIndex&,const QModelIndex&)));

//  setGeometry(QRect(geometry().topLeft(),QSize(400,400)));
//  setMinimumSize(QSize(64,64));
  QSizePolicy sp(QSizePolicy::Expanding,QSizePolicy::Expanding);
  sp.setHeightForWidth(true);
  setSizePolicy(sp);
  resize(256,256);
/*  zooming = false;
  keepRatio = true;
  showTicks = false;
  tickLength = 5;
  dragging = false;
  pagemodel = NULL;
  scaler.setTo(QRectF(rect()));*/
//  emit scalerChanged(&scaler);
//  setSelectionMode(QAbstractItemView::SingleSelection);
}

NVB2DPageView::~ NVB2DPageView( )
{
}

/*
void NVB2DPageView::resizeEvent( QResizeEvent * event )
{
  if (zooming) {
    QSizeF new_size = scaler.rectTo().size();
    new_size.scale(event->size(),keepRatio?(Qt::KeepAspectRatio):(Qt::IgnoreAspectRatio));
    scaler.setTo(QRectF(QPointF(),new_size));
    }
}
*/
QToolBar * NVB2DPageView::generateToolbar(QWidget * parent) const
{
  QToolBar * tBar = new QToolBar(parent);
  QAction * act;
  
/*
  act = tBar->addAction(QIcon(_2Dview_zoom),"Zoom view with window",this,SLOT(setZooming(bool)));
  act->setCheckable(true);
  act->setChecked(zooming);
  
  act = tBar->addAction(QIcon(_2Dview_keepar),"Keep aspect ratio",this,SLOT(setCarefulZooming(bool)));
  act->setCheckable(true);
  act->setChecked(keepRatio);
    
  tBar->addSeparator();
  
  act = tBar->addAction(QIcon(_2Dview_scale),"Show ticks",this,SLOT(setShowTicks(bool)));
  act->setCheckable(true);
  act->setChecked(showTicks);
    */ 
  
  act = tBar->addAction(QIcon(_2Dview_export),"Export view",this,SLOT(exportImage()));
  
  return tBar;
}

int NVB2DPageView::heightForWidth(int w) const
{
  if (keepRatio) {
    return (int) (w*itemsRect.height()/itemsRect.width());
    }
  else return -1;
}


bool NVB2DPageView::hasWidthForHeight() const
{
  return keepRatio;
}

int NVB2DPageView::widthForHeight(int h) const
{
  if (keepRatio) {
		return (int)((h*itemsRect.width())/itemsRect.height());
    }
  else return -1;
}


/*
void NVB2DPageView::setModel(QAbstractItemModel * model)
{
  if (pagemodel) delete pagemodel;
  pagemodel = (NVBPageModel*)model;
  QAbstractItemView::setModel(new NVBPageViewModel(pagemodel));
}
*/


void NVB2DPageView::rowsAboutToBeRemoved( const QModelIndex & parent, int start, int end )
{
  if (currentIndex >= start && currentIndex <= end && activeViz.valid != 0)
    emit activeVizEOL();
}


void NVB2DPageView::rowsRemoved( const QModelIndex & parent, int start, int end )
{
  Q_UNUSED(parent);
  if (currentIndex > end) currentIndex -= end - start + 1;
  else if (currentIndex >= start) currentIndex = -1;

  rebuildRect();
  autoFocus();
}

void NVB2DPageView::rowsInserted( const QModelIndex & parent, int start, int end )
{
  if (parent.isValid()) return;
//  int last = pagemodel->rowCount()-2-end+start;

  if (currentIndex >= start) currentIndex += end-start+1;

  foreach(QGraphicsItem* item, theScene->items()) {
    if (item->zValue() <= -start)
      item->setZValue(item->zValue()-end+start-1);
    }

//  scaler<int,qreal> z(start-1,end+1,);

  for (int i = start; i <= end; i++) {
    NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();
//    tmp->setZValue(z->scale(i));
    if (tmp.valid && tmp.vtype == NVB::TwoDView) {
      theScene->addItem(tmp.TwoDViz);
#if QT_VERSION >= 0x040500
      itemsRect |= tmp.TwoDViz->mapRectToScene(tmp.TwoDViz->boundingRect());
#else
      itemsRect |= tmp.TwoDViz->mapToScene(tmp.TwoDViz->boundingRect()).boundingRect();
#endif
      tmp.TwoDViz->setZValue(-i);
//       tmp.TwoDViz->setSelected(false);
      }
    }

  autoFocus();

//  show();
}

QSize NVB2DPageView::minimumSizeHint( ) const
{
//   if (scene()->items().count() > 1) // there are items other that the event catcher
//   else 
  return QSize(100,100);
}

/*
void NVB2DPageView::drawTicks( QPainter * painter )
{
  QRectF base = visibleArea().normalized();
  int orderW = (int)floor(log10(base.width()));
  int orderH = (int)floor(log10(base.height()));
  int mainW = (int)floor(base.width()/pow(10,orderW));
  int mainH = (int)floor(base.height()/pow(10,orderH));
  double stepW, stepH;
  if (mainW % 3 == 0) stepW = mainW/3;
  else stepW = mainW/2.0;
  if (mainH % 3 == 0) stepH = mainH/3;
  else stepH = mainH/2.0;
  int rx,ry;
  painter->save();
  painter->setPen(Qt::black);
  for (double x = ceil(base.x()/pow(10,orderW)); x <= base.right()/pow(10,orderW); x += stepW) {
    rx = (int)(scaler.XScaler().scale(x*pow(10,orderW)));
    painter->drawLine(rx,0,rx,tickLength);
    painter->drawLine(rx,height(),rx,height()-tickLength);
    }
  for (double y = ceil(base.y()/pow(10,orderH)); y <= base.bottom()/pow(10,orderH); y += stepH) {
    ry = (int)(scaler.YScaler().scale(y*pow(10,orderH)));
    painter->drawLine(0,ry,tickLength,ry);
    painter->drawLine(width(),ry,width()-tickLength,ry);
    }
  painter->restore();
}
*/
/*
QSize NVB2DPageView::minimalSizeHint( ) const
{
  if (!hasVisibleItems()) return QSize();
  QSize sz;
  for (int i = model()->rowCount()-1; i>=0; i--) {
    if (( NVB::PageType)(model()->index(i,0).data(PageTypeRole).toInt()) == NVB::TopoPage)
      sz.scale(model()->index(i,0).data(PageDataSizeRole).toSize(),Qt::IgnoreAspectRatio);
    }
  return sz;
}

QSize NVB2DPageView::sizeHint( ) const
{
  return QSize(scaler.rectTo().size().toSize());
}
*/

void NVB2DPageView::autoFocus()
{
  fitInView(itemsRect,Qt::KeepAspectRatio);
//  fitInView(theScene->itemsBoundingRect(),Qt::KeepAspectRatio);
}

void NVB2DPageView::resizeEvent(QResizeEvent * event)
{
  QGraphicsView::resizeEvent(event);
  autoFocus();
}

void NVB2DPageView::updateVizs(const QModelIndex & start, const QModelIndex & end)
{
  for (int i = start.row(); i <= end.row(); i++) {
    NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();
//    tmp->setZValue(z->scale(i));
    if (tmp.valid && tmp.vtype == NVB::TwoDView) {
      theScene->addItem(tmp.TwoDViz);
      tmp.TwoDViz->setZValue(-i);
//       tmp.TwoDViz->setSelected(false);
      }
    }
  rebuildRect();

  if (activeViz.valid == 0 && currentIndex >= start.row() && currentIndex <= end.row()) {
      activateViz(getVizFromSelection());
    }
  autoFocus();
}

void NVB2DPageView::select(const QModelIndex & index)
{
  if (!index.isValid()) return;
  if (index.row() == currentIndex) return;
  emit activeVizEOL();
//   removeActiveViz();
/*
#if QT_VERSION >= 0x040400
  QGraphicsItem * i = scene()->mouseGrabberItem();
  if (i) {
    i->ungrabMouse();
    i->ungrabKeyboard();
    }

  NVBVizUnion tmp = vizmodel->index(index.row()).data(PageVizItemRole).value<NVBVizUnion>();
  if (tmp.valid && tmp.vtype == NVB::TwoDView) {
    tmp.TwoDViz->grabMouse();
    tmp.TwoDViz->grabKeyboard();
    }
#else
  NVBVizUnion tmp = vizmodel->index(currentIndex).data(PageVizItemRole).value<NVBVizUnion>();
  if (tmp.valid && tmp.vtype == NVB::TwoDView) {
    tmp.TwoDViz->setZValue(-currentIndex);
    }
  tmp = vizmodel->index(index.row()).data(PageVizItemRole).value<NVBVizUnion>();
  if (tmp.valid && tmp.vtype == NVB::TwoDView) {
    tmp.TwoDViz->setZValue(1);
    }
#endif
*/
  currentIndex = index.row();
  activateViz(getVizFromSelection());
}

void NVB2DPageView::setActiveVisualizer(NVBVizUnion viz, const QModelIndex & index)
{
  if (!index.isValid()) return;

//   checkForDesactivatedVizs();
//   removeActiveViz();

  if (viz.valid == 0 || viz.vtype != NVB::TwoDView) return;

// This call will be followed by a call to removeActiveViz()
// in case the filter author doesn't do something wrong
  if (activeViz.valid != 0)
    emit activeVizEOL();

// Just checking for non-conforming authors
  Q_ASSERT_X(activeViz.valid == 0,"deactivating viz","Visualizer still valid after deactivation request");

  currentIndex = index.row(); // ###
  NVBVizUnion tmp = getVizFromSelection();
  if (tmp.valid) {
    tmp.TwoDViz->setZValue(1);
    viz.TwoDViz->setZValue(2);
    viz.TwoDViz->setVisible(true);
    scene()->addItem(viz.TwoDViz);
    if (viz.filter) {
      connect(this,SIGNAL(activeVizEOL()),viz.filter,SLOT(vizDeactivationRequest()));
      connect(viz.filter,SIGNAL(detach2DViz()),SLOT(removeActiveViz()));
      }
    else {
      connect(this,SIGNAL(activeVizEOL()),this,SLOT(removeActiveViz()));
      }
    activeViz = viz;
    activateViz(activeViz);
    }

}

void NVB2DPageView::removeActiveViz()
{
  if (activeViz.valid != 0) {
    deactivateFilter();

    activeViz.TwoDViz->setVisible(false);

    if (activeViz.TwoDViz->scene() == theScene) {
      theScene->removeItem(activeViz.TwoDViz);
      }

    if (activeViz.filter) { // Assuming the plugin never gets a pointer to a 2DView
      this->disconnect(activeViz.filter);
      activeViz.filter->disconnect(this);
      }
    else {// self-managed
      this->disconnect(this);
      delete activeViz.TwoDViz;
      }

    NVBVizUnion viz = getVizFromSelection();

    if (viz.valid != 0) {
      viz.TwoDViz->setZValue(-currentIndex);
      activateViz(viz);
      }

    activeViz = NVBVizUnion();
    }
}

// void NVB2DPageView::checkForDesactivatedVizs()
// {
//   if (externalVizActive && theScene->items().count() == vizmodel->rowCount()+1) {
//     activeViz = getVizFromSelection(); // if the item is not in the scene, it was probably already deleted.
//     activeFilter = 0;
//     activeViz = NVBVizUnion();
//     removeActiveViz();
//     }
// }

void NVB2DPageView::keyReleaseEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
    emit activeVizEOL();
    }
  else
    QGraphicsView::keyReleaseEvent(event);
}

/*
bool NVB2DPageView::viewportEvent(QEvent * event)
{
  if (event->type() == QEvent::KeyRelease) {
    QKeyEvent * e = (QKeyEvent*)event;
    if (e && e->key() == Qt::Key_Escape && e->modifiers() == Qt::NoModifier) {
      removeActiveViz();
      return true;
      }
    }
  return QGraphicsView::viewportEvent(event);
}
*/
bool NVB2DPageView::eventFilter(QObject * watched, QEvent * event)
{
  if (watched != scene()) return false;
  switch (event->type()) {
//     case QEvent::GraphicsSceneContextMenu :
//     case QEvent::GraphicsSceneDragEnter :
//     case QEvent::GraphicsSceneDragLeave :
//     case QEvent::GraphicsSceneDragMove :
//     case QEvent::GraphicsSceneDrop :
//     case QEvent::GraphicsSceneHelp :
    case QEvent::GraphicsSceneHoverEnter :
    case QEvent::GraphicsSceneHoverLeave :
    case QEvent::GraphicsSceneHoverMove :
    case QEvent::GraphicsSceneMouseDoubleClick :
    case QEvent::GraphicsSceneMouseMove :
    case QEvent::GraphicsSceneMousePress :
    case QEvent::GraphicsSceneMouseRelease :
//     case QEvent::GraphicsSceneMove :
//     case QEvent::GraphicsSceneResize :
    case QEvent::GraphicsSceneWheel: {
//       if (activeViz.valid) {
//         activeViz.TwoDViz->sceneEvent(event);
        return true;
//         }
      }
    default : return false;
    }
  return false;
}

NVBVizUnion NVB2DPageView::getVizFromSelection()
{
  NVBVizUnion tmp = vizmodel->index(currentIndex).data(PageVizItemRole).value<NVBVizUnion>();
  if (tmp.valid && tmp.vtype == NVB::TwoDView)
    return tmp;
  else
    return NVBVizUnion();
}

void NVB2DPageView::activateViz(NVBVizUnion viz)
{
//   deactivateFilter();
  if (viz.valid) {
//     activeViz.TwoDViz->setSelected(true);
    eventCatcher->installSceneEventFilter(viz.TwoDViz);
    eventCatcher->setCursor(viz.TwoDViz->cursor());
    activeFilter = viz.TwoDViz;
    }
}

void NVB2DPageView::deactivateFilter()
{
  if (activeFilter) {
    eventCatcher->removeSceneEventFilter(activeFilter);
    eventCatcher->unsetCursor();
    activeFilter = 0;
    }
}

void NVB2DPageView::exportImage( )
{
  QStringList sl;
  foreach (QByteArray b, QImageWriter::supportedImageFormats()) {
    QString a(b);
    sl << a.toUpper() + " files (*." + a + ")";
    };

  QString filename = QFileDialog::getSaveFileName( 0, "Export image", QString(), sl.join(";;"));
  
  if (filename.isEmpty()) return;
  
  QImage i(1024,1024,QImage::Format_RGB32);
  QPainter p(&i);
  scene()->render(&p);
  p.end();
  i.save(filename);
}

void NVB2DPageView::rebuildRect( )
{
  itemsRect = QRectF();
  for (int i = 0; i < vizmodel->rowCount();i++) {
    NVBVizUnion tmp = vizmodel->index(i).data(PageVizItemRole).value<NVBVizUnion>();
    if (tmp.valid && tmp.vtype == NVB::TwoDView)
#if QT_VERSION >= 0x040500      
      itemsRect |= tmp.TwoDViz->mapRectToScene(tmp.TwoDViz->boundingRect());
#else
      itemsRect |= tmp.TwoDViz->mapToScene(tmp.TwoDViz->boundingRect()).boundingRect();
#endif
    }
}

