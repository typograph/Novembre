//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBDIRMODELHELPERS_H
#define NVBDIRMODELHELPERS_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QLinkedList>
#include <QtCore/QStack>
#include <QtCore/QDir>
#include <QtCore/QSignalMapper>
#include <QtGui/QApplication>
#include <QtCore/QRunnable>

#include "NVBFile.h"
#include "NVBFileFactory.h"
#include "NVBFileFilterDialog.h"
#include "NVBColumnsModel.h"

class NVBFileInfo;

class NVBDirModelFileInfoLessThan {
		// I like this crazy idea from the Trolls
	private:
		NVBTokens::NVBTokenList sortKey;
		Qt::SortOrder sortOrder;

	public:
		inline NVBDirModelFileInfoLessThan(NVBTokens::NVBTokenList key = NVBTokens::NVBTokenList(), Qt::SortOrder order = Qt::AscendingOrder)
			: sortKey(key), sortOrder(order) {;}

		bool operator()(const NVBFileInfo * fi1, const NVBFileInfo * fi2) const;
	};

class NVBDirModelFileInfoFilter {
	private:
		QList<NVBFileFilter> filters;
		const NVBDirModelColumns * columns;
	public:
		inline NVBDirModelFileInfoFilter()
			: columns(0) {;}
		inline NVBDirModelFileInfoFilter(QList<NVBFileFilter> fs, const NVBDirModelColumns * c)
			: filters(fs), columns(c) {;}
		bool operator()(const NVBFileInfo * fi) const;
	};


class NVBDirEntry;

/**
 * \class NVBDirEntryLoader
 *
 * This class handles the task of populating / refreshing
 * the contents of a NVBDirEntry
 *
 */

class NVBDirEntryLoader : public QRunnable {
	private:
		NVBDirEntry * e;
		NVBFileFactory * fileFactory;

		QList<int> indexesOf(const QStringList & items, const QStringList & list);
	public:
		NVBDirEntryLoader(NVBDirEntry * entry);
		virtual ~NVBDirEntryLoader();

		int locateFile(QString file, QList<NVBFileInfo*> list);

		void run();
	};

class NVBDirEntry : public QObject {
		Q_OBJECT
		friend class NVBDirEntryLoader;
	public:
		enum eventType { FolderListEvent = QEvent::User + 1, FileListEvent = QEvent::User + 2 };
		enum ContentChangeType { FolderInsert, FolderRemove, FileInsert, FileRemove };
		enum WatchedContentType { NoContent, FileContent, AllContent };
		enum Status {
		  Virgin, /// nothing was done with this entry after it's been created
		  Loading, /// NVBDirEntryLoader working
		  Populated, /// All operations finished, data available
		  Error /// Some error occured during loading

		};
		NVBDirEntry * parent;
		QString label;
		QDir dir;
		QList<NVBDirEntry*> folders;

		inline const NVBFileInfo * fileAt(int i) { return files.at(indexMap.at(i)); }
		inline int fileCount() const { return indexMap.count(); }
		/// Removes a file from the full list using mapped index
		void removeFileAt(int i);
		void removeFolderAt(int i);

	private:
		Status status;

		NVBDirModelFileInfoLessThan	sorter;
		NVBDirModelFileInfoFilter accepted;

		/* due to sorting, we need fast insertion.
			 However, QLinkedList doesn't allow sorting anymore
			 and it will not be effective anyway.
			 According to my tests, below 30000 elements sorting QVector is faster
			 than sorting QList, and this stands even for sorting by insertion.
			 But the gain is rather small, and above 30000 QList is faster.
			 Also, prepending in QList is faster, and we don't seem to need
			 our pointers to occupy the same place in memory.
			 */
		QList<NVBFileInfo*> files;
		QList<int> indexMap;

		WatchedContentType type;
		NVBDirEntry(); // private constructor... Checking

		Q_DISABLE_COPY(NVBDirEntry)

	public:
		NVBDirEntry(NVBDirEntry * _parent, QString _label);
		NVBDirEntry(NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive = false);
		~NVBDirEntry();

		int indexOf( QString name );

		void populate();

		bool isContainer() const { return (type == NoContent); }
		bool isRecursive() const { return (type == AllContent); }
		bool isPopulated() const { return status == NVBDirEntry::Populated || status == NVBDirEntry::Loading; }
		Status getStatus() const { return status; }
		void setDirWatch(bool b) {
			if (!b) {
				type = FileContent;

				if (parent) parent->setDirWatch(false);
				}
			}
		void addFolder( NVBDirEntry * folder );
		void insertFolder( int index, NVBDirEntry * folder );

		int estimatedFileCount() const;
		int estimatedFolderCount() const;
		int folderCount() const;

	private:
		void recurseFolders();
		/// inserts a new file, with filtering and sorting
		void insertFile(NVBFileInfo * file);
		/// Removes a file from the full list using full index
		void removeOrigFileAt(int i);
		/// Does a non-recursive filtering of the list, not notifying the model (it is in LayoutChanged mode)
		void refilter();

	protected:
		virtual bool event(QEvent *);

	signals:
		void beginOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type);
		void endOperation();
		void filesLoaded(const NVBDirEntry * entry, int fstart, int fend);
	private slots:
		void setLoaded() { status = NVBDirEntry::Populated; }
		void errorOnLoad() { status = NVBDirEntry::Error; }
	public slots:
		bool refresh();
		void refreshSubfolders();
		void sort(const NVBDirModelFileInfoLessThan & lessThan);
		void filter(const NVBDirModelFileInfoFilter & acceptFile);
	};

Q_DECLARE_METATYPE(NVBDirEntry::ContentChangeType);

#endif
