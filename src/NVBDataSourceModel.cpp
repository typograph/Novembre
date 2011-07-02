#include "NVBDataSourceModel.h"
#include "NVBDatasetIcons.h"
#include "NVBMimeData.h"
#include "NVBforeach.h"

#include <QtCore/QAbstractItemModel>

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

  return mimeData(indexes.first());
}

QMimeData* NVBDataSourceModel::mimeData(const QModelIndex& index) const
{
  return new NVBDataSourceMimeData(index.data(PageRole).value<NVBDataSource*>());
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

// --------------- NVBDataSourceListModel

NVBDataSourceListModel::NVBDataSourceListModel(const QList<NVBDataSource*> * sources)
	: QAbstractListModel(0)
{
	cachecounts << 0;
	NVB_FOREACH(NVBDataSource * s, sources) {
		
		NVBDataSourceModel * m = new NVBDataSourceModel(s);
		
		connect(m,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(subDataChanged(QModelIndex,QModelIndex)));
		connect(m,SIGNAL(modelAboutToBeReset()),this,SLOT(subModelAboutToBeReset()));
		connect(m,SIGNAL(modelReset ()),this,SLOT(subModelReset()));
		connect(m,SIGNAL(rowsAboutToBeInserted(const QModelIndex&,int,int)),this,SLOT(subRowsAboutToBeInserted(const QModelIndex&,int,int)));
		connect(m,SIGNAL(rowsAboutToBeMoved(const QModelIndex&,int,int,const QModelIndex&,int)),this,SLOT(subRowsAboutToBeMoved(const QModelIndex&,int,int,const QModelIndex&,int)));
		connect(m,SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,int,int)),this,SLOT(subRowsAboutToBeRemoved(QModelIndex,int,int)));
		connect(m,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(subRowsInserted(QModelIndex,int,int)));	connect(m,SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),this,SLOT(subRowsMoved(QModelIndex,int,int,QModelIndex,int)));
		connect(m,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(subRowsRemoved(QModelIndex,int,int)));

		models << m;
		cachecounts << cachecounts.last() + m->rowCount();
		}
}

NVBDataSourceListModel::~NVBDataSourceListModel() {
	foreach(NVBDataSourceModel * m, models)
		delete m;
}

int NVBDataSourceListModel::rowCount(const QModelIndex& ) const
{
	return cachecounts.last();
}

QVariant NVBDataSourceListModel::data(const QModelIndex& index, int role) const
{
	NVBDSLModelSubIndex i = map(index.row());
	if (i.first)
		return i.first->data(i.first->index(i.second),role);
	return QVariant();
}

const NVBDataSet* NVBDataSourceListModel::dataSetAt(const QModelIndex& index) const
{
	NVBDSLModelSubIndex i = map(index.row());
	if (i.first)
		return i.first->dataSetAt(i.first->index(i.second));
	return 0;
}


Qt::ItemFlags NVBDataSourceListModel::flags(const QModelIndex& index) const
{
	NVBDSLModelSubIndex i = map(index.row());
	if (i.first)
		return i.first->flags(i.first->index(i.second));
	return Qt::NoItemFlags;
}

QMimeData* NVBDataSourceListModel::mimeData(const QModelIndexList& indexes) const
{
  if (indexes.count() > 1) {
    NVBOutputError("Dragging more than one object");
    return 0;
    }

  if (indexes.isEmpty()) return 0;

	QModelIndex index = indexes.first();
	NVBDSLModelSubIndex i = map(index.row());
	if (i.first)
		return i.first->mimeData(i.first->index(i.second));
	return 0;
}

QStringList NVBDataSourceListModel::mimeTypes() const
{
  return QStringList()
		<< NVBDataSourceMimeData::dataSetMimeType()
		<< NVBDataSourceMimeData::dataSourceMimeType()
		<< "text/plain"
		<< "image/x-qt-image"
		;
}

NVBDSLModelSubIndex NVBDataSourceListModel::map(int k) const {
	if (k < 0) {
		NVBOutputError("Index out of bounds");
		return NVBDSLModelSubIndex(0,0);
		}

	QList<int>::const_iterator it = qUpperBound(cachecounts,k);
	int i = it - cachecounts.begin() - 1;
	// If k is in list, it belongs to next element
	// If k is not in list, it belongs to the next element anyway

	if (i+1 < cachecounts.count())
		return NVBDSLModelSubIndex(models.at(i),k-cachecounts.at(i));
	else {
		NVBOutputError("Index out of bounds");
		return NVBDSLModelSubIndex(0,0);
		}
}

#define callerModelIndex() models.indexOf(qobject_cast<NVBDataSourceModel*>(sender()))
#define callerModelStartIndex() cachecounts.at(callerModelIndex())
#define callerModelRowCount() qobject_cast<NVBDataSourceModel*>(sender())->rowCount()

void NVBDataSourceListModel::subDataChanged(QModelIndex topLeft, QModelIndex bottomRight)
{
	int mi = callerModelStartIndex();
	emit dataChanged(index(mi+topLeft.row()),index(mi+bottomRight.row()));
}

void NVBDataSourceListModel::subModelAboutToBeReset()
{
	// Can't do anything here. Or&
	int mi = callerModelIndex();
	int rc = callerModelRowCount();
	beginRemoveRows(QModelIndex(),cachecounts.at(mi),cachecounts.at(mi+1)-1);
	for(int i = mi+1; i<cachecounts.size(); i++) {
		cachecounts[i] -= rc;
		}
	endRemoveRows();
}

void NVBDataSourceListModel::subModelReset()
{
	int mi = callerModelIndex();
	int rc = callerModelRowCount();
	beginInsertRows(QModelIndex(),cachecounts.at(mi),cachecounts.at(mi)+rc-1);
	for(int i = mi+1; i<cachecounts.size(); i++) {
		cachecounts[i] += rc;
		}	
	endInsertRows();
}

void NVBDataSourceListModel::subRowsAboutToBeInserted(const QModelIndex& , int start, int end)
{
	int mi = callerModelStartIndex();
	beginInsertRows(QModelIndex(),mi+start,mi+end);
}

void NVBDataSourceListModel::subRowsInserted(const QModelIndex& , int start, int end)
{
	int mi = callerModelIndex();
	int rc = end-start+1;
	for(int i = mi+1; i<cachecounts.size(); i++)
		cachecounts[i] += rc;
	endInsertRows();
}

void NVBDataSourceListModel::subRowsAboutToBeMoved(const QModelIndex& , int sourceStart, int sourceEnd, const QModelIndex& , int destinationRow)
{
	int mi = callerModelStartIndex();
	beginMoveRows(QModelIndex(),mi+sourceStart,mi+sourceEnd,QModelIndex(),mi+destinationRow);
}

void NVBDataSourceListModel::subRowsMoved(const QModelIndex& , int , int , const QModelIndex&, int )
{
	// moving rows doesn't affect the cache
	endMoveRows();
}

void NVBDataSourceListModel::subRowsAboutToBeRemoved(const QModelIndex& , int start, int end)
{
	int mi = callerModelStartIndex();
	beginRemoveRows(QModelIndex(),mi+start,mi+end);
}

void NVBDataSourceListModel::subRowsRemoved(const QModelIndex& , int start, int end)
{
	int mi = callerModelIndex();
	int rc = end-start+1;
	for(int i = mi+1; i<cachecounts.size(); i++)
		cachecounts[i] -= rc;
	endRemoveRows();
}

