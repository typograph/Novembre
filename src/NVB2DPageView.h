#ifndef NVBPAGEVIEW_H
#define NVBPAGEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSizePolicy>
#include <QResizeEvent>
#include "NVBPageViewModel.h"
#include "NVBVizModel.h"
#include "NVBLogger.h"
#include "NVBGraphicsItems.h"
#include "NVBDataView.h"

class QRubberBand;

class NVB2DPageView : public QGraphicsView, public NVBDataView {
Q_OBJECT
private:

/*  bool dragging;
	QPoint dragStart;*/
	bool paintSizeMarker;
	bool zooming;
  bool keepRatio;
	QPoint zoomRCpos; // right-click spot
	QRubberBand * zoomRubberBand;
  QGraphicsItem * activeFilter;
  NVBFullGraphicsItem * eventCatcher;
/*  bool showTicks;
  uchar tickLength; */
  QGraphicsScene * theScene;
  NVBVizModel * vizmodel;
  int currentIndex;
  NVBVizUnion activeViz ;
  bool externalVizActive;
  QRectF itemsRect; // the actual bounding rect of all pages in the scene
	QRectF zoomRect;  // the rect we plan to show
/*
  NVB2DViewDelegate * topoDlg;
  NVB2DSpecViewDelegate * specDlg;
  NVB2DDummyDelegate * dummyDlg;
  NVBToolsFactory * tools;
*/

  void activateSelectedViz();
  void activateViz(NVBVizUnion viz);
  void deactivateFilter();
  NVBVizUnion getVizAt(int row);
  NVBVizUnion getVizFromSelection();
public:
  NVB2DPageView(NVBVizModel* model, QWidget * parent = 0);
  ~NVB2DPageView();

//  virtual void setModel ( QAbstractItemModel * model );


  QToolBar * generateToolbar(QWidget * parent) const;
  QAbstractListModel * getVizModel() { return vizmodel; }

//   void show(const QModelIndex & index);
//   void showOnly(const QModelIndex & index);
//   void showAll();
//   void hide(const QModelIndex & index);
//   void hideAll();
//   bool hasVisibleItems() const;

/*
  void levelUp(QModelIndex & index);
  void levelDown(QModelIndex & index);
  void levelTop(QModelIndex & index);
  void levelBottom(QModelIndex & index);
*/

//   QModelIndex createNewPage( NVB::PageType type);
//   void setDelegate(NVBGeneralDelegate * delegate);

//   virtual void resizeEvent ( QResizeEvent * event );
//   virtual void paintEvent ( QPaintEvent * event );
//   virtual void keyPressEvent ( QKeyEvent * event ); 
//   virtual void mouseDoubleClickEvent ( QMouseEvent * event ); 
//   virtual void mouseMoveEvent ( QMouseEvent * event );
//   virtual void mousePressEvent ( QMouseEvent * event );
//   virtual void mouseReleaseEvent ( QMouseEvent * event );

  virtual bool eventFilter ( QObject * watched, QEvent * event );
  virtual QSize minimumSizeHint() const;
  virtual int heightForWidth ( int w ) const;

  virtual bool hasWidthForHeight() const;
  virtual int  widthForHeight(int h) const;

	virtual bool acceptsPage(NVBDataSource * /*source*/) const { return true; }

  virtual const QWidget * w() const { return this; }
  virtual QWidget * w() { return this; }

public slots :
  void select( const QModelIndex & index);
  void setActiveVisualizer( NVBVizUnion, const QModelIndex & );
	void setShowMarker(bool show) { paintSizeMarker = show; viewport()->update(); }

/// Connect to this slot to tell the view the active visualizer is not needed.
/// Delete the visualizer afterwards
  void removeActiveViz();
  
  void exportImage();
//   void setMouseCursor(const QCursor & cursor);
//  void enableZooming();
	void setZooming(bool _zooming) {zooming = _zooming;}
//   void setCarefulZooming(bool _keepRatio) { keepRatio = _keepRatio; }
//   void setShowTicks(bool _showTicks) { showTicks = _showTicks; update(); }
	/// Swaps graphic items at row1 and row2
	void swapItems(int row1, int row2);

protected :
//   QRectF visibleArea();
//  void drawTicks(QPainter * painter);
	virtual void paintEvent ( QPaintEvent * event );

	virtual void wheelEvent( QWheelEvent *event );

  virtual void resizeEvent ( QResizeEvent * event );
  virtual void keyReleaseEvent ( QKeyEvent * event );

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
//   virtual bool viewportEvent ( QEvent * event );

protected slots:
  virtual void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
  virtual void rowsRemoved ( const QModelIndex & parent, int start, int end );
  virtual void rowsInserted ( const QModelIndex & parent, int start, int end );
  virtual void updateVizs(const QModelIndex& start, const QModelIndex& end);
//   virtual void checkForDesactivatedVizs();

  void rebuildRect();
  void autoFocus();
//   virtual void normalizeScaler();
//   virtual void resetScaler();
signals :

/// The view emits this signal when the active visualizer has to be deactivated.
/// The filter has to call (deactivateControlViz)
  void activeVizEOL();
//   virtual void scalerChanged(Q2DScaler * );

};

#endif
