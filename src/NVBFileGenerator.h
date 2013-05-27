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

#ifndef NVBFILEGENERATOR_H
#define NVBFILEGENERATOR_H

#include <QtCore/QtPlugin>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "NVBFileInfo.h"

class NVBFile;
class NVBSettingsWidget;

/**
  * NVBFileGenerator is a base class for loading physical files into internal data format
  * This class is exclusively used by NVBFileFactory and does not figure anywhere else.
  * While subclassing, don't forget to make your class a descendant of QObject
  *
  * When implementing \a availableInfoFields(), no optimization and/or
  * caching is necessary, since this function is unlikely
  * to be called more than once in the lifetime of the generator
  */

class NVBFileGenerator {

	public:
		virtual ~NVBFileGenerator() {;}

		/// A human-readable name of the module, e.g. "Omicron STM files"
		virtual QString moduleName() const = 0;
		/// A thourough description of the module
		virtual QString moduleDesc() const = 0;
#ifndef FILEGENERATOR_NO_GUI
		/// A configuration page for plugin features (if any)
		virtual NVBSettingsWidget* configurationPage() const { return 0; }
#endif

		/// Get the nameFilter for use in QFileDialog
		virtual QString nameFilter() const {
			return QString("%1 (%2)").arg(moduleName(), extFilters().join(" "));
			}

		/// The list of supported filenames, using wildcards \sa QDir::setNameFilers. Filters are case-insensitive
		virtual QStringList extFilters() const = 0; // wildcards

		/// Load full file data from \param file
		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw() = 0;
		/// Load file information from \param info
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() = 0;

		/// Possible info keys in comments (used in user column format)
		virtual QStringList availableInfoFields() const = 0;
		/// Files that will be open on loading this file (including the file itself)
		virtual inline NVBAssociatedFilesInfo associatedFiles(QString filename) const {
			return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), QStringList(filename), this);
			}

	};

// NVBFileGenerator interface should have the same major version as Novembre itself
// Minor version should be the same or smaller
Q_DECLARE_INTERFACE(NVBFileGenerator, "com.novembre.fileGenerator/0.1.0");

#endif
