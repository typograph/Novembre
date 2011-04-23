//
// C++ Interface: NVBFile
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBFILE_H
#define NVBFILE_H

#include <QtCore/QList>
#include "NVBFileInfo.h"

/**
	\class NVBFile
	An NVBFile is created by NVBFileFactory after a file load request.
	It contains a number of NVBDataSources and global comments.
	*/
class NVBFile : public QObject, public QList<NVBDataSource*> {
	Q_OBJECT
	friend class NVBFileGenerator;

	private:
		NVBAssociatedFilesInfo files;
		int refCount;
		NVBDataComments comments;
    NVBFile();
		NVBFile(QList<NVBDataSource*>);
	public:
		NVBFile(const NVBFile & other):QObject(),QList< NVBDataSource* >(other) {;}
		NVBFile(NVBAssociatedFilesInfo sources): QObject(), QList<NVBDataSource*>(), files(sources), refCount(0) {;}
		NVBFile(NVBAssociatedFilesInfo sources, QList<NVBDataSource*> pages, NVBDataComments file_comments): QObject(), QList<NVBDataSource*>(pages), files(sources), refCount(0), comments(file_comments) {;}

		virtual ~NVBFile();

		/// Returns info about the original files
		NVBAssociatedFilesInfo sources() const { return files; }

		//! \Returns all file-global comments 
		inline NVBDataComments getAllComments() const { return comments; }
		//! Return a file-global comment for \a key
		inline NVBVariant getComment(const QString & key) const { return comments.value(key); }
		//! Return a unified list of comments for all datasources for \a key
		NVBVariant collectComments(const QString & key) const;

		//! Checks for global comment duplicates, \sa NVBConstructableDataSource::filterAddComments
		void filterAddComments(NVBDataComments & comments);
		
		inline QString name() const { return files.name(); }

//		/// Adds a datasource to the end of file.
//		virtual void addSource(NVBDataSource * data);
//		/// Adds data to the end of file.
//		virtual void addSources(QList<NVBDataSource *> pages);
	//  /// Sets an icon for the last page
	//  virtual void setVisualizer(NVBVizUnion visualizer);

		bool inUse() { return refCount != 0; }

	public slots:
		/// Increase the reference count
		virtual void use();
		/// Decrease the reference count
		virtual void release();
	signals:
		/// This signal is emitted when the file is not in use any more (refcount == 0).
		void free(NVBFile *);
};

#endif
