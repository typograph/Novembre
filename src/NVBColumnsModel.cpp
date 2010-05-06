
#include "NVBColumnsModel.h"

NVBDirModelColumns::NVBDirModelColumns():QAbstractTableModel()
{
}

Qt::ItemFlags NVBDirModelColumns::flags(const QModelIndex & index) const
{
  if (index.isValid()) {
    if (index.column() == 0)
      return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
    else
      return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
  else
    return QAbstractTableModel::flags(index);
}

QVariant NVBDirModelColumns::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal) return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  switch(section) {
    case 0 : return "Name";
    case 1 : return "Contents";
    default : return QVariant();
    }
}

QVariant NVBDirModelColumns::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (index.column() == 0) {
    switch (role) {
      case Qt::EditRole :
      case Qt::DisplayRole : return names.at(index.row());
      case Qt::CheckStateRole : return Qt::Checked;
      default : return QVariant();
      }
    }
  else {
    if (role != Qt::DisplayRole)
      return QVariant();
    else
      return QVariant::fromValue(keys.at(index.row()));
    }
}

int NVBDirModelColumns::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return names.size();
}

bool NVBDirModelColumns::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid()) return false;

  switch(role) {
    case Qt::EditRole :
    case Qt::DisplayRole : {
      if (index.column() == 0)
        names.replace(index.row(),value.toString());
      else
        keys.replace(index.row(),value.toString());
      }
    case Qt::CheckStateRole : {
      if (index.column() != 0) return false;
      
      }
    default : return false;
    }

}

QString NVBDirModelColumns::name(int row)
{
  if (row > rowCount())
    return QString();
  else
    return names.at(row);
}

NVBTokens::NVBTokenList NVBDirModelColumns::key(int row)
{
  if (row > rowCount())
    return NVBTokens::NVBTokenList();
  else
    return keys.at(row);
}

// void NVBDirModelColumns::addColumn(QString name, QString key)
// {
//   addColumn(name, NVBTokens::NVBTokenList(key));
// }

void NVBDirModelColumns::addColumn( QString name, NVBTokens::NVBTokenList key )
{
  beginInsertRows(QModelIndex(),rowCount()+1,rowCount()+1);
  names << name;
  keys << key;
  endInsertRows();
}

void NVBDirModelColumns::removeColumn(int row)
{
  if (row < rowCount()) {
    beginRemoveRows(QModelIndex(),row,row);
    names.removeAt(row);
    keys.removeAt(row);
    endRemoveRows();
    }
}

QList< NVBColumnDescriptor > NVBDirModelColumns::clmnDataList()
{
  QList< NVBColumnDescriptor > list;
  for (int i = 0; i<rowCount();i++) {
    list << NVBColumnDescriptor(name(i),key(i));
    }
  return list;
}

void NVBDirModelColumns::updateColumn(int index, NVBColumnDescriptor column)
{
  names.replace(index,column.name);
  keys.replace(index,column.contents);
}

QString NVBDirModelColumns::sourceKey( int row )
{
  return key(row).sourceString();
}

QString NVBDirModelColumns::verboseKey( int row )
{
  return key(row).verboseString();
}


