//
// Copyright 2006 Timofey <typograph@elec.ru>
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

#include <QtCore/QString>
#include <QtCore/QRectF>

#include "NVBFileInfo.h"
#include "NVBPages.h"
#include "NVBPageViewModel.h"

/**
\class NVBFile
An NVBFile is created by NVBFileFactory after a file load request.
It is a model with pages.
*/
class NVBFile : public NVBPageViewModel {
		Q_OBJECT
		friend class NVBFileGenerator;

	private:
		NVBAssociatedFilesInfo sourceInfo;
		int refCount;
//  friend class NVBFileFactory;
	public:
		/// Constructs an empty file with \a source
		NVBFile(NVBAssociatedFilesInfo sources): NVBPageViewModel(), sourceInfo(sources), refCount(0) {;}
		NVBFile(NVBAssociatedFilesInfo sources, QList<NVBDataSource*> pages);
		virtual ~NVBFile();

		/// Returns info about the original files
		NVBAssociatedFilesInfo sources() const { return sourceInfo; }

		/// Return information about page at @p index
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

		/// \returns the total area occupied by all pages in the x-y plane
		QRectF fullarea();
		/// \returns the name of the file
		QString name() const { return sourceInfo.name(); }

//  /// helper function
//   static QVariant pageData(NVBDataSource* page, int role);

		/// Adds a page to the end of file.
		virtual void addSource(NVBDataSource * page);
		/// Adds pages to the end of file.
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
		/// This signal is emitted when the file is deleted. \a filename is the name of this file.
		void free(QString name);
	};

#endif
