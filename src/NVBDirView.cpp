//
// C++ Implementation: NVBDirView
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "NVBLogger.h"
#include "NVBDirView.h"
#include <QPaintEvent>
#include <QRegion>
#include <QRect>
#include <QPoint>
#include <QWidget>
#include <QPainter>
#if QT_VERSION < 0x040300
#include <QStyleOptionViewItemV2>
#elif QT_VERSION < 0x040400
#include <QStyleOptionViewItemV3>
#else
#include <QStyleOptionViewItemV4>
#endif
#include <QScrollBar>

NVBDirView::NVBDirView(QWidget * parent):QAbstractItemView(parent)
{
  top_row = 0;
  gwidth = 0;
  pages_per_row = 0;
  heights.resize(10);
  setGridSize(iconSize()+QSize(20,20));
}

NVBDirView::~ NVBDirView()
{
}

QModelIndex NVBDirView::indexAt(const QPoint & point) const
{
  if (!model())
    return QModelIndex();

  if ( point.x() <= leftMargin() || point.x() > leftMargin() + gwidth)
    return QModelIndex();

  // Using fileDistance here is too expensive
	if ( point.y() <= topMargin())// || point.y() > (model()->rowCount() - top_row)*gridSize.height())
    return QModelIndex();

  int irow = top_row;
  int inside_y = point.y();

  while (inside_y > fileHeight(irow))
    inside_y -= fileHeight(irow++);

	if (irow >= model()->rowCount())
		return QModelIndex();

  inside_y -= topMargin();
  if (inside_y < 0)
    return QModelIndex();

  inside_y -= headerHeight();
  if (inside_y < 0)
    return model()->index(irow,0);

  inside_y -= midMargin();
  if (inside_y < 0)
    return QModelIndex();

	int col = (point.x()-leftMargin())/gridSize.width();
	int row = inside_y/gridSize.height();

	return model()->index(col+row*pages_per_row,0,model()->index(irow,0));

}

void NVBDirView::scrollTo(const QModelIndex & index, ScrollHint hint)
{
  if (!model() && !index.isValid())
    return;

  int new_top_row = top_row;

  QModelIndex oindex = index;
  if (oindex.parent().isValid())
    oindex = oindex.parent();

  switch (hint) {
    case QAbstractItemView::PositionAtTop : {
      new_top_row = oindex.row();
      setDirtyRegion(viewport()->rect());
      break;
      }
    case QAbstractItemView::PositionAtBottom : {
      new_top_row = oindex.row();
      if (new_top_row) {
        int th = fileHeight(new_top_row) + fileHeight(new_top_row-1);
        // The following is black magic of operator precedence
        while (th <= viewport()->height() && --new_top_row > 0)
          th += fileHeight(new_top_row-1);
        }
      setDirtyRegion(viewport()->rect());
      break;
      }
    case QAbstractItemView::PositionAtCenter : {
      new_top_row = oindex.row();
      if (new_top_row) {
        int th = fileHeight(new_top_row-1);
        while (th <= viewport()->height()/2 && new_top_row > 0)
          th += fileHeight(--new_top_row-1);
        }
      setDirtyRegion(viewport()->rect());
      break;
      }
    case QAbstractItemView::EnsureVisible : {
      if (oindex.row() < new_top_row)
        scrollTo(oindex,QAbstractItemView::PositionAtTop);
//       else if (fileVOffset(oindex.row())+fileHeight(oindex.row()) > vpos + viewport()->height())
      else if (fileDistance(oindex.row()+1, new_top_row) > viewport()->height())
        scrollTo(oindex,QAbstractItemView::PositionAtBottom);
      new_top_row = top_row;
      break;
      }
    }
  verticalScrollBar()->setValue(new_top_row);
}

QRect NVBDirView::visualRect(const QModelIndex & index) const
{
  if (!index.isValid()) return QRect();
  if (index.parent().isValid()) {
    if (pages_per_row < 1)
      return QRect();
    return QRect(
            QPoint(
              leftMargin() + gridSize.width()*(index.row()%pages_per_row),
              fileDistance(index.parent().row(), top_row) + topMargin() + headerHeight() + midMargin() + gridSize.height()*(index.row()/pages_per_row)
              )
            ,gridSize
            );
    }
  else
    return QRect(0,fileDistance(index.row(),top_row)+topMargin(),viewport()->width(),headerHeight());
}

QRect NVBDirView::visualRect(int index) const
{
  return QRect(0,fileDistance(index,top_row),viewport()->width(),fileHeight(index));
}

// int NVBDirView::fileVOffset(int index) const
// {
//   if (offsets.at(index) >= 0)
//     return offsets.at(index);
//   
//   if (offsets.first() < 0) {
//     offsets[0] = 0;
//     for (int i = 0; i < index; i++)
//       offsets[i+1] = offsets.at(i) + fileHeight(i);
//     }
//   else 
//     offsets[index] = fileHeight(index-1) + fileVOffset(index-1);
//   return offsets.at(index);
// }

int NVBDirView::fileHeight(int index) const
{
  return fileHeightLite(pages_per_row ? (model()->rowCount(model()->index(index,0)) + pages_per_row - 1) / pages_per_row : 0);
/*  if (heights.at(index) < 0) {
    if (gwidth < gridSize.width())
      heights[index] = topMargin() + headerHeight() + btmMargin();
    else
      heights[index] = topMargin() + headerHeight() + midMargin() + gridSize.height() * (1 + (model()->rowCount(model()->index(index,0)) - 1) / ( gwidth / gridSize.width() ) ) + btmMargin();
    }
  return heights.at(index);*/
}

QModelIndex NVBDirView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/)
{
  if (!currentIndex().isValid())
    return model()->index(0,0,model()->index(0,0));

  QModelIndex cindex = currentIndex();
  QModelIndex pindex = cindex.parent();

  switch (cursorAction) {
    case QAbstractItemView::MoveUp : {
      if ( cindex.row() < pages_per_row ) {
        if (pindex.row() == 0)
          return cindex;
        QModelIndex up = model()->index(pindex.row()-1,0);
        while (model()->rowCount(up) == 0 && up.row() > 0)
          up = model()->index(up.row()-1,0);
        if (model()->rowCount(up) == 0) return QModelIndex();
        if (cindex.row() >= model()->rowCount(up) - pages_per_row*((model()->rowCount(up)-1)/pages_per_row))
          return model()->index(model()->rowCount(up)-1,0,up);
        else
          return model()->index(pages_per_row*((model()->rowCount(up)-1)/pages_per_row) + cindex.row(),0,up);
        }
      else
        return model()->index(cindex.row() - pages_per_row,0,pindex);
      }
    case QAbstractItemView::MoveDown : {
      if ( cindex.row() + pages_per_row >= model()->rowCount(pindex) ) {
        if (pindex.row() == model()->rowCount()-1)
          return cindex;
        QModelIndex down = model()->index(pindex.row()+1,0);
        while (model()->rowCount(down) == 0 && down.row() < model()->rowCount()-1)
          down = model()->index(down.row()+1,0);
        if (model()->rowCount(down) == 0) return QModelIndex();
        if (model()->rowCount(down) <= cindex.row()%pages_per_row)
          return model()->index(model()->rowCount(down)-1,0,down);
        else
          return model()->index(cindex.row()%pages_per_row,0,down);
        }
      else
        return model()->index(cindex.row() + pages_per_row,0,pindex);
      }
    case QAbstractItemView::MoveLeft : {
      if (cindex.row() == 0)
        return cindex;
      return model()->index(cindex.row()-1,0,pindex);
      }
    case QAbstractItemView::MoveRight : {
      if (cindex.row() == model()->rowCount(pindex)-1)
        return cindex;
      return model()->index(cindex.row()+1,0,pindex);
      }
    case QAbstractItemView::MoveHome : {
      return model()->index(0,0,model()->index(0,0));
      }
    case QAbstractItemView::MoveEnd : {
      return model()->index(0,0,model()->index(model()->rowCount()-1,0));
      }
    case QAbstractItemView::MovePageUp : {
      if (pindex.row() == 0)
        return cindex;
      return model()->index(0,0,model()->index(pindex.row()-1,0));
      break;
      }
    case QAbstractItemView::MovePageDown : {
      if (pindex.row() == model()->rowCount() - 1)
        return cindex;
      return model()->index(0,0,model()->index(pindex.row()+1,0));
      break;
      }
    case QAbstractItemView::MoveNext : {
      if (cindex.row() == model()->rowCount(pindex)-1) {
        if (pindex.row() == model()->rowCount()-1)
          return cindex;
        QModelIndex down = model()->index(pindex.row()+1,0);
        return model()->index(0,0,down);
        }
      return model()->index(cindex.row()+1,0,pindex);
      }
    case QAbstractItemView::MovePrevious : {
      if (cindex.row() == 0) {
        if (pindex.row() == 0)
          return cindex;
        QModelIndex up = model()->index(pindex.row()-1,0);
        return model()->index(model()->rowCount(up)-1,0,up);
        }
      return model()->index(cindex.row()-1,0,pindex);
      }
    }
  return QModelIndex();
}

QRegion NVBDirView::visualRegionForSelection(const QItemSelection & selection) const
{
  if (selection.indexes().isEmpty())
    return QRegion();
  return visualRect(selection.indexes().first());
}

void NVBDirView::setSelection(const QRect & rect, QItemSelectionModel::SelectionFlags /*flags*/)
{
  setCurrentIndex(indexAt(rect.topLeft()));
}

void NVBDirView::paintEvent(QPaintEvent * e)
{
  if (!model())
    return;
	if (top_row < 0)
		return;

  QPainter painter(viewport());

// Find the first visible row that intersects with e->rect()

  int rendered_row = top_row;
  int row_bottom = fileHeight(rendered_row);
  while (rendered_row < model()->rowCount() && row_bottom <= e->rect().top()) row_bottom += fileHeight(++rendered_row);

  int row_top = row_bottom - fileHeight(rendered_row);

// Render down to the last visible row that intersects with e->rect()

  while (rendered_row < model()->rowCount() && row_top < e->rect().bottom()) {
    // TODO Draw alternating background
    drawHeader(rendered_row,&painter);
    if (gwidth >= gridSize.width())
      drawItems(rendered_row,&painter);
    row_top += fileHeight(rendered_row++);
  }

}

#include <QStyleOptionTitleBar>
#include <QFont>

void NVBDirView::drawHeader(int index, QPainter * painter) const
{
  int t = fileDistance(index,top_row) + topMargin();

	NVBOutputDMsg("<>");

  painter->setBrush(QBrush(Qt::black));
  painter->setPen(QPen(painter->brush(),style()->pixelMetric(QStyle::PM_DefaultFrameWidth),Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));

  QFont f = painter->font();
  f.setWeight(QFont::Bold);
  painter->setFont(f);

  QRect hrect = QRect(leftMargin(),t,viewport()->width()-leftMargin()-rightMargin(),headerHeight());

  style()->drawItemText(painter,hrect,Qt::AlignCenter,palette(),true,model()->index(index,0).data(Qt::DisplayRole).toString(),QPalette::WindowText);

  QRect trect = style()->itemTextRect(painter->fontMetrics(), hrect, Qt::AlignCenter, true, model()->index(index,0).data(Qt::DisplayRole).toString());
  QLine hline = QLine(trect.right() + leftMargin(),trect.center().y(),hrect.right(),trect.center().y());
  painter->drawLine(hline);
  hline = QLine(leftMargin(),trect.center().y(),trect.left()-rightMargin(),trect.center().y());
  painter->drawLine(hline);

}

void NVBDirView::drawItems(int index, QPainter * painter) const 
{
	NVBOutputDMsg(QString::number(index));

  int nitems = model()->rowCount(model()->index(index,0));
//   QStyleOptionViewItem option = ;
#if QT_VERSION < 0x040300
  QStyleOptionViewItemV2
#elif QT_VERSION < 0x040400
  QStyleOptionViewItemV3
#else
  QStyleOptionViewItemV4
#endif
    option = viewOptions();
  option.features |= QStyleOptionViewItemV2::WrapText; // | QStyleOptionViewItemV2::HasDisplay | QStyleOptionViewItemV2::HasDecoration;
#if QT_VERSION >= 0x040300
  option.locale = locale();
  option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
  option.widget = this;
#endif  
  option.decorationPosition = QStyleOptionViewItem::Top;
  option.displayAlignment = Qt::AlignCenter;
  option.showDecorationSelected = false;
//   option.state = state();

  for (int i=0; i<nitems; i++) {
    QModelIndex dindex = model()->index(i,0,model()->index(index,0));
    option.rect = visualRect(dindex);

    if (dindex == currentIndex())
      option.state |= QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active;
    else
      option.state &= ~( QStyle::State_Selected | QStyle::State_HasFocus | QStyle::State_Active );

    itemDelegate()->paint(painter, option, dindex);


/*
    if (selections && selections->isSelected(*it))
      option.state |= QStyle::State_Selected;
    if (isEnabled()) {
      QPalette::ColorGroup cg;
      if ((itemmodel()->flags(*it) & Qt::ItemIsEnabled) == 0) {
          option.state &= ~QStyle::State_Enabled;
          cg = QPalette::Disabled;
        }
      else {
          cg = QPalette::Normal;
        }
      option.palette.setCurrentColorGroup(cg);
      }
      if (focus && current == *it) {
          option.state |= QStyle::State_HasFocus;
          if (viewState == EditingState)
              option.state |= QStyle::State_Editing;
      }
      if (*it == hover)
          option.state |= QStyle::State_MouseOver;
      else
          option.state &= ~QStyle::State_MouseOver;

      if (alternate) {
          int row = (*it).row();
          if (row != previousRow + 1) {
              // adjust alternateBase according to rows in the "gap"
              if (!d->hiddenRows.isEmpty()) {
                  for (int r = qMax(previousRow + 1, 0); r < row; ++r) {
                      if (!d->hiddenRows.contains(r))
                          alternateBase = !alternateBase;
                  }
              } else {
                  alternateBase = (row & 1) != 0;
              }
          }
          if (alternateBase) {
              option.features |= QStyleOptionViewItemV2::Alternate;
          } else {
              option.features &= ~QStyleOptionViewItemV2::Alternate;
          }

          // draw background of the item (only alternate row). rest of the background
          // is provided by the delegate
          QStyle::State oldState = option.state;
          option.state &= ~QStyle::State_Selected;
          style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, &painter, this);
          option.state = oldState;

          alternateBase = !alternateBase;
          previousRow = row;
      }

      if (const QWidget *widget = d->editorForIndex(*it)) {
          QRegion itemGeometry(option.rect);
          QRegion widgetGeometry(widget->geometry());
          painter.save();
          painter.setClipRegion(itemGeometry.subtracted(widgetGeometry));
          d->delegateForIndex(*it)->paint(&painter, option, *it);
          painter.restore();
      } else {
          d->delegateForIndex(*it)->paint(&painter, option, *it);
      }

*/

    }
}

void NVBDirView::setModel(QAbstractItemModel * m)
{
  verticalScrollBar()->setRange(0, -1);
  if (m == 0) {
    top_row = 0;
    QAbstractItemView::setModel(m);
    updateScrollBars();
    }
  else {
    QAbstractItemView::setModel(m);
    top_row = 0;
//     offsets.resize(heights.size()+1);
    connect(m,SIGNAL(layoutChanged()),this,SLOT(updateScrollBars()));
		connect(m,SIGNAL(modelReset()),this,SLOT(updateScrollBars()));
		connect(m,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(updateScrollBars()));
    connect(m,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(updateScrollBars()));
    updateScrollBars();
    }
}

void NVBDirView::resizeEvent(QResizeEvent * event)
{
  pages_per_row = (viewport()->width()-leftMargin()-rightMargin())/gridSize.width();
  gwidth = gridSize.width()*pages_per_row;

//   gwidth = gridSize.width()*((event->size().width()-leftMargin()-rightMargin())/gridSize.width());
  updateScrollBars();
  QAbstractItemView::resizeEvent(event);
}

void NVBDirView::setGridSize(QSize s)
{
  gridSize = s;
  
  int ogwidth = gwidth;
  pages_per_row = (viewport()->width()-leftMargin()-rightMargin())/gridSize.width();
  gwidth = gridSize.width()*pages_per_row;
  
  if (ogwidth != gwidth) {
    int h = topMargin() + headerHeight() + btmMargin();
    for (int i = 0; i < 10; i++) {
      heights[i] = h+i*gridSize.height();
      }
    }
  
  setMinimumSize(1 + leftMargin() + gridSize.width() + rightMargin(), 1 + topMargin() + headerHeight() + midMargin() + gridSize.height() + btmMargin());
  
  updateScrollBars();

  scheduleDelayedItemsLayout();
}

void NVBDirView::scrollContentsBy(int dx, int dy)
{
  if (dy != 0 && model()) {
    top_row = qMin(qMax(top_row-dy,0),model()->rowCount()-1);
    emit dataWindow(top_row,bottom_row());
    setDirtyRegion(viewport()->rect());
    }
  QAbstractItemView::scrollContentsBy(dx,dy);
}

/**
	* \fn NVBDirView::headerHeight
	*
	* \returns Height of the header (equal to text height)
	*/

int NVBDirView::headerHeight() const
{
  return style()->pixelMetric(QStyle::PM_TitleBarHeight);
}

/**
	* \fn NVBDirView::midMargin
	*
	* \returns Distance between the header and the first grid line (items)
	*/

int NVBDirView::midMargin() const
{
  return 3;
}

/**
	* \fn NVBDirView::topMargin
	*
	* \returns Distance between the top of the view / bottom of previous item and the header
	*/

int NVBDirView::topMargin() const
{
  return 5;
}

/**
	* \fn NVBDirView::btmMargin
	*
	* \returns Distance between the last row of items and the next file
	*/

int NVBDirView::btmMargin() const
{
  return 3;
}

int NVBDirView::leftMargin() const
{
  return 10;
}

int NVBDirView::rightMargin() const
{
  return 10;
}

int NVBDirView::verticalOffset() const
{
  return top_row;
}

void NVBDirView::updateScrollBars()
{
//   heights.fill(-1);
//   offsets.fill(-1);
  if (model()) {
		if (top_row < 0 || top_row >= model()->rowCount())
      top_row = model()->rowCount() - 1;
    verticalScrollBar()->setRange(0, model()->rowCount()-1);
    }
  else
    verticalScrollBar()->setRange(0, -1);
  update();
}

int NVBDirView::fileDistance( int index1, int index2 ) const
{
  int i0 = qMin(index1,index2);
  int in = qMax(index1,index2);
  int dst = 0;
  while (in > i0)
    dst += fileHeight(i0++);
  return dst;
}

int NVBDirView::fileHeightLite( int nrows ) const
{
  if (heights.size() <= nrows) {
    int i0 = heights.size();
    heights.resize(nrows+1);
    for (int i = i0; i<heights.size(); i++)
      heights[i] = -1;
    }
  if (heights.at(nrows) < 0) {
    heights[nrows] = heights[0] + gridSize.height()*nrows;
    }
  return heights.at(nrows);
}

int NVBDirView::bottom_row()
{
  if (!model())
    return -1;

  int rendered_row = top_row;
  int row_bottom = fileHeight(rendered_row);
  while (rendered_row < model()->rowCount() && row_bottom <= height()) row_bottom += fileHeight(++rendered_row);

  return rendered_row - 1;
}



