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
/// Index of the file on top of the view
  int top_row;
/// Maximum width of the grid (page per row * grid cell width)
  int gwidth;
/// Maximum number of pager horizontally
  int pages_per_row;
/// The size of the grid
  QSize gridSize;

/// Pixel height of each file (changes with pages_per_row)
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

/// Index of the last visible model row
  int bottom_row();

private:

  QRect visualRect (int index) const;
//   int fileVOffset(int index) const;
/// Returns height (in pixels) of the file at \a index
  int fileHeight(int index) const;
/// Cache-using version of \fn fileHeight
	int fileHeightLite(int npages) const;
/// Distance in pixels between \a index1 and \a index2
  int fileDistance(int index1, int index2) const;

/// Draws the header - a horizontal line with filename in the middle
  void drawHeader(int index, QPainter * painter) const;
/// Draws all visible items of file at \a index
  void drawItems(int index, QPainter * painter) const;

/// Height of the header (equal to text height)
	inline int headerHeight() const { return style()->pixelMetric(QStyle::PM_TitleBarHeight); }
/// Distance between the top of the view / bottom of previous item and the header
	inline int topMargin() const { return 3; }
/// Distance between the header and the first grid line (items)
	inline int midMargin() const { return 5; }
/// Distance between the last row of items and the next file
	inline int btmMargin() const { return 3; }
/// Distance between the left border and the first grid line
	inline int leftMargin() const { return 10; }
/// Distance between the last grid line and the right border
	inline int rightMargin() const { return 10; }

private slots:
  void updateScrollBars();

signals:
  void dataWindow(int start, int end);

//   QRect itemRect(const QModelIndex & index);
};

#endif
