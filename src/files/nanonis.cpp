//
// Copyright 2006 Timofey <typograph@elec.ru>
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
#include "nanonis.h"
#include "../core/dimension.h"
#include "../core/NVBContColoring.h"
#include "../core/NVBDiscrColoring.h"
#include <QDir>
#include "../core/NVBFileInfo.h"
#include "../core/NVBFile.h"
#include <QDebug>
#include <QRectF>

QStringList NanonisFileGenerator::availableInfoFields() const {
	return QStringList() \
	       << "Bias" \
	       << "User comment" \
	       << "Line time" \
	       ;
	}

NVBAssociatedFilesInfo NanonisFileGenerator::associatedFiles(QString filename) const {
	return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), QStringList(filename), this);
	}

NVBFile * NanonisFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	if (info.first().right(3) == "dat")
		return loadSpectrum(info);

	if (info.first().right(3) == "3ds")
		return load3DS(info);

	if (info.first().right(3) == "sxm")
		return loadSXM(info);

	NVBOutputError("Unknown Nanonis datafile type");
	return 0;
	}

NVBFileInfo * NanonisFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	if (info.count() > 1 || info.first().right(3) == "dat")
		return loadSpectrumInfo(info);

	if (info.first().right(3) == "3ds")
		return load3DSInfo(info);

	if (info.first().right(3) == "sxm")
		return loadSXMInfo(info);

	NVBOutputError("Unknown Nanonis datafile type");
	return 0;
}


Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
