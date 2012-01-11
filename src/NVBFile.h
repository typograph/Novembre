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
#include "NVBPages.h"
#include "NVBPageViewModel.h"

/**
\class NVBFile
An NVBFile is created by NVBFileFactory after a file load request.
It is a model with pages.
*/
class NVBFile : public NVBPageViewModel
{
Q_OBJECT
friend class NVBFileGenerator;

private:
  NVBAssociatedFilesInfo sourceInfo;
  int refCount;
//  friend class NVBFileFactory;
public:
	/// Constructs an empty file with \a source
	NVBFile(NVBAssociatedFilesInfo sources):NVBPageViewModel(),sourceInfo(sources),refCount(0) {;}
	NVBFile(NVBAssociatedFilesInfo sources, QList<NVBDataSource*> pages);
  virtual ~NVBFile();

	/// Returns info about the original files
	NVBAssociatedFilesInfo sources() const { return sourceInfo; }

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
