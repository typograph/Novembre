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
#include "NVBContColoring.h"

#include <QHash>

#if QT_VERSION >= 0x040300
#include <QIconEngineV2>
class NVBMixTSIcon : public QObject, public QIconEngineV2 {
public :
	NVBMixTSIcon(NVB3DDataSource* topo, NVBSpecDataSource* spec):QIconEngineV2(),
#else
#include <QIconEngine>
class NVBMixTSIcon : public QObject, public QIconEngine {
public :
	NVBMixTSIcon(NVB3DDataSource* topo, NVBSpecDataSource* spec):QIconEngine(),
#endif
	cache(0),stopo(topo),sspec(spec) {
		if (!stopo || !sspec) {
			NVBOutputError("A topography page and a spectroscopy page are needed");
			return;
			}
		cache = colorizeWithPlaneSubtraction(stopo);
		if (!cache)  {
			NVBOutputError("pixmap allocation failed");
			return;
			}
		}
	virtual ~NVBMixTSIcon() { if (cache) delete (cache);}

	static QImage * colorizeWithPlaneSubtraction(NVB3DDataSource * page) {
		const double * pdata = page->getData();

		double xnorm = 0, ynorm = 0;

		int iw = page->resolution().width();
		int ih = page->resolution().height();
		int sz = iw*ih;

		for(int i=0; i < sz; i += iw)
			xnorm += pdata[i] - pdata[i+iw-1];
		for(int i=0; i < iw; i += 1)
			ynorm += pdata[i] - pdata[i+sz-ih];

		xnorm /= (iw-1)*ih;
		ynorm /= iw*iw*(ih-1);

		double * ndata = (double *) malloc(sz*sizeof(double));

		double zmin = pdata[0], zmax = pdata[0];

		for(int i=0; i < iw; i += 1)
			for(int j=0; j < sz; j += iw) {
				ndata[i+j] = pdata[i+j] + xnorm*i + ynorm*j;
				zmin = qMin(zmin,ndata[i+j]);
				zmax = qMax(zmax,ndata[i+j]);
				}

		NVBRescaleColorModel * rm = new NVBRescaleColorModel(page->getColorModel());
		rm->setLimits(zmin,zmax);

		QImage * i = dynamic_cast<NVBContColorModel*>(rm)->colorize( ndata , page->resolution() );
		delete rm;
		free(ndata);
		return i;
	}

	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) {
		if (!cache) {
			painter->save();
			painter->setPen(QPen(Qt::blue));
			painter->setBrush(Qt::blue);
			painter->drawLine(rect.topLeft(),rect.bottomRight());
			painter->drawLine(rect.topRight(),rect.bottomLeft());
			painter->restore();
			return;
			}

		painter->drawImage(rect,*cache);
		// Paint dots

		painter->save();
		painter->setPen(QPen(Qt::blue));
		painter->setBrush(Qt::blue);

		scaler<double,int> w(stopo->position().left(),stopo->position().right(),rect.left(),rect.right());
		scaler<double,int> h(stopo->position().top(),stopo->position().bottom(),rect.top(),rect.bottom());

		foreach( QPointF p,sspec->positions()) {
			painter->drawEllipse(w.scale(p.x())-1,h.scale(p.y())-1,2,2);
			}

		painter->restore();

		}

protected:
//  NVBDataSource * provider;
	QImage* cache;
	NVB3DDataSource* stopo;
	NVBSpecDataSource* sspec;
};

class NVBSpecOverlayIconProvider {
private:
	QHash< NVBFile *, QList<QIcon> > cache;

	QHash< NVBFile *, QList<QIcon> >::iterator createIcons(NVBFile * file) {
		NVBSpecDataSource * specnote = 0;	
		QList<QIcon> icons;
		
		for (int i=0; i < file->rowCount(QModelIndex()); i++) {
			if (file->data(file->index(i),PageTypeRole).value<NVB::PageType>() == NVB::SpecPage) {
				specnote = (NVBSpecDataSource*)(file->data(file->index(i),PageRole).value<NVBDataSource*>());
				break;
				}
		  }

		for (int i=0; i < file->rowCount(QModelIndex()); i++) {
			if (specnote && file->data(file->index(i),PageTypeRole).value<NVB::PageType>() == NVB::TopoPage)
				icons << QIcon(new NVBMixTSIcon((NVB3DDataSource*)(file->data(file->index(i),PageRole).value<NVBDataSource*>()),specnote));
			else
				icons << file->data(file->index(i),Qt::DecorationRole).value<QIcon>();
		  }
		  
		return cache.insert(file,icons);
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
	, files(QVector<NVBFile*>())
	, fnamecache(0)
	, operationRunning(false)
	, mode(Normal)
{
	overlay = new NVBSpecOverlayIconProvider();

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
	delete overlay;
	foreach(NVBFile * f, files) {
		if (f) f->release();
		}
}

void NVBDirViewModel::setDisplayItems(QModelIndexList items) {
	beginResetModel();
	indexes.clear();
	foreach(NVBFile * f, files) {
		if (f) f->release();
		}
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
	else
		return rowcounts.at(parent.row());
}

bool NVBDirViewModel::loadFile(int index) const
{
	if (inprogress.contains(index)) return false;
	if (files.at(index)) return true;
	if (unloadables.contains(index)) return false;

	fileFactory->openFile(dirModel->getAllFiles(indexes[index]),this);
	inprogress.append(index);
	return false;
/*
	NVBFile * f = fileFactory->openFile(dirModel->getAllFiles(indexes[index]));
	if (f) {
		f->use();
		files[index] = f;
		return true;
		}
	else {
//     emit layoutAboutToBeChanged();
		unloadables << index;
//     emit layoutChanged();
		return false;
		}
*/
}

bool NVBDirViewModel::event ( QEvent * e ) {
	if (e->type() == QEvent::User) {
		NVBFileLoadEvent * x = static_cast<NVBFileLoadEvent*>(e);
		if (x) {
			fileLoaded(x->name,x->file);
			return true;
		}
	}
	return QAbstractItemModel::event(e);
}

void NVBDirViewModel::fileLoaded(QString name, NVBFile * file)
{
	int index = -1;

	for(int i = rowCount()-1;i>=0;i-=1)
		if (indexes.at(i).data(Qt::DisplayRole) == name) {
			index = i;
			break;
			}

	if (index < 0)
		return;

	if (file) {
		file->use();
		files[index] = file;
		}
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
		return indexes.at(index.row()).data(role);
		}
	else {
		if (files.at(index.internalId()-1)) {
			if (mode == SpectroscopyOverlay && role == Qt::DecorationRole)
				return overlay->icon(files.at(index.internalId()-1),index.row());
			return files.at(index.internalId()-1)->index(index.row(),0).data(role);
			}
		else if (unloadables.contains(index.internalId()-1))
			return unloadableData(role);
		else {
			loadFile(index.internalId()-1);
			return inProgressData(role);
			}
		}

	return QVariant();
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

	return not unloadables.contains(parent.row());
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
			for (int i = first; i <= last; i++)
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
				rowcounts[i] = fInfo->pages.size();
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
			files[i]->release();
			files[i] = 0;
			}

	for(int i=end+1;i<files.size();i++)
		if (files.at(i)) {
			files[i]->release();
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
	if ( i.internalId() == 0 || !loadFile(i.internalId()-1)) return 0;

	return new NVBDataSourceMimeData(NVBToolsFactory::hardlinkDataSource(files.at(i.internalId()-1)->index(i.row(),0).data(PageRole).value<NVBDataSource*>()));
}

QStringList NVBDirViewModel::mimeTypes () const {
	return QStringList() << NVBDataSourceMimeData::dataSourceMimeType();
}


void NVBDirViewModel::setMode(Mode m) {
	mode = m;
	overlay->reset();
	emit dataChanged(index(0,0),index(rowCount()-1,0));
}
