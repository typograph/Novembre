#ifndef NVBCOLUMNSMODEL_H
#define NVBCOLUMNSMODEL_H

#include "NVBTokens.h"
#include <QAbstractTableModel>
#include <QStringList>

struct NVBColumnDescriptor {
  QString name;
//   QList<NVBCntToken> contents;
  NVBTokens::NVBTokenList contents;
  NVBColumnDescriptor(QString n = QString(), QString c = QString()):name(n),contents(NVBTokens::NVBTokenList(c)) {;}
  NVBColumnDescriptor(QString n, NVBTokens::NVBTokenList c ):name(n),contents(c) {;}
};


class NVBDirModelColumns : public QAbstractTableModel {

private:
  QStringList names;
  QList<NVBTokens::NVBTokenList> keys;

public:
  NVBDirModelColumns();
  virtual ~NVBDirModelColumns() {;}

  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
  virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const { return parent.isValid() ? 0 : 2; }
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

  QList<NVBColumnDescriptor> clmnDataList();

  QString name(int row) const;
  NVBTokens::NVBTokenList key(int row) const;
  QString sourceKey(int row) const;
  QString verboseKey(int row) const;
//   void addColumn(QString name, QString key);
  void addColumn(QString name, NVBTokens::NVBTokenList key);
  void updateColumn(int index, NVBColumnDescriptor column);
  void removeColumn(int row);

  QStringList columnNames() { return names; }

};


#endif
