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

#include <QtCore/QString>
#include <QtCore/QRectF>

#include "NVBFileInfo.h"

/**
	\class NVBFile
	An NVBFile is created by NVBFileFactory after a file load request.
	It contains a number of NVBDataSources and global comments.
	*/
class NVBFile : public QList<NVBDataSource*>{
	Q_OBJECT
	friend class NVBFileGenerator;

	private:
		NVBAssociatedFilesInfo files;
		int refCount;
		NVBDataComments comments;
	public:
		/// Constructs an empty file with \a source
		NVBFile(NVBAssociatedFilesInfo sources):sourceInfo(sources),refCount(0) {;}
		NVBFile(NVBAssociatedFilesInfo sources, QList<NVBDataSource*> pages);
		virtual ~NVBFile();

		/// Returns info about the original files
		NVBAssociatedFilesInfo sources() const { return files; }

		inline NVBDataComments getAllComments() { return comments; }

		inline QString name() const { return files.name(); }

		/// Adds a datasource to the end of file.
		virtual void addSource(NVBDataSource * data);
		/// Adds data to the end of file.
		virtual void addSources(QList<NVBDataSource *> pages);
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
//		/// This signal is emitted when the file is deleted. \a filename is the name of this file.
//		void free(QString name);
};

#endif
