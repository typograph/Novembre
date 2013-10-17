//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef NVBDIRVIEWMODEL_H
#define NVBDIRVIEWMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QWaitCondition>
#include "NVBDirModel.h"

#include <QtGui/QIcon>

class NVBShowFirstPageConverter;
class NVBFile;
class NVBFileModel;
class NVBFileFactory;
class NVBSpecOverlayIconProvider;

class NVBDirViewModelLoader : public QThread {
		Q_OBJECT
	private:
		QQueue<NVBAssociatedFilesInfo> queue;
		QQueue<QString> names;
		QMutex mutex;
		QWaitCondition condition;
		NVBFileFactory * factory;
	public:

		NVBDirViewModelLoader(NVBFileFactory * ff) : QThread(), factory(ff) {;}
		virtual ~NVBDirViewModelLoader();

		void run();

		void loadFile(const NVBAssociatedFilesInfo& info);
		void reset();
		bool loadingFinished();

	signals:
		void fileReady(NVBFile *, QString);
	};

class NVBFile2ImageConverter {
	private:
		mutable QHash<NVBFile*, QPixmap> pixmaps;
	protected:
		virtual QPixmap convertToImage(NVBFile * file) const = 0;
	public:
		QPixmap pixmapFromFile(NVBFile * file) const {
			if (!pixmaps.contains(file))
				pixmaps.insert(file, convertToImage(file));

			return pixmaps.value(file);
			}
		void reset() { pixmaps.clear(); }
	};

/**
This class provides a list of files in the folder with pages. Loads on demand.
Unloads if slot defineWindow connected
*/
class NVBDirViewModel : public QAbstractItemModel {
		Q_OBJECT
	public:

		enum Mode {
		  Normal = 0,
		  SpectroscopyOverlay,
		  SingleImage
//
		};

		NVBDirViewModel(NVBFileFactory * factory, NVBDirModel * model, QObject * parent = 0);
		virtual ~NVBDirViewModel();

		virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex &index) const;
		virtual QVariant data(const QModelIndex &index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		virtual QModelIndex parent ( const QModelIndex & index ) const;

		void setDisplayItems(QModelIndexList items);

		NVBAssociatedFilesInfo getAllFiles(const QModelIndex & index);

		virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
		virtual QStringList mimeTypes () const;

	private:
		NVBFileFactory * fileFactory;
		QList<QPersistentModelIndex> indexes;
		QPersistentModelIndex dirindex;
		NVBDirModel * dirModel;
		mutable QVector<NVBFileModel*> files;
		mutable QList<int> unloadables;
		mutable QList<int> inprogress;
		mutable QVector<int> rowcounts;
		QVector<QString> * fnamecache;
		mutable NVBDirViewModelLoader	loader;

		NVBSpecOverlayIconProvider * overlay;
		NVBFile2ImageConverter * imgConverter;
		NVBShowFirstPageConverter * firstConverter;

		bool operationRunning;
		void cacheRowCounts() const;
		void cacheRowCounts ( int first, int last ) const;

		Mode mode;
		bool overlayActive;

		mutable QPixmap unavailable, loading;

		QVariant unloadableData(int role) const;
		QVariant inProgressData(int role) const;
		QVariant extraDataAt(int index, int role) const;

	private slots:
		void parentInsertingRows( const QModelIndex & parent, int first, int last );
		void parentInsertedRows( const QModelIndex & parent, int first, int last );
		void parentRemovingRows( const QModelIndex & parent, int first, int last );
		void parentRemovedRows( const QModelIndex & parent, int first, int last );
//	void parentChangingLayout();
//	void parentChangedLayout();
		bool loadFile(int index) const;
//	void fileLoaded();
//	void fileLoaded(int index);
		void fileLoaded(NVBFile* file, QString name);
		void setMode(Mode m);

	public slots:
		void defineWindow(int start, int end);
		void setSingleImageProvider(NVBFile2ImageConverter * provider);
		void setOverlayActive(bool active);
		void setSingleImageModeActive(bool active);
		
	};


#endif
