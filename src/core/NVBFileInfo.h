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
#include "NVBAxisMaps.h"

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
		explicit NVBAssociatedFilesInfo(QString n = QString()): QStringList(), _name(n), _generator(0) {;}
		/// Constructs an associated files info from provided parameters
		NVBAssociatedFilesInfo(QString n, QStringList f, const NVBFileGenerator * g): QStringList(f), _name(n), _generator(g) {;}
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
			return !operator==(other);
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

struct NVBAxisInfo {
	QString name;
	axissize_t length;
	NVBPhysValue span;

	NVBAxisInfo(QString n, axissize_t l, NVBPhysValue s = NVBPhysValue())
		: name(n)
		, length(l)
		, span(s.getDimension().isValid() ? s.abs() : NVBPhysValue(l, NVBUnits()))
		{;}
	};

struct NVBDataInfo {

	NVBDataInfo(const NVBDataSet * source) {
		name = source->name();
		type = source->type();
		dimension = source->dimension();

		for(axisindex_t i = 0; i < source->nAxes(); i++) {
			const NVBAxis & a = source->axisAt(i);
			axes << NVBAxisInfo(a.name(), a.length(), a.physMap() ? (a.physMap()->value(a.length() - 1) - a.physMap()->value(0)).abs() : NVBPhysValue());
			}

		comments = source->getAllComments();
		}

	NVBDataInfo(QString dataName, NVBUnits dataDimension, QList<NVBAxisInfo> dataAxes, NVBDataComments dataComments, NVBDataSet::Type dataType = NVBDataSet::Undefined)
		:	name(dataName)
		, type(dataType)
		, dimension(dataDimension)
		,	axes(dataAxes)
		, comments( dataComments)
		{;}

	QString name;
	NVBDataSet::Type type;
	NVBUnits dimension;
	QList<NVBAxisInfo> axes;
	NVBDataComments comments;
	};

class NVBFileInfo : public QList<NVBDataInfo> {
	private:
	public:
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(QString name) { files = NVBAssociatedFilesInfo(name); }
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(NVBAssociatedFilesInfo info): files(info) {;}
		/// Generate a NVBFileInfo from existing NVBFile \a file
		NVBFileInfo(const NVBFile * const file);
		virtual ~NVBFileInfo() {;}

		NVBAssociatedFilesInfo files;
		NVBDataComments comments;

/// Gets a comment for a given key, taking into account individual datasets
		/**
		 * Note, that where NVBFile has bottom-up search recursion,
		 * NVBFile info does it top-down
		 */
		NVBVariant getComment(const QString & key);
		NVBDataComments getAllComments() { return comments; }
		void filterAddComments(NVBDataComments & newComments);

		NVBVariant getInfo(const NVBTokens::NVBTokenList & list) const;
		QString getInfoAsString(const NVBTokens::NVBTokenList & list) const;

	protected :
		NVBVariant fileParam(NVBTokens::NVBFileParamToken::NVBFileParam p) const;
		NVBVariant dataParam(const NVBDataInfo& pi, NVBTokens::NVBDataParamToken::NVBDataParam p) const;
		NVBVariant axisParam(const NVBDataInfo& pi, QString name, NVBTokens::NVBAxisParamToken::NVBAxisParam p) const;
		NVBVariant axisParam(const NVBDataInfo& pi, int index, NVBTokens::NVBAxisParamToken::NVBAxisParam p) const;
	};

#endif
