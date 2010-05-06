//
// C++ Interface: NVBDirView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NVBDIRVIEW_H
#define NVBDIRVIEW_H

#include <QAbstractItemView>
#include <QRect>
#include <QVector>

class QPaintEvent;
class QResizeEvent;

class NVBDirView : public QAbstractItemView
{
Q_OBJECT
private:
  int top_row;
  int gwidth;
  int pages_per_row;
  QSize gridSize;

  mutable QVector<int> heights;
//   mutable QVector<int> offsets;

public:
  NVBDirView( QWidget * parent = 0 );
  virtual ~NVBDirView();

  virtual QModelIndex indexAt ( const QPoint & point ) const;
  virtual void scrollTo ( const QModelIndex & index, ScrollHint hint = EnsureVisible );
  virtual QRect visualRect ( const QModelIndex & index ) const;

  void setGridSize(QSize s);

  virtual void setModel(QAbstractItemModel * m);

protected:

  virtual void paintEvent(QPaintEvent *e);
  virtual void resizeEvent ( QResizeEvent * event );

  virtual int horizontalOffset () const { return 0; } // Always wrapped
  virtual int verticalOffset () const;
  virtual bool isIndexHidden(const QModelIndex & /*index*/) const { return false; }
  virtual QModelIndex moveCursor ( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
  virtual void setSelection ( const QRect & rect, QItemSelectionModel::SelectionFlags flags );
  virtual QRegion visualRegionForSelection ( const QItemSelection & selection ) const;
  virtual void scrollContentsBy(int dx, int dy);

  int bottom_row();

private:

  QRect visualRect (int index) const;
//   int fileVOffset(int index) const;
  int fileHeight(int index) const;
  inline int fileHeightLite(int npages) const;
  int fileDistance(int index1, int index2) const;

  void drawHeader(int index, QPainter * painter) const;
  void drawItems(int index, QPainter * painter) const;

  inline int headerHeight() const;
  inline int topMargin() const;
  inline int midMargin() const;
  inline int btmMargin() const;
  inline int leftMargin() const;
  inline int rightMargin() const;

private slots:
  void updateScrollBars();

signals:
  void dataWindow(int start, int end);

//   QRect itemRect(const QModelIndex & index);
};

#endif
