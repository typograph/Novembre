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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QVariant>

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

struct NVBDataInfo {

	NVBDataInfo(const NVBDataSet * source) {
		name = source->name();
		dimension = source->dimension();
		sizes = source->sizes();
		comments = source->comments();
		}

	NVBDataInfo(QString dataName, NVBUnits dataDimension, QVector<axissize_t> dataSizes, NVBDataComments dataComments)
	:	name(dataName)
	, dimension(dataDimension)
	,	sizes(dataSizes)
	, comments( dataComments)
	{;}

	QString name;
	NVBUnits dimension;
	QVector<axissize_t> sizes;
	NVBDataComments comments;
};

class NVBFileInfo {
	private:
	public:
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(QString name) { files = NVBAssociatedFilesInfo(name); }
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(NVBAssociatedFilesInfo info):files(info) {;}
		/// Generate a NVBFileInfo from existing NVBFile \a file
		NVBFileInfo(const NVBFile * const file);
		virtual ~NVBFileInfo() {;}

		NVBAssociatedFilesInfo files;
		QList<NVBDataInfo> dataInfos;
		NVBDataComments comments;

		NVBVariant getInfo(const NVBTokens::NVBTokenList & list) const;
		QString getInfoAsString(const NVBTokens::NVBTokenList & list) const;

	protected :
		NVBVariant fileParam(NVBTokens::NVBFileParamToken::NVBFileParam p) const;
		NVBVariant pageParam(NVBDataInfo pi, NVBTokens::NVBPageParamToken::NVBPageParam p) const;
};

#endif
