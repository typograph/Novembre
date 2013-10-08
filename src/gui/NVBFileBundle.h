//
// Copyright 2010 - 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef NVBFILEBUNDLE_H
#define NVBFILEBUNDLE_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include "NVBFileGenerator.h"
#include "NVBFileInfo.h"

class NVBFileFactory;
class NVBFile;

class NVBFileBundle: public NVBFileGenerator {
// 		Q_OBJECT
// 		Q_INTERFACES(NVBFileGenerator)

	private:
		NVBFileFactory * const fileFactory;

	public:
		NVBFileBundle(NVBSettings settings, NVBFileFactory * ff ) : NVBFileGenerator(settings), fileFactory(ff) {;}
		virtual ~NVBFileBundle() {;}

		virtual inline QString moduleName() const { return QString("Novembre bundle files"); }
		virtual inline QString moduleDesc() const { return QString("Novembre file format for bundling together files that normally would be opened separately."); }

		virtual QStringList extFilters() const {
			static QStringList exts = QStringList() << "*.nbl";
			return exts;
			}

		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

		virtual QStringList availableInfoFields() const { return QStringList(); }
		virtual NVBAssociatedFilesInfo associatedFiles(QString filename) const;
	};

#endif
