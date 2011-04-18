#include "NVBDataSourceModel.h"
#include "NVBDatasetIcons.h"
#include "NVBMimeData.h"

#define PageRole Qt::UserRole

NVBDataSourceModel::NVBDataSourceModel(const NVBDataSource * s)
: QAbstractListModel()
, source(0)
, m(Direct)
{
	setSource(s);
}

NVBDataSourceModel::~NVBDataSourceModel()
{
	setSource(0);
}

void NVBDataSourceModel::setSource(const NVBDataSource* s)
{
	if (source) {
		disconnect(source,0,this,0);
		releaseDataSource(source);
		}

	source = s;

	if (source) {
		useDataSource(source);

		connect(source,SIGNAL(objectPopped(const NVBDataSource*, const NVBDataSource*)),this,SLOT(setSource(const NVBDataSource*)));
		connect(source,SIGNAL(objectPushed(const NVBDataSource*, const NVBDataSource*)),this,SLOT(setSource(const NVBDataSource*)));

		connect(source,SIGNAL(axesAboutToBeResized()),this,SLOT(parentAboutToReform()));
		connect(source,SIGNAL(axesResized()),this,SLOT(parentReformed()));
		connect(source,SIGNAL(dataAboutToBeReformed()),this,SLOT(parentAboutToReform()));
		connect(source,SIGNAL(dataReformed()),this,SLOT(parentReformed()));

		initIcons();
		}
}

QVariant NVBDataSourceModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (index.row() >= rowCount()) return QVariant();
//  if (role == Qt::CheckStateRole) return QVariant(Qt::Checked);
//  if (role == OriginalIndexRole) return QVariant(pages.at(index.row()).row());
	NVBDataSet * dset = source->dataSets().at(index.row());
  switch (role) {
    case Qt::DecorationRole : return QIcon(icons.at(index.row()));
    case Qt::DisplayRole    :
    case Qt::EditRole       : return dset->name();
    case Qt::StatusTipRole  :
    case Qt::ToolTipRole    : return dset->name();
    case PageRole           : return QVariant::fromValue(dset);
    default : return QVariant();
    }
}

Qt::ItemFlags NVBDataSourceModel::flags(const QModelIndex& index) const
{
  return QAbstractItemModel::flags(index);
}

QMimeData* NVBDataSourceModel::mimeData(const QModelIndexList& indexes) const
{
  if (indexes.count() > 1) {
    NVBOutputError("Dragging more than one object");
    return 0;
    }

  if (indexes.isEmpty()) return 0;

  return new NVBDataSourceMimeData(indexes.at(0).data(PageRole).value<NVBDataSource*>());
}

QStringList NVBDataSourceModel::mimeTypes() const
{
  return QStringList()
		<< NVBDataSourceMimeData::dataSetMimeType()
		<< NVBDataSourceMimeData::dataSourceMimeType()
		<< "text/plain"
		<< "image/x-qt-image"
		;
}

int NVBDataSourceModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid() || !source)
		return 0;
	else
		switch (m) {
			case Direct:
				return source->dataSets().count();
			case Crossed:
				return crosses.count();
			default :
				return 0;
			}
}

void NVBDataSourceModel::calculateCrosses()
{
	crosses.clear();
	// TODO implement crosspages
}

void NVBDataSourceModel::initIcons()
{
	icons.clear();
	
	switch(m) {
		case Direct: {
			foreach(const NVBDataSet * dset, source->dataSets())
				icons << createDatasetIcon(dset);
			break;
			}
		case Crossed: {
			break;
			}
		default: {
			NVBOutputError("Unknown mode");
			break;
			}
		}
}

void NVBDataSourceModel::parentAboutToReform() {
	beginResetModel();
}


void NVBDataSourceModel::parentReformed()	{
	initIcons();
	endResetModel();
}