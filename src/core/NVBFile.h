//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

		// Private constructors - nobody should use them
		NVBFile();
		explicit NVBFile(const QList<NVBDataSource*> & list);
		NVBFile(const NVBFile & other);
		NVBFile & operator=(const NVBFile & other);

	public:
		NVBFile(NVBAssociatedFilesInfo sources): QObject(), QList<NVBDataSource*>(), files(sources), refCount(0) {;}
		NVBFile(NVBAssociatedFilesInfo sources, QList<NVBDataSource*> pages, NVBDataComments file_comments): QObject(), QList<NVBDataSource*>(pages), files(sources), refCount(0), comments(file_comments) {;}

		virtual ~NVBFile();

		/// Returns info about the original files
		NVBAssociatedFilesInfo sources() const { return files; }

		//! \return all file-global comments
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
