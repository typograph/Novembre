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
		comments = source->getAllComments();

		if (type == NVB::TopoPage) {
			const NVB3DDataSource * tsource = qobject_cast<const NVB3DDataSource*>(source);

			if (tsource) {
				datasize = tsource->resolution();
				xSpan = NVBPhysValue(fabs(tsource->position().width()), tsource->xDim());
				ySpan = NVBPhysValue(fabs(tsource->position().height()), tsource->yDim());
				}
			}
		else if (type == NVB::SpecPage) {
			const NVBSpecDataSource * ssource = qobject_cast<const NVBSpecDataSource*>(source);

			if (ssource) {
				datasize = ssource->datasize();
				xSpan = NVBPhysValue(fabs(ssource->boundingRect().width()), ssource->tDim());
				}
			}
		else
			datasize = QSize();
		}

	NVBPageInfo(QString _name, NVB::PageType _type, QSize size, NVBPhysValue xspan, NVBPhysValue yspan, QMap<QString, NVBVariant> _comments)
		:	name(_name)
		, type(_type)
		,	datasize(size)
		, xSpan(xspan)
		, ySpan(yspan)
		, comments(_comments)
		{;}

	QString name;
	NVB::PageType type;
	QSize datasize;
	NVBPhysValue xSpan, ySpan;
	QMap<QString, NVBVariant> comments;
	};

class NVBFileInfo { /*: public QObject */
// Q_OBJECT
	public:
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(QString name) { files = NVBAssociatedFilesInfo(name); }
		/// Generates an invalid NVBFileInfo
		NVBFileInfo(NVBAssociatedFilesInfo info): files(info) {;}
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
