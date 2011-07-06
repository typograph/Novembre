//
// C++ Interface: NVBPageRefactorModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBPAGEREFACTORMODEL_H
#define NVBPAGEREFACTORMODEL_H

#include <QAbstractListModel>

class NVBPageViewModel;

class NVBPageRefactorModel : public QAbstractListModel {
public:
  enum RefactorMode {
    None = 0, /// The source model is unchanged
    Empty = 1, /// This model is empty
    MarkSpectra /// Mark spectroscopic positions on topography
    };

private:
  NVBPageViewModel * source;
  RefactorMode mode, bkup_mode;

  QList<QIcon> icons;

  void initialize();
  void clear();

public:
  NVBPageRefactorModel():QAbstractListModel(),source(0),mode(Empty),bkup_mode(None) {;}
  ~NVBPageRefactorModel() {clear();}

  void setModel(NVBPageViewModel * model);
  void setMode(RefactorMode new_mode);

  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

//   virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

  virtual bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

  virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
  virtual Qt::DropActions supportedDropActions () const;
  virtual QStringList mimeTypes () const;
  virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

};

#endif
