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

//#include <QtGui/QApplication>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
//#include <QtCore/QDir>
//#include <QtCore/QLibrary>
//#include <QtCore/QThread>
//#include <QtCore/QEvent>
#include <QtCore/QSignalMapper>
//#include <dlfcn.h>

#include <QtCore/QFuture>
#include <QtCore/QtConcurrentRun>

#include "NVBLogger.h"
#include "NVBFile.h"
#include "NVBFileInfo.h"
#include "NVBFileGenerator.h"

class QAction;

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
	/// If queue length goes over the limit at that point, the oldest entry in the queue is deleted
		virtual void add(NVBFile* file);
	/// If a file with \a filename is in the queue, it is removed from the queue and returned
		virtual NVBFile * retrieve(QString filename);
		virtual NVBFile * retrieve(const NVBAssociatedFilesInfo & info);
	/// If a file with \a filename is in the queue, it is returned without removing it from the queue
		virtual NVBFile * consult(QString filename);
		virtual NVBFile * consult(const NVBAssociatedFilesInfo & info);
};


/**
 * \class NVBFileFactory
 *
 * This class is responsible for creating NVBFile and NVBFileInfo objects.
 * It collects information about provided file plugins and creates NVBFile's.
 * To save up on disk operations, opened files go to a stack.
 */
class NVBFileFactory : public QObject {
Q_OBJECT
private:
	/// Files that are not used by any window are cached in this structure
		NVBFileQueue * deadTree;

	/// Loaded files in use
		QList<NVBFile *> files;
		
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
		inline NVBFile* openFile( QString filename, bool track = true ) { return loadFile(associatedFiles(filename),track); }
		NVBFile* openFile( const NVBAssociatedFilesInfo & info, bool track = true );

		NVBFileInfo* openFileInfo( QString filename ) const;
		inline NVBFileInfo* openFileInfo( const NVBAssociatedFilesInfo & info ) const
			{ return info.loadFileInfo(); }

	/// Cache of parameters available from generators
		QStringList commentNames;

	/// Allowed filename wildcards provided by generators
		QMultiMap<QString, const NVBFileGenerator*> wildcards;

	/// \returns a list of all generators with wildcards matching \a filename
		QList<const NVBFileGenerator*> getGeneratorsFromFilename(QString filename) const;
	
public:
	NVBFileFactory();
	virtual ~NVBFileFactory();

	/**
	 * Blocking version of loadFile
	 * 
	 */
		NVBFile* getFile( QString filename, bool track = true );
		NVBFile* getFile( const NVBAssociatedFilesInfo & info, bool track = true );
//		void openFile( const NVBAssociatedFilesInfo & info, const QObject * receiver );

	/**
	 * Asyncronously loads a file.
	 * 
	 * Checks all caches for file \a filename, if found, returns it, if not, uses \a loadFile
	 * @param filename Name of the file to be open
	 *
	 */
		QFuture<NVBFile*> loadFile( QString filename, bool track = true )
			{ return QtConcurrent::run(this, static_cast<NVBFile * (NVBFileFactory::*)(QString, bool)>(&NVBFileFactory::getFile),filename,track); }
	/**
	 * \overload
	 * 
	 */
		QFuture<NVBFile*> loadFile( const NVBAssociatedFilesInfo & info, bool track = true )
			{ return QtConcurrent::run(this, static_cast<NVBFile * (NVBFileFactory::*)(const NVBAssociatedFilesInfo &, bool)>(&NVBFileFactory::getFile),info,track); }

	/**
	 * Blocking version of loadFileInfo
	 */
	/// Load only info from \a filename. 
		NVBFileInfo* getFileInfo( QString filename );
		NVBFileInfo* getFileInfo( const NVBAssociatedFilesInfo & info );

	/** 
	 * Asyncronously loads a fileInfo object
	 * 
	 * Load only info from \a filename. Reuses loaded files if any
	 * @param filename Name of the file for the info to be read from.
	 * @return info from file \a filename
	 * 
	 */
		QFuture<NVBFileInfo*> loadFileInfo( QString filename )
			{ return QtConcurrent::run(this, static_cast<NVBFileInfo * (NVBFileFactory::*)(QString)>(&NVBFileFactory::getFileInfo),filename); }
		QFuture<NVBFileInfo*> loadFileInfo( const NVBAssociatedFilesInfo & info )
			{ return QtConcurrent::run(this, static_cast<NVBFileInfo * (NVBFileFactory::*)(const NVBAssociatedFilesInfo &)>(&NVBFileFactory::getFileInfo),info); }
	

	/// Returns info about files associated with name \a filename
		NVBAssociatedFilesInfo associatedFiles(QString filename) const;
		
	/// Generates a string of openable files in the format acceptable by \c QDir
		QStringList getDirFilters() const;
		inline QString getDirFilter() const { return getDirFilters().join(";"); }
	/// Generates a string of openable files in the format acceptable by \c QFileDialog
		QString getDialogFilter();

	/// Returns possible comment fields
		QStringList availableInfoFields() const { return commentNames; }
		
	/// 
		QList<QAction*> generatorActions() const { return gActions;}

private slots:
	/// Put file in the dead tree
		void bury(NVBFile *);
	/// Removes the file associated with \a filename from all caches
		void release(QString filename);
	/// Enables/disables a generator. The object will be cast to NVBFileGenerator
		void changeGenerator(QObject * go);
	/// Updates wildcard list
		void updateWildcards();
};

Q_DECLARE_METATYPE(NVBFileFactory*);

#endif
