#ifndef NVBDIRMODELHELPERS_H
#define NVBDIRMODELHELPERS_H

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

#if QT_VERSION >= 0x040400
template <class T>
	class QFutureWatcher;
#endif

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

class NVBDirEntry : public QObject {
Q_OBJECT
public:
	enum ContentChangeType { FolderInsert, FolderRemove, FileInsert, FileRemove };
	enum WatchedContentType { NoContent, FileContent, AllContent };
	enum Status { Virgin, Populated, Loaded, Error };
	NVBDirEntry * parent;
	QString label;
	QDir dir;
	QList<NVBDirEntry*> folders;

	inline const NVBFileInfo * fileAt(int i) { return files.at(indexMap.at(i)); }
	inline int fileCount() const { return indexMap.count(); }
	/// Removes a file from the full list using mapped index
	void removeFileAt(int i);

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
	bool isPopulated() const { return status == NVBDirEntry::Populated || status == NVBDirEntry::Loaded; }
	bool isLoaded() const { return status == NVBDirEntry::Loaded; }
	Status getStatus() const { return status; }
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

private:
	void recurseFolders();
	/// inserts a new file, with filtering and sorting
	void insertFile(NVBFileInfo * file);
	/// Removes a file from the full list using full index
	void removeOrigFileAt(int i);
	/// Does a non-recursive filtering of the list, not notifying the model (it is in LayoutChanged mode)
	void refilter();
signals:
	void beginOperation(const NVBDirEntry * entry, int row, int count, NVBDirEntry::ContentChangeType type);
	void endOperation();
	void filesLoaded(const NVBDirEntry * entry, int fstart, int fend);
private slots:
	void notifyLoading(int start, int end);
	void setLoaded() { status = NVBDirEntry::Loaded; qApp->restoreOverrideCursor();}
public slots:
	bool refresh(NVBFileFactory * fileFactory);
	void refreshSubfolders(NVBFileFactory * fileFactory);
	void sort(const NVBDirModelFileInfoLessThan & lessThan);
	void filter(const NVBDirModelFileInfoFilter & acceptFile);
};

#endif
