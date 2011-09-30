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
class QKeyEvent;

class NVBDirView : public QAbstractItemView
{
Q_OBJECT
private:
/// Index of the file on top of the view
  int top_row;
/// Vertical coordinate (in pixels) of topleft corner of \a top_row in view
	int soft_shift;
/// Maximum number of pages horizontally
  int pages_per_row;
/// The size of the grid
  QSize gridSize;
/// cached vertical offset
	int voffset;

/// The view will try to keep current visible items in the viewport if rows are inserted or removed above
/**
 * If true, the view will keep the top row in the view on any model change (except deleting this row)
 * If false, the view will keep voffset constant on any model change (except deleting all rows below the top)
 */
	bool keepItemsOnModelChanges;
	
/// Number of rows before \a top_row with corresponding number of pages
	mutable QVector<int> counts_above;
/// Total number of rows with corresponding number of pages
	mutable QVector<int> counts_total;

/// Total visual height of all items in the view, not including midMargin() at every end. For an empty view is equal to 0
	int totalHeight();
  void calculateVOffset();
	void updateTopRow(int rold, int rnew);

	
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
	virtual void keyPressEvent(QKeyEvent *event);

  virtual int horizontalOffset () const { return 0; } // Always wrapped
  virtual inline int verticalOffset () const { return voffset; }
  virtual bool isIndexHidden(const QModelIndex & /*index*/) const { return false; }
  virtual QModelIndex moveCursor ( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
  virtual void setSelection ( const QRect & rect, QItemSelectionModel::SelectionFlags flags );
  virtual QRegion visualRegionForSelection ( const QItemSelection & selection ) const;
  virtual void scrollContentsBy(int dx, int dy);

private:

  QRect visualRect (int index) const;
//   int fileVOffset(int index) const;
/// Returns height (in pixels) of the file at \a index. Height is counted from header to bottom row of items (without midMargin())
  int fileHeight(int index) const;
/// Distance in pixels between \a index1 and \a index2
  int fileDistance(int index1, int index2) const;

/// Draws the header - a horizontal line with filename in the middle
  void drawHeader(int index, int y, QPainter * painter) const;
/// Draws all visible items of file at \a index
  void drawItems(int index, int y, QPainter* painter) const;

/// Height of the header (equal to text height)
	inline int headerHeight() const { return style()->pixelMetric(QStyle::PM_TitleBarHeight); }
/// Distance between the header and the first grid line (items)
	inline int headerMargin() const { return 10; }
/// Distance between the last grid line (items) and next header
/**
	* This distance is inserted before the first file, between files and also after the file/page to be positioned at bottom
	*/
	inline int midMargin() const { return 15; }
/// Distance between the left border and the first grid line
	inline int leftMargin() const { return 10; }
/// Distance between the last grid line and the right border
	inline int rightMargin() const { return 10; }

	inline int viewportHeight() const { return viewport()->height() - 2*midMargin(); }

private slots:
	void updateScrollBars();
	void invalidateCache();
	virtual void	rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
	virtual void	rowsRemoved ( const QModelIndex & parent, int start, int end );
//	virtual void	rowsAboutToBeInserted ( const QModelIndex & parent, int start, int end );
	virtual void	rowsInserted ( const QModelIndex & parent, int start, int end );
	
//signals:
//  void dataWindow(int start, int end);

//   QRect itemRect(const QModelIndex & index);
};

#endif
