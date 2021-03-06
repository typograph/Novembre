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

#ifndef NVBDIRMODEL_H
#define NVBDIRMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QLinkedList>
#include <QtCore/QStack>
#include <QtCore/QDir>
#include <QtCore/QSignalMapper>
#include <QtGui/QApplication>

#include "NVBFile.h"
#include "NVBFileFactory.h"
#include "NVBFileFilterDialog.h"
#include "NVBColumnsModel.h"
#include "NVBDirModelHelpers.h"

#define NVBColKeyRole (Qt::UserRole+1)
#define NVBColStrKeyRole (Qt::UserRole+2)

class QTextStream;

struct NVBDirExportOptions {
	bool valid;
	QString fileName;
	bool recursiveExport;
	bool loadOnExport;
	bool fullFileNames;
	};

class NVBDirModelColumns;
class QFileSystemWatcher;
class NVBDirEntry;
/**
This class provides a tree of folders with STM files in them.
*/
class NVBDirModel : public QAbstractItemModel {
		Q_OBJECT
	public:

		NVBDirModel(NVBFileFactory * _fileFactory, QObject * parent = 0);
		virtual ~NVBDirModel();

		QModelIndex addFolderItem( QString label, QString path = QString(), bool recursive = false, const QModelIndex & parent  = QModelIndex() );
		void editFolderItem( QString label, QString path, bool recursive, const QModelIndex & index);
		bool removeItem( const QModelIndex& index );

		bool isAFile( const QModelIndex& index ) const;
		bool isRecursive( const QModelIndex& index ) const;

		virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int estimatedRowCount(const QModelIndex &parent = QModelIndex()) const;
		int folderCount(const QModelIndex &parent = QModelIndex(), bool treatRecursiveSeparately = false) const;
		int fileCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex &index) const;
		virtual QVariant data(const QModelIndex &index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );

		virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		virtual QModelIndex parent ( const QModelIndex & index ) const;

//	NVBDirFileEntry indexToFileEntry( const QModelIndex & index ) const;
		const NVBFileInfo * indexToInfo( const QModelIndex & index ) const;

		QString getFullPath(const QModelIndex & index);
		NVBAssociatedFilesInfo getAllFiles(const QModelIndex & index);

		inline void addColumn(QString name, QString key) { return addColumn(name, NVBTokens::NVBTokenList(key)); }
		void addColumn(QString name, NVBTokens::NVBTokenList key);
		void addColumn(NVBColumnDescriptor column);
		void updateColumn(int index, NVBColumnDescriptor column);
		void removeColumn(int index);

		int visibleRows(NVBDirEntry * entry = 0 ) const;

		static inline bool variantGreaterOrEqualTo(const NVBVariant & l, const NVBVariant & r) { return l >= r; }
		static inline bool variantLessThan(const NVBVariant & l, const NVBVariant & r) { return l < r;}

		virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );

	public slots:
		void refresh();

		void showFilterDialog();
		void removeFilters();
		void showColumnDialog();
		void exportData(const QModelIndex & index, NVBDirExportOptions options) const;

	private:

		struct NVBDirEntryOperation {
			NVBDirEntryOperation(): e(0), r(0), c(0), t(NVBDirEntry::FolderRemove) {;}
			NVBDirEntryOperation(const NVBDirEntry * _e, int _r, int _c, NVBDirEntry::ContentChangeType _t)
				: e(_e)
				, r(_r)
				, c(_c)
				, t(_t)
				{;}
			const NVBDirEntry * e;
			int r, c;
			NVBDirEntry::ContentChangeType t;
			};

		QStack<NVBDirEntryOperation> entryOperationStack;

		QList<NVBFileFilter> filters;

		NVBFileFactory * fileFactory;
		QFileSystemWatcher * watcher;
		NVBDirModelColumns * columns;

		int sortColumn;
		Qt::SortOrder sortOrder;

		NVBDirEntry * head;
		NVBDirEntry * indexToEntry( const QModelIndex & index ) const;
		QModelIndex entryToIndex( const NVBDirEntry * entry ) const;
		NVBDirEntry * indexToParentEntry( const QModelIndex & index ) const;
		NVBDirEntry * findEntryByPathFrom( const NVBDirEntry * entry, const QString & path ) const;
		QList<QModelIndex> folderIndexes( NVBDirEntry * entry );
		int visibleRows(NVBDirEntry*);
		void connectEntry(NVBDirEntry * entry) const;

		QSignalMapper timerToEntryMap;

		void exportItemFiles(QTextStream * file, const QModelIndex & index, NVBDirExportOptions options) const;
		void exportItemData(QTextStream * file, const QModelIndex & index, int row, bool fullName) const;

		bool lessThan(const QModelIndex & left, const QModelIndex & right) const;

	private slots:
		void watchedDirChanged(const QString & path);
		void beginEntryOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type);
		void endEntryOperation();
		void setEntryRefreshTimer(NVBDirEntry * entry);
		void refreshWatchedDir( QObject * o_entry );
		void notifyFilesLoaded(const NVBDirEntry * entry, int fstart, int fend);

	};

#endif
