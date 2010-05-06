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

#include <QApplication>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QLibrary>
//#include <dlfcn.h>

#include "NVBLogger.h"
#include "NVBFile.h"
#include "NVBFileInfo.h"
#include "NVBFileGenerator.h"

using namespace NVBErrorCodes;

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
	/// If a file with \a filename is in the queue, it is returned without removing it from the queue
		virtual NVBFile * consult(QString filename);
};

class NVBFileLoader : public QObject {
Q_OBJECT
public:
	NVBFileLoader(QString dir);

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
	
	/// Find file in list by name
		NVBFile * retrieveLoadedFile( QString filename );

	/// Available generators
		QList<const NVBFileGenerator*> generators;

	/// Load file from \a filename. Returns NULL if file wasn't opened.
	/// The returned file is already considered in use.
		inline NVBFile* loadFile( QString filename ) { return loadFile(associatedFiles(filename)); }
		inline NVBFile* loadFile( const NVBAssociatedFilesInfo & info );

	/// Load only info from \a filename. 
		inline NVBFileInfo* loadFileInfo( QString filename ) { return loadFileInfo(associatedFiles(filename)); }
		inline NVBFileInfo* loadFileInfo( const NVBAssociatedFilesInfo & info )  { return info.loadFileInfo(); }

	/// Cache of parameters available from generators
		QStringList commentNames;

	/// Allowed filename wildcards provided by generators
		QMultiMap<QRegExp, const NVBFileGenerator*> wildcards;

	/// \returns a list of all generators with wildcards matching \a filename
		QList<const NVBFileGenerator*> getGeneratorsFromFilename(QString filename) const;
	
public:
	NVBFileFactory();
	virtual ~NVBFileFactory();

	/// Checks all caches for file \a filename, if found, returns it, if not, uses \a loadFile
	/// @param filename Name of the file to be open
		inline NVBFile* openFile( QString filename ) { return openFile(associatedFiles(filename)); }
		NVBFile* openFile( const NVBAssociatedFilesInfo & info );

	/// Load only info from \a filename. Reuses loaded files if any
	/// @param filename Name of the file for the info to be read from.
	/// @return info from file \a filename
		inline NVBFileInfo* getFileInfo( QString filename );
		inline NVBFileInfo* getFileInfo( const NVBAssociatedFilesInfo & info );

	/// Generates a string of openable files in the format acceptable by \c QDir
		QStringList getDirFilters() const;
		inline QString getDirFilter() const { return getDirFilters().join(";"); }
	/// Generates a string of openable files in the format acceptable by \c QFileDialog
		QString getDialogFilter();

	/// Returns possible comment fields
		QStringList availableInfoFields() const { return commentNames; }
	/// Returns info about files associated with name \a filename
		NVBAssociatedFilesInfo associatedFiles(QString filename) const;

	/// Return the smallest possible list of all file associations in folder.
	/// Files in \a files will be taken into account and won't be reloaded
		void associatedFiles(const QDir & d, QList<NVBAssociatedFilesInfo> & files) const;

private slots:
	/// Put file in the dead tree
		void bury(NVBFile *);
	/// Removes the file associated with \a filename from all caches
		void release(QString filename);
};

Q_DECLARE_METATYPE(NVBFileFactory*);

#endif