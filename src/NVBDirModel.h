//
// C++ Interface: NVBDirModel
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBDIRMODEL_H
#define NVBDIRMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QList>
#include <QStack>
#include <QDir>
#include <QSignalMapper>
#include <QApplication>

#include "NVBFile.h"
#include "NVBFileFactory.h"
#include "NVBFileFilterDialog.h"
#include "NVBColumnsModel.h"

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

#if QT_VERSION >= 0x040400
template <class T>
	class QFutureWatcher;
#endif

class NVBDirEntry : public QObject {
Q_OBJECT
public:
	enum ContentChangeType { FolderInsert, FolderRemove, FileInsert, FileRemove };
	enum WatchedContentType { NoContent, FileContent, AllContent };
	NVBDirEntry * parent;
	QString label;
	QDir dir;
	QList<NVBDirEntry*> folders;
	QLinkedList<NVBFileInfo*> files; // due to sorting, we need fast insertion.
	QList<int> filtered;

private:
	bool populated;
	bool loaded;
#if QT_VERSION >= 0x040400
	QFutureWatcher<NVBFileInfo*> * fileLoader;
#endif
	WatchedContentType type;
	NVBDirEntry(); // private constructor... Checking
	NVBDirEntry(const NVBDirEntry & ):QObject() {;} // private copy constructor... Checking
	NVBDirEntry(const NVBDirEntry * ):QObject() {;} // private copy constructor... Checking
public:
	NVBDirEntry(NVBDirEntry * _parent, QString _label);
	NVBDirEntry(NVBDirEntry * _parent, QString _label, QDir _dir, bool recursive = false);
	~NVBDirEntry();
	
	int indexOf( QString name );
	
	void populate(NVBFileFactory * fileFactory);
	
	bool isContainer() const { return (type == NoContent); }
	bool isRecursive() const { return (type == AllContent); }
	bool isPopulated() const { return populated; }
	bool isLoaded() const { return loaded; }
	void setDirWatch(bool b) {
		if (!b) {
			type = FileContent;
			if (parent) parent->setDirWatch(false);
			}
		}
	void addFolder( NVBDirEntry * folder );
	
	int estimatedFileCount() const;
	int estimatedFolderCount() const;
	int folderCount() const;

	void invalidateFiltered();

private:
	void recurseFolders();
signals:
	void beginOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type);
	void endOperation();
	void filesLoaded(const NVBDirEntry * entry, int fstart, int fend);
private slots:
	void notifyLoading(int start, int end);
	void setLoaded() { loaded = true; }
public slots:
	bool refresh(NVBFileFactory * fileFactory);
	void refreshSubfolders(NVBFileFactory * fileFactory);
};

class NVBDirModelColumns;
class QFileSystemWatcher;
/**
This class provides a tree of folders with STM files in them.
*/
class NVBDirModel : public QAbstractItemModel
{
Q_OBJECT
public:

	NVBDirModel(NVBFileFactory * _fileFactory, QObject * parent = 0);
	virtual ~NVBDirModel();
	
	QModelIndex addFolderItem( QString label, QString path = QString(), bool recursive = false, const QModelIndex & parent  = QModelIndex() );
	bool removeItem( const QModelIndex& index );
	
	bool isAFile( const QModelIndex& index ) const;
	
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
	
	NVBDirFileEntry indexToFileEntry( const QModelIndex & index ) const;
	const NVBFileInfo * indexToInfo( const QModelIndex & index ) const { return indexToFileEntry(index).info; }
	
	QString getFullPath(const QModelIndex & index);
	
	inline void addColumn(QString name, QString key) { return addColumn(name, NVBTokens::NVBTokenList(key)); }
	void addColumn(QString name, NVBTokens::NVBTokenList key);
	void addColumn(NVBColumnDescriptor column);
	void updateColumn(int index, NVBColumnDescriptor column);
	void removeColumn(int index);
	
	int visibleRows(NVBDirEntry * entry = 0 ) const;
	
public slots:
	void refresh();
	
	void showFilterDialog();
	void removeFilters() {
		filters.clear();
		updateFilters();
		}
	void showColumnDialog();
	void exportData(const QModelIndex & index, NVBDirExportOptions options) const;
	
private:
	
	struct NVBDirEntryOperation {
		NVBDirEntryOperation():e(0),r(0),c(0),t(NVBDirEntry::FolderRemove) {;}
		NVBDirEntryOperation(const NVBDirEntry * _e, int _r, int _c, NVBDirEntry::ContentChangeType _t):e(const_cast<NVBDirEntry*>(_e)),r(_r),c(_c),t(_t) {;}
	//     NVBDirEntryOperation(const NVBDirEntryOperation & o):e(o.e),r(o.r),c(o.c),t(o.t) {;}
	//     NVBDirEntryOperation& operator=(const NVBDirEntryOperation & o);
		NVBDirEntry * e;
		int r,c;
		NVBDirEntry::ContentChangeType t;
		};

	QStack<NVBDirEntryOperation> entryOperationStack;
	
	QList<NVBFileFilter> filters;
	
	NVBFileFactory * fileFactory;
	QFileSystemWatcher * watcher;
	NVBDirModelColumns * columns;
	NVBDirEntry* head;
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

	bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
	bool lessThan(const QModelIndex & left, const QModelIndex & right) const;
	static inline bool variantGreaterOrEqualTo(const NVBVariant & l, const NVBVariant & r) { return l >= r; }
	static inline bool variantLessThan(const NVBVariant & l, const NVBVariant & r) { return l < r;}
	static bool NVBFileInfoLessThan(const NVBFileInfo * fi1, const NVBFileInfo * fi2 );

private slots:
	void watchedDirChanged(const QString & path);
	void beginEntryOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type);
	void endEntryOperation();
	void setEntryRefreshTimer(NVBDirEntry * entry);
	void refreshWatchedDir( QObject * o_entry );
	void notifyFilesLoaded(const NVBDirEntry * entry, int fstart, int fend);

protected slots:
	void sortingStarted(int estimated = -1) const;
	void sortingFinished() const;
	void updateFilters();
	
};

#endif
