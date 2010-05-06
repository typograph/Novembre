#ifndef NVBDATAVIEW_H
#define NVBDATAVIEW_H

class NVBDataSource;
class QWidget;
class QToolBar;

class NVBDataView {
public:
/// Unfortunately, Qt does not provide these functions, we must do it ourselves to ensure uniform 2DView
  virtual bool hasWidthForHeight() const = 0;
  virtual int  widthForHeight(int h) const = 0;

//   virtual bool hasHeightForWidth() const { return w()->hasHeightForWidth(); }
//   virtual int  heightForWidth(int h) const { return w()->heightForWidth(); }

/// For use with drag&drop. Will be called by NVBFileWindow to find out if a drag/drop event should be accepted.
  virtual bool acceptsPage(NVBDataSource * source) const = 0;

/// Provides a toolbar for the NVBFileWindow to use.
  virtual QToolBar * generateToolbar(QWidget * parent) const = 0;

/// Typesafe casting. Implement as \code return this;
  virtual const QWidget * w() const = 0;
  virtual QWidget * w() = 0;
};

#endif
