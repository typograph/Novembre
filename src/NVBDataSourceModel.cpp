#include "NVBDataSourceModel.h"
#include "NVBDatasetIcons.h"


NVBDataSourceModel::NVBDataSourceModel(const NVBDataSource * s) : QAbstractListModel()
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
		source->disconnect(this);
		releaseDataSource(source);
		}

	source = s

	if (source) {
		useDataSource(source);

		connect(source,SIGNAL(objectPopped(NVBDataSource*,NVBDataSource*)),this,SLOT(setSource(const NVBDataSource*)));
		connect(source,SIGNAL(objectPushed(NVBDataSource*,NVBDataSource*)),this,SLOT(setSource(const NVBDataSource*)));
		connect(source,SIGNAL(axesResized()),this,SLOT(updateIcons()));
		connect(source,SIGNAL(dataReformed()),this,SLOT(updateIcons()));

		updateIcons();
		}
}

void NVBDataSourceModel::clear()
{

}

QVariant NVBDataSourceModel::data(const QModelIndex& index, int role) const
{

}

Qt::ItemFlags NVBDataSourceModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

QMimeData* NVBDataSourceModel::mimeData(const QModelIndexList& indexes) const
{
    return QAbstractItemModel::mimeData(indexes);
}

QStringList NVBDataSourceModel::mimeTypes() const
{
  return QStringList() << NVBDataSourceMimeData::dataSourceMimeType();
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

void NVBDataSourceModel::updateIcons()
{

}

void NVBDataSourceModel::calculateCrosses()
{

}

void NVBDataSourceModel::initIcons()
{
	icons.clear();
	
	switch(mode) {
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
