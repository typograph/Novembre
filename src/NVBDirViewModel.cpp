//
// C++ Implementation: NVBDirViewModel
//
// Description:
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NVBDirViewModel.h"
#include "NVBFile.h"
#include "NVBFileFactory.h"
#include "NVBDirModel.h"
#include "NVBMimeData.h"
#include <QtCore/QFutureWatcher>
#include "NVBDataSourceModel.h"
#include <QtGui/QPainter>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtGui/QIconEngineV2>
#include <QtCore/QHash>
#include "NVBAxisMaps.h"
#include "NVBDatasetIcons.h"

class NVBFileModel : public NVBDataSourceListModel {
private:
	NVBFile * source;
public:
	NVBFileModel(NVBFile * file) : NVBDataSourceListModel(file), source(file) { if (source) source->use(); }
	~NVBFileModel() { source->release(); }
	
	inline NVBFile * file() { return source; }
};

class NVBShowFirstPageConverter : public NVBFile2ImageConverter {
private:
	virtual QPixmap convertToImage(NVBFile * file) const {
		if (file->isEmpty() || file->first()->dataSets().isEmpty())
			return QPixmap();
		return NVBDataColorInstance::colorize(file->first()->dataSets().first());
	}
};

void NVBDirViewModelLoader::run() {
	forever {
		mutex.lock();
		if (queue.isEmpty())
			condition.wait(&mutex);
		NVBAssociatedFilesInfo files = queue.dequeue();
		mutex.unlock();
		
		NVBFile* file = factory->getFile(files);
		if (file)
			file->moveToThread(QCoreApplication::instance()->thread());
		mutex.lock();
		emit fileReady(file,names.dequeue());
		mutex.unlock();
		}
	}
		
bool NVBDirViewModelLoader::loadingFinished() { // TODO it would be better to make a waitForTermination func
	QMutexLocker locker(&mutex);
	return names.isEmpty();
	}
	
void NVBDirViewModelLoader::loadFile(const NVBAssociatedFilesInfo & info) {
	QMutexLocker locker(&mutex);
	
	if (names.contains(info.name()))
		return;
	
	queue.enqueue(info);
	names.enqueue(info.name());
	if (queue.count() == 1) {
		if (isRunning())
			condition.wakeOne();
		else
			start();
		}
	}
	
void NVBDirViewModelLoader::reset() {
		QMutexLocker locker(&mutex);
		queue.clear();
		}

class NVBSpecOverlayIconProvider {
private:
	QHash< NVBFile *, QList<QIcon> > cache;

	QHash< NVBFile *, QList<QIcon> >::iterator createIcons(NVBFile * file) {
		return cache.insert(file,createSpecOverlayIcons(file));
		}

public:

	QIcon icon(NVBFile* file, int index) {
		QHash< NVBFile *, QList<QIcon> >::iterator i = cache.find(file);
		if (i == cache.end())
			i = createIcons(file);
		return i->at(index);
		}
	
	void reset() { cache.clear(); }
};

NVBDirViewModel::NVBDirViewModel(NVBFileFactory * factory, NVBDirModel * model, QObject * parent)
	: QAbstractItemModel( parent )
	, fileFactory(factory)
	, dirModel(model)
	, files(QVector<NVBFileModel*>())
	, fnamecache(0)
	, loader(factory)
	, operationRunning(false)
{
	
	connect(&loader,SIGNAL(fileReady(NVBFile*,QString)),this,SLOT(fileLoaded(NVBFile*,QString)));
	overlay = new NVBSpecOverlayIconProvider();
	imgConverter = new NVBShowFirstPageConverter();

//  cacheRowCounts();
	connect(dirModel,SIGNAL(rowsAboutToBeInserted(const QModelIndex &,int,int)), this, SLOT(parentInsertingRows(const QModelIndex &,int,int)));
	connect(dirModel,SIGNAL(rowsInserted (const QModelIndex &,int,int)), this, SLOT(parentInsertedRows(const QModelIndex &,int,int)));
	connect(dirModel,SIGNAL(rowsAboutToBeRemoved (const QModelIndex &,int,int)), this, SLOT(parentRemovingRows( const QModelIndex &,int,int)));
	connect(dirModel,SIGNAL(rowsRemoved (const QModelIndex &,int,int)), this, SLOT(parentRemovedRows(const QModelIndex &,int,int)));
//  connect(dirModel,SIGNAL(layoutAboutToBeChanged()), this, SLOT(parentChangingLayout()));
//  connect(dirModel,SIGNAL(layoutChanged()), this, SLOT(parentChangedLayout()));
}

NVBDirViewModel::~NVBDirViewModel()
{
	loader.reset(); // waitUntilFinished();
	delete overlay;
	delete imgConverter;
	foreach(NVBFileModel * f, files) {
		if (f) delete f;
		}
}

void NVBDirViewModel::setDisplayItems(QModelIndexList items) {
	beginResetModel();
	loader.reset();
	indexes.clear();
	overlay->reset();
	imgConverter->reset();
	foreach(NVBFileModel * f, files)
		delete f;

	dirindex = QPersistentModelIndex(QModelIndex());
	if (items.count() == 1 && ! dirModel->isAFile(items.first())) { // The only folder is expanded
		dirindex = QPersistentModelIndex(items.first());
		int foldc = dirModel->folderCount(items.first());
		int filec = dirModel->fileCount(items.first());
		for(int i = 0; i < filec; i += 1)
			indexes << QPersistentModelIndex(items.first().child(foldc + i,0));
		}
	else // ignore folders and colums > 0
		foreach(QModelIndex i, items)
			if (i.column()==0 && dirModel->isAFile(i))
				indexes << QPersistentModelIndex(i);

	files.fill(0,indexes.count());
	unloadables.clear();
				inprogress.clear();
				cacheRowCounts();
	endResetModel();
}

int NVBDirViewModel::rowCount( const QModelIndex & parent ) const
{
	if (!parent.isValid()) // how many files?
		return rowcounts.size();
	else if (mode == SingleImage)
		return 0;
	else
		return rowcounts.at(parent.row());
}

bool NVBDirViewModel::loadFile(int index) const
{
				if (inprogress.contains(index)) return false;
				if (files.at(index)) return true;
	if (unloadables.contains(index)) return false;
	
	loader.loadFile(dirModel->getAllFiles(indexes[index]));
				inprogress.append(index);

	return false;
}

void NVBDirViewModel::fileLoaded(NVBFile* file, QString name)
{
	int index = -1;

	for(int i = rowCount()-1;i>=0;i-=1)
		if (indexes.at(i).data(Qt::DisplayRole) == name) {
			index = i;
			break;
			}

	if (index < 0) {
		if (file) file->release();
		return;
		}

	if (file)
		files[index] = new NVBFileModel(file);
	else
		unloadables << index;

	QModelIndex fileix = this->index(index,0);
	emit dataChanged(this->index(0,0,fileix),this->index(rowCount(fileix)-1,0,fileix));
}

int NVBDirViewModel::columnCount( const QModelIndex & ) const
{
	return 1;
}

Qt::ItemFlags NVBDirViewModel::flags( const QModelIndex & index ) const
{
	if (!index.isValid())
		return QAbstractItemModel::flags(index);
	if (!index.parent().isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
}

QVariant NVBDirViewModel::data( const QModelIndex & index, int role ) const
{
	if (!index.isValid()) return QVariant();

	if (index.internalId() == 0) {
		if (mode == SingleImage) {
			if (role == Qt::DecorationRole) {
				if (files.at(index.row()))
					return QIcon(imgConverter->pixmapFromFile(files.at(index.row())->file()));
				else
					return extraDataAt(index.row(),role);
				}
			else if (role == Qt::DisplayRole) {
				const QAbstractItemModel * m = indexes.at(index.row()).model();
				QModelIndex pi = indexes.at(index.row()).parent();
				int row = indexes.at(index.row()).row();
				int c = m->columnCount(pi);
				QStringList texts;
				for(int i=0;i<c;i++) {
					texts << m->index(row,i,pi).data(Qt::DisplayRole).toString();
					if (texts.last().isEmpty()) texts.removeLast();
					}
				return texts.join("\n");
				}
			}
		return indexes.at(index.row()).data(role);
		}
	else {
		if (files.at(index.internalId()-1)) {
			if (mode == SpectroscopyOverlay && role == Qt::DecorationRole)
				return overlay->icon(files.at(index.internalId()-1)->file(),index.row());
			return files.at(index.internalId()-1)->index(index.row(),0).data(role);
			}
		else
			return extraDataAt(index.internalId()-1,role);
		}

	return QVariant();
}

QVariant NVBDirViewModel::extraDataAt(int index, int role) const {
	if (unloadables.contains(index))
		return unloadableData(role);
	else {
		loadFile(index);
		return inProgressData(role);
		}
}

QVariant NVBDirViewModel::unloadableData(int role) const {
	static QString dataul("Cannot be loaded");
	if (unavailable.isNull()) {
		unavailable = QPixmap(20,20);
		QPainter p(&unavailable);
		p.fillRect(0,0,20,20,Qt::white);
		p.setPen(Qt::red);
		p.drawLine(0,0,20,20);
		p.drawLine(0,20,20,0);
		p.end();
	}
	switch(role) {
		case Qt::DecorationRole:
			return QVariant::fromValue<QPixmap>(unavailable);
		case Qt::DisplayRole:
			return dataul;
		default:
			return QVariant();
	}
}

QVariant NVBDirViewModel::inProgressData(int role) const {
	static QString datal("Loading");
	if (loading.isNull()) {
		loading = QPixmap(20,20);
		QPainter p(&loading);
		p.fillRect(0,0,20,20,Qt::white);
		p.setPen(Qt::black);
		p.drawLine(5,5,15,15);
		p.drawLine(5,5,15,5);
		p.drawLine(5,15,15,5);
		p.drawLine(5,15,15,15);
		p.end();
	}
	switch(role) {
		case Qt::DecorationRole:
			return QVariant::fromValue<QPixmap>(loading);
		case Qt::DisplayRole:
			return datal;
		default:
			return QVariant();
	}
}

QVariant NVBDirViewModel::headerData( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
	return QVariant();
}

bool NVBDirViewModel::hasChildren( const QModelIndex & parent ) const
{
	if (!parent.isValid()) return true;

	if (mode == SingleImage)
		return false;

	return !unloadables.contains(parent.row());
}

QModelIndex NVBDirViewModel::index( int row, int column, const QModelIndex & parent ) const
{
	if (column > 0) return QModelIndex();

	if (row >= rowCount(parent)) return QModelIndex();

	if (parent.isValid())
		return createIndex(row,column,parent.row()+1);
	else
		return createIndex(row,column,0);
}

QModelIndex NVBDirViewModel::parent( const QModelIndex & index ) const
{
	if (!index.isValid())
		return QModelIndex();

	if (index.internalId() == 0)
		return QModelIndex();

	return createIndex(index.internalId()-1,0,0);
}

void NVBDirViewModel::parentInsertingRows(const QModelIndex & parent, int first, int last)
{
	Q_UNUSED(first)
	Q_UNUSED(last)
	if (dirindex.isValid() && parent == dirindex)
		operationRunning = true;
}

void NVBDirViewModel::parentInsertedRows(const QModelIndex & /*parent*/, int first, int last)
{
	if (operationRunning) {
		int fc = dirModel->folderCount(dirindex);
		first -= fc;
		last -= fc;
		first = qMax(first,0);
		if (last >= first) {
			beginInsertRows(QModelIndex(),first, last);
/*
			if (last >= rowcounts.count()) {
					rowcounts.resize(last+1);
					files.resize(last+1);
					unloadables.resize(last+1);
					}
*/
			rowcounts.insert(first,last-first+1,0);
			for (int i = first; i <= last; i += 1)
				indexes.insert(i,QPersistentModelIndex(dirModel->index(fc+i,0,dirindex)));
			for(int i = 0; i < unloadables.size(); i++) {
				if (unloadables.at(i) >= first)
					unloadables[i] += last-first+1;
				}
			files.insert(first,last-first+1,0);
			cacheRowCounts(first,last);
			endInsertRows();
			}
		operationRunning = false;
		}
}

void NVBDirViewModel::parentRemovingRows(const QModelIndex & parent, int first, int last)
{
	if (dirindex.isValid() && parent == dirindex) {
		int fc = dirModel->folderCount(dirindex);
		first = qMax(first,fc);
		if (last >= first) {
			operationRunning = true;
			beginRemoveRows(QModelIndex(),first - fc, last - fc);
			}
		}
}

void NVBDirViewModel::parentRemovedRows(const QModelIndex & /*parent*/, int first, int last)
{
	if (operationRunning) {
		int fc = dirModel->folderCount(dirindex);
		first -= fc;
		last -= fc;
		first = qMax(first,0);
		if (first <= last) {
			rowcounts.remove(first,last-first+1);
			for(int i = 0; i < unloadables.size(); i++) {
				if (unloadables.at(i) > last)
					unloadables[i] -= last-first+1;
				else if (unloadables.at(i) >= first)
					unloadables.removeAt(i--);
				}
			for (int i = first; i <= last; i++) {
				if (files.at(i))
					files.at(i)->release();
			files.remove(first,last-first+1);
			for (int i = first; i <= last; i += 1)
				indexes.removeAt(i);

//     rowcounts.resize(dirModel->fileCount(dirindex));
			}
		endRemoveRows();
		operationRunning = false;
		}
}

void NVBDirViewModel::cacheRowCounts( ) const
{
	rowcounts.resize(indexes.count());
	cacheRowCounts(0,rowcounts.size()-1);
}

void NVBDirViewModel::cacheRowCounts( int first, int last ) const
{
	for (int i = first ; i <= last; i++) {
		if (unloadables.contains(i))
			rowcounts[i] = 0;
		else {
			const NVBFileInfo * fInfo = dirModel->indexToInfo(indexes.at(i));
			if (fInfo)
				rowcounts[i] = fInfo->size();
			else
				rowcounts[i] = 0;
			}
		}
}

/*
void NVBDirViewModel::parentChangingLayout( )
{
	emit layoutAboutToBeChanged();
	// Cache filenames
	fnamecache = new QVector<QString>(rowCount());
	for(int i = rowCount()-1;i>=0;i--) {
		fnamecache->operator[](i) = dirModel->indexToInfo(dirModel->index(i,0,dirindex))->files.name();
		}
}

void NVBDirViewModel::parentChangedLayout( )
{
	if (fnamecache) {
		QVector<NVBFile*> recache(files.size());
		QVector<int> recounts(rowcounts.size());
		recache.fill(0);
		for(int i = rowCount()-1;i>=0;i--) {
			int x = fnamecache->indexOf(dirModel->indexToInfo(dirModel->index(i,0,dirindex))->files.name());
			changePersistentIndex(index(x,0),index(i,0));
			recache[i] = files.at(x);
			recounts[i] = rowcounts.at(x);
			}
		files = recache;
		rowcounts = recounts;
		delete fnamecache;
		fnamecache = 0;
		}
	emit layoutChanged();
}
*/

void NVBDirViewModel::defineWindow(int start, int end)
{
	for(int i=0;i<start;i++)
		if (files.at(i)) {
			delete files[i];
			files[i] = 0;
			}

	for(int i=end+1;i<files.size();i++)
		if (files.at(i)) {
			delete files[i];
			files[i] = 0;
			}
}

NVBAssociatedFilesInfo NVBDirViewModel::getAllFiles(const QModelIndex & index) {
	if (!index.isValid() || !dirModel) return NVBAssociatedFilesInfo();

	if (index.internalId() == 0)
		return dirModel->getAllFiles(indexes.at(index.row()));
	else {
		return dirModel->getAllFiles(indexes.at(index.internalId()-1));
//		if (loadFile(index.internalId()-1))
//			return files.at(index.internalId()-1)->sources();
//		else
//			return NVBAssociatedFilesInfo();
		}

}

QMimeData * NVBDirViewModel::mimeData(const QModelIndexList &ixs) const {
	if (ixs.count() > 1) {
		NVBOutputError("Dragging more than one object");
		return 0;
		}

	if (ixs.isEmpty()) return 0;

	QModelIndex i = ixs.first();

	if (mode == SingleImage) {
		QMimeData * md = new QMimeData();
		md->setImageData(i.data(Qt::DecorationRole));
		return md;
	}

	if ( i.internalId() == 0 || !loadFile(i.internalId()-1)) return 0;

	NVBDataSourceListModel * m = files.at(i.internalId()-1);
	
	return m->mimeData(QModelIndexList() << m->index(i.row()));
}

QStringList NVBDirViewModel::mimeTypes () const {
	return QStringList()
		<< NVBDataSourceMimeData::dataSetMimeType()
		<< NVBDataSourceMimeData::dataSourceMimeType()
		<< "text/plain"
		<< "image/x-qt-image"
		;
}


void NVBDirViewModel::setMode(Mode m) {
	mode = m;
	overlay->reset();
	emit dataChanged(index(0,0),index(rowCount()-1,0));
}

void NVBDirViewModel::setSingleImageProvider(NVBFile2ImageConverter * provider) {
	if (!provider) {
//		mode = Normal;
//		return;
		provider = new NVBShowFirstPageConverter();
		}

	if (imgConverter)
		delete imgConverter;
	imgConverter = provider;

	mode = SingleImage;
}
