//
// C++ Interface: NVBFileFactory
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBFACTORY_H
#define NVBFACTORY_H

#include <QtGui/QApplication>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QLibrary>
#include <QtCore/QThread>
#include <QtCore/QEvent>
#include <QtCore/QSignalMapper>
//#include <dlfcn.h>

#include "NVBLogger.h"
#include "NVBFile.h"
#include "NVBFileInfo.h"
#include "NVBFileGenerator.h"

class QAction;

// using namespace NVBErrorCodes;

class NVBFileLoadEvent : public QEvent {
public:
	QString name;
	NVBFile * file;
	NVBFileLoadEvent(QString n, NVBFile * f):QEvent(QEvent::User),name(n),file(f) {;}
};



class NVBFileQueue : protected QList<NVBFile*> {
private:
	int depth;
public:
	/// Constructs a fixed length queue
	/// \param queue_depth : maximal queue length
		NVBFileQueue(int queue_depth);
	/// Destroys NVBFileQueue
		virtual ~NVBFileQueue();

	/// Add a file \a file to the queue.
	/// If queue length goes over the limit at that point, the oldest enrty in the queue is deleted
		virtual void add(NVBFile* file);
	/// If a file with \a filename is in the queue, it is removed from the queue and returned
		virtual NVBFile * retrieve(QString filename);
		virtual NVBFile * retrieve(const NVBAssociatedFilesInfo & info);
	/// If a file with \a filename is in the queue, it is returned without removing it from the queue
		virtual NVBFile * consult(QString filename);
		virtual NVBFile * consult(const NVBAssociatedFilesInfo & info);
};

class NVBFileLoader : public QThread {
Q_OBJECT
public:
	NVBAssociatedFilesInfo info;
	NVBFile * file;
	QList<QObject *> customers;
	NVBFileLoader(const NVBAssociatedFilesInfo & i, QObject * r):info(i),file(0),customers( QList<QObject*>() << r ) {;}
	virtual void run();
};

/**
  This class is responsible for creating NVBFile and NVBFileInfo objects.
  It collects information about provided file plugins and creates NVBFile's.
  To save up on disk operations, opened files go to a stack.
*/
class NVBFileFactory : public QObject{
Q_OBJECT
private:
	/// Files that are not used by any window are cached in this structure
		NVBFileQueue * deadTree;

	/// Loaded files in use
		QList<NVBFile *> files;
	
	/// Loader threads
		QList<NVBFileLoader *> loaders;

	/// Check if a loader for a file is created already
		NVBFileLoader * getLoaderInProgress(const NVBAssociatedFilesInfo & info);

	/// Find file in list by name
		int loadedFileIndex( QString filename );
		NVBFile * retrieveLoadedFile( QString filename );
		NVBFile * retrieveLoadedFile( const NVBAssociatedFilesInfo & info);

	/// Available generators (selected by user)
		QList<const NVBFileGenerator*> generators;
	/// All available generators
		QList<const NVBFileGenerator*> allGenerators;
	///
		QList<QAction*> gActions;
		QSignalMapper actMapper;

	/// Load file from \a filename. Returns NULL if file wasn't opened.
	/// The returned file is already considered in use.
		inline NVBFile* loadFile( QString filename ) { return loadFile(associatedFiles(filename)); }
		NVBFile* loadFile( const NVBAssociatedFilesInfo & info );

	/// Load only info from \a filename. 
		inline NVBFileInfo* loadFileInfo( QString filename ) const { return loadFileInfo(associatedFiles(filename)); }
		inline NVBFileInfo* loadFileInfo( const NVBAssociatedFilesInfo & info ) const { return info.loadFileInfo(); }

	/// Cache of parameters available from generators
		QStringList commentNames;

	/// Allowed filename wildcards provided by generators
		QMultiMap<QString, const NVBFileGenerator*> wildcards;

	/// \returns a list of all generators with wildcards matching \a filename
		QList<const NVBFileGenerator*> getGeneratorsFromFilename(QString filename) const;
	
public:
	NVBFileFactory();
	virtual ~NVBFileFactory();

	/// Checks all caches for file \a filename, if found, returns it, if not, uses \a loadFile
	/// @param filename Name of the file to be open
		NVBFile* openFile( QString filename );
		NVBFile* openFile( const NVBAssociatedFilesInfo & info );
		void openFile( const NVBAssociatedFilesInfo & info, const QObject * receiver );

	/// Load only info from \a filename. Reuses loaded files if any
	/// @param filename Name of the file for the info to be read from.
	/// @return info from file \a filename
		NVBFileInfo* getFileInfo( QString filename );
		NVBFileInfo* getFileInfo( const NVBAssociatedFilesInfo & info );

	/// Generates a string of openable files in the format acceptable by \c QDir
		QStringList getDirFilters() const;
		inline QString getDirFilter() const { return getDirFilters().join(";"); }
	/// Generates a string of openable files in the format acceptable by \c QFileDialog
		QString getDialogFilter();

	/// Returns possible comment fields
		QStringList availableInfoFields() const { return commentNames; }
		
	/// Returns info about files associated with name \a filename
		NVBAssociatedFilesInfo associatedFiles(QString filename) const;
	/// Return the smallest possible list of all new file associations in folder.
	/// Files in \a files will be taken into account and won't be reloaded
		QList<NVBAssociatedFilesInfo> associatedFilesFromDir(const QDir & d, QList<NVBAssociatedFilesInfo> * old_files = 0, QList<int> * deleted = 0 ) const;
	/// Return the smallest possible set of files loaded from the supplied list.
	/// Files in \a files will be taken into account and won't be reloaded
		QList<NVBAssociatedFilesInfo> associatedFilesFromList(QStringList names, QList<NVBAssociatedFilesInfo> * old_files = 0, QList<int> * deleted = 0 ) const;

	/// 
		QList<QAction*> generatorActions() const { return gActions;}

private slots:
	/// Put file in the dead tree
		void bury(NVBFile *);
	/// Removes the file associated with \a filename from all caches
		void release(QString filename);
	/// Removes a finished loader
		void removeLoader();
	/// Enables/disables a generator. The object will be cast to NVBFileGenerator
		void changeGenerator(QObject * go);
	/// Updates wildcard list
		void updateWildcards();
};

Q_DECLARE_METATYPE(NVBFileFactory*);

#endif
