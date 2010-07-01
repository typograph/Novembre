//
// C++ Interface: NVBFileWrappers
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBFILEINFO_H
#define NVBFILEINFO_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QVariant>

#include "NVBDataSource.h"
#include "NVBTokens.h"

class NVBFile;
class NVBFileGenerator;
class NVBFileInfo;

class NVBAssociatedFilesInfo : public QStringList {

private:
	/// The name under which the collection has to appear
		QString _name;
	/// Generator used to get this structure
		const NVBFileGenerator * _generator;

public:
	/// Constructs an invalid associated files info
		explicit NVBAssociatedFilesInfo(QString n = QString()):QStringList(),_name(n),_generator(0){;}
	/// Constructs an associated files info from provided parameters
		NVBAssociatedFilesInfo(QString n, QStringList f, const NVBFileGenerator * g):QStringList(f),_name(n),_generator(g) {;}
	/// Wrapper for generator functions
		NVBFileInfo * loadFileInfo() const;
	// There's no wrapper for the loadFile, because file counting should be implemented in NVBFileFactory,
	// and thus we whould have to make either generators or files factory-aware, and that goes a long way.

		inline QString name() const { return _name; }
		inline const NVBFileGenerator * generator() const { return _generator; }

	/// If the file is in associated files. Will resolve symlinks.
		inline bool contains(QString filename) const { return QStringList::contains(QDir::cleanPath(filename)); }

		bool operator==(const NVBAssociatedFilesInfo & other) const;
		bool operator!=(const NVBAssociatedFilesInfo & other) const {
			return !(*this == other);
		}

};


/*
  I have decided not to make NVBFileInfo a QObject.
  NVBFileFactory will _not_ reserve the files for info.

  I am doing this mainly for reasons of long-term memory
  salvation. Otherwise each file present in a browser
  will remain in memory till the browser is closed.
  If the data was loaded for these files (the user had
  a look on the data), it will mean megabytes at least.

  Thus, there is currently no need for signals.
*/

struct NVBPageInfo {

	NVBPageInfo(const NVBDataSource * source) {
		name = source->name();
		type = source->type();
		if (type == NVB::TopoPage)
			datasize = ((NVB3DDataSource*)source)->resolution();
		else if (type == NVB::SpecPage)
			datasize = ((NVBSpecDataSource*)source)->datasize();
		else
			datasize = QSize();
		comments = source->getAllComments();
		}

	NVBPageInfo(QString _name, NVB::PageType _type, QSize size, QMap<QString,NVBVariant> _comments)
	:	name(_name)
	, type(_type)
	,	datasize(size)
	, comments( _comments)
	{;}

	QString name;
	NVB::PageType type;
	QSize datasize;
	QMap<QString,NVBVariant> comments;
};

class NVBFileInfo /*: public QObject */{
// Q_OBJECT
public:
	/// Generates an invalid NVBFileInfo
	NVBFileInfo(QString name) { files = NVBAssociatedFilesInfo(name); }
	/// Generates an invalid NVBFileInfo
	NVBFileInfo(NVBAssociatedFilesInfo info):files(info) {;}
	/// Generate a NVBFileInfo from existing NVBFile \a file
	NVBFileInfo(const NVBFile * const file);
	virtual ~NVBFileInfo() {;}

	NVBAssociatedFilesInfo files;
	QList<NVBPageInfo> pages;
	
	bool hasTopo();
	bool hasSpec();
	
	NVBVariant getInfo(const NVBTokens::NVBTokenList & list) const;
	QString getInfoAsString(const NVBTokens::NVBTokenList & list) const;
	
protected :
	NVBVariant fileParam(NVBTokens::NVBFileParamToken::NVBFileParam p) const;
	NVBVariant pageParam(NVBPageInfo pi, NVBTokens::NVBPageParamToken::NVBPageParam p) const;
};

#endif
