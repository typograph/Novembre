#ifndef NVB3DPAGEVIEW_H
#define NVB3DPAGEVIEW_H

#include <QStackedWidget>
#include "NVBDataView.h"
#include "NVBPageViewModel.h"
#include "NVBVizModel.h"
#include "NVBLogger.h"

class NVB3DPageView : public QStackedWidget, public NVBDataView {
//class NVB3DPageView : public Qwt3D::Plot3D, public NVBDataView {
Q_OBJECT
private:
  NVBVizModel * vizmodel;

  Qwt3D::Plot3D * currentPlot();

public:
  NVB3DPageView(NVBVizModel* model, QWidget * parent);
  ~NVB3DPageView();

  virtual bool hasWidthForHeight() const { return false; } // FIXME not entirely true, since there are constrains
	virtual int  widthForHeight(int) const { return -1; }

  virtual bool acceptsPage(NVBDataSource * source) const;

  virtual const QWidget * w() const { return this; }
  virtual QWidget * w() { return this; }

  virtual QToolBar * generateToolbar(QWidget * parent) const;
//   QAbstractListModel * getVizModel() { return vizmodel; }

public slots :
  void select( const QModelIndex & index);
  void setActiveVisualizer( NVBVizUnion, const QModelIndex & ) {;} // I have no idea what it could be

  void exportImage();

protected :

protected slots:
//   virtual void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
  virtual void rowsRemoved ( const QModelIndex & parent, int start, int end );
  virtual void rowsInserted ( const QModelIndex & parent, int start, int end );
  virtual void updateVizs(const QModelIndex& start, const QModelIndex& end);

signals :

};

#endif
