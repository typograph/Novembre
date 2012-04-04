#include "NVB2DPageView.h"
#include <QIcon>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
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
  connect(vizmodel,SIGNAL(itemsSwapped(int,int)),SLOT(swapItems(int,int)));

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
  
	tBar->addAction(QIcon(_2Dview_export),"Export view",this,SLOT(exportImage()));
	act = tBar->addAction(QIcon(_2Dview_marker),"Show scale",this,SLOT(setShowMarker(bool)));
	act->setCheckable(true);
	act->setChecked(false);

  return tBar;
}

void NVB2DPageView::setShowMarker(bool show) {
	if (eventCatcher) {
		eventCatcher->showSizeMarker(show);
		if (scene()) scene()->update();
		}
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


void NVB2DPageView::rowsAboutToBeRemoved( const QModelIndex &, int start, int end )
{
  if (currentIndex >= start && currentIndex <= end && activeViz.valid != 0)
    emit activeVizEOL();
	for (int i = start; i <= end; i++)
		theScene->removeItem(getVizAt(i).TwoDViz);
  foreach(QGraphicsItem* item, theScene->items()) {
    if (item->zValue() <= -end-1)
      item->setZValue(item->zValue()+end-start+1);
		else if (item->zValue() <= -start)
			theScene->removeItem(item);
    }
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
//       tmp.TwoDViz->setSelected(false);
			tmp.TwoDViz->setZValue(-i);
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

NVBVizUnion NVB2DPageView::getVizAt(int row)
{
  NVBVizUnion tmp = vizmodel->index(row).data(PageVizItemRole).value<NVBVizUnion>();
  if (tmp.valid && tmp.vtype == NVB::TwoDView)
    return tmp;
  else
    return NVBVizUnion();
}

NVBVizUnion NVB2DPageView::getVizFromSelection()
{
	return getVizAt(currentIndex);
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
	sl << "All files (*.*)";
  foreach (QByteArray b, QImageWriter::supportedImageFormats()) {
    QString a(b);
    sl << a.toUpper() + " files (*." + a + ")";
    };

	QString selected = "All files (*.*)";
	QString filename = QFileDialog::getSaveFileName( 0, "Export image", QString(), sl.join(";;"),&selected);

  if (filename.isEmpty()) return;
  
  QImage i(1024,1024,QImage::Format_RGB32);
  QPainter p(&i);
	scene()->render(&p,QRectF(),sceneRect());
	p.end();
	if (i.save(filename)) return;
	int ix = sl.indexOf(selected);
	if (ix > 0 && i.save(filename + '.' + QImageWriter::supportedImageFormats().at(ix-1))) return;

	QMessageBox::warning(this,"Image export failed","Couldn't save " + filename);

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

void NVB2DPageView::swapItems(int row1, int row2)
{
	// Now we have to remember that the items have been swapped already in the model
	NVBVizUnion v1 = getVizAt(row1), v2 = getVizAt(row2);
	// The conditions are checking for raised items
	if (v1.TwoDViz->zValue() == -row2)
		v1.TwoDViz->setZValue(-row1);
	if (v2.TwoDViz->zValue() == -row1)
		v2.TwoDViz->setZValue(-row2);
}
