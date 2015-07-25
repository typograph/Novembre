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
#include "../core/NVBContColoring.h"
#include "../core/NVBFileInfo.h"
#include "../core/NVBFile.h"
//#include <QDebug>

NanonisHeader NanonisFileGenerator::getNanonisHeader(QFile & file) {
	NanonisHeader h;

	QString key;
	QString tmp;
	QStringList value;

	while (true) {
		tmp = QString(file.readLine(100)).trimmed();

		if (tmp.isEmpty())
			continue;
		else if (tmp.at(0) != ':') {
			value << tmp;
			}
		else {
			h.insert(key, value);

			if (tmp == ":SCANIT_END:") break;

			value.clear();
			key = tmp.mid(1, tmp.length() - 2);
			}
		}

	file.read(4); // 0x0a 0x0a 0x1a 0x04 -- Whatever that means

	return h;
	}

NVBFile * NanonisFileGenerator::loadSXM(const NVBAssociatedFilesInfo & info) const {

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	if (!QString(file.readLine(100)).contains("NANONIS", Qt::CaseInsensitive)) {
		NVBOutputPMsg("The file doesn't have nanonis format");
		return 0;
		}

	NVBFile * f = new NVBFile(info);

	if (!f) return 0;

	NanonisHeader h = getNanonisHeader(file);

	QStringList l = h.value("DATA_INFO");
	QStringList headers = l.takeFirst().split('\t'); // Headers

	while (l.last().isEmpty()) l.removeLast();

	int dirIndex = headers.indexOf("Direction");

	// Now we know the number of pages
	foreach(QString s, l) {
		QStringList e = s.split('\t');

		if (e.at(dirIndex) == "both") {
			f->addSource(new NanonisPage(file, h, headers, e));
			f->addSource(new NanonisPage(file, h, headers, e, true));
			}
		else
			f->addSource(new NanonisPage(file, h, headers, e));
		}

	file.close();

	return f;
	}

NVBFileInfo * NanonisFileGenerator::loadSXMInfo(const NVBAssociatedFilesInfo & info) const{

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	if (!QString(file.readLine(100)).contains("NANONIS", Qt::CaseInsensitive))
		return 0;

	NanonisHeader h = getNanonisHeader(file);
	file.close();

	if (!h.contains("DATA_INFO")) {
		NVBOutputError("File format error: no DATA_INFO field");
		return 0;
		}

	QStringList l = h.value("DATA_INFO");

	if (l.isEmpty()) {
		NVBOutputError("File format error: DATA_INFO field empty");
		return 0;
		}

	QStringList headers = l.takeFirst().split('\t'); // Headers

	while (l.last().isEmpty()) l.removeLast();

	int dirIndex = headers.indexOf("Direction");

	// Now we know the number of pages

	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) {
		NVBOutputError("NVBFileInfo allocation failure");
		return 0;
		}

	foreach(QString s, l) {
		QStringList e = s.split('\t');

		QSize psize;

		if (h.contains("SCAN_PIXELS") && !h.value("SCAN_PIXELS").isEmpty()) {
			QStringList s_size = h.value("SCAN_PIXELS").first().split(' ', QString::SkipEmptyParts);
			psize = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());
			}

		QMap<QString, NVBVariant> comments;

		if (h.contains("BIAS") && !h.value("BIAS").isEmpty())
			comments.insert("Bias", NVBPhysValue(h.value("BIAS").first().toDouble(), NVBDimension("V")));

		if (h.contains("COMMENT") && !h.value("COMMENT").isEmpty())
			comments.insert("User comment", h.value("COMMENT").join(" "));

		if (h.contains("SCAN_TIME") && !h.value("SCAN_TIME").isEmpty())
			comments.insert("Line time", NVBPhysValue(h.value("SCAN_TIME").first().split(' ', QString::SkipEmptyParts).first().toDouble(),NVBDimension("s")));
		
		NVBPhysValue xspan, yspan;

		if (h.contains("SCAN_RANGE") && !h.value("SCAN_RANGE").isEmpty()) {
			QStringList s_range = h.value("SCAN_RANGE").first().split(' ', QString::SkipEmptyParts);
			xspan = NVBPhysValue(s_range.at(0).toDouble(), NVBDimension("m"));
			yspan = NVBPhysValue(s_range.at(1).toDouble(), NVBDimension("m"));
			}

		if (e.at(dirIndex) == "both") {
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")), NVB::TopoPage, psize, xspan, yspan, comments));
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")), NVB::TopoPage, psize, xspan, yspan, comments));
			}
		else
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")), NVB::TopoPage, psize, xspan, yspan, comments));
		}

	return fi;
	}

NanonisPage::NanonisPage(QFile & file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection): NVB3DPage() {
	pagename = di_data.at(di_headers.indexOf("Name"));

	// size

	if (!header.contains("SCAN_PIXELS") && header.value("SCAN_PIXELS").isEmpty()) {
		NVBOutputError("No scan dimensions available");
		return;
		}

	QStringList s_size = header.value("SCAN_PIXELS").first().split(' ', QString::SkipEmptyParts);
	_resolution = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());

	if (!header.contains("SCAN_RANGE") && header.value("SCAN_RANGE").isEmpty()) {
		NVBOutputError("No scan size available");
		return;
		}

	QStringList s_range = header.value("SCAN_RANGE").first().split(' ', QString::SkipEmptyParts);

	if (!header.contains("SCAN_OFFSET") && header.value("SCAN_OFFSET").isEmpty()) {
		NVBOutputError("No scan offset available");
		return;
		}

	QStringList s_offset = header.value("SCAN_OFFSET").first().split(' ', QString::SkipEmptyParts);

	_position = QRectF(
	              s_offset.at(0).toDouble() - s_range.at(0).toDouble() / 2,
	            - s_offset.at(1).toDouble() - s_range.at(1).toDouble() / 2,
	              s_range.at(0).toDouble(), s_range.at(1).toDouble()
	            );

	if (header.contains("SCAN_ANGLE") && !header.value("SCAN_ANGLE").isEmpty())
		_angle = header.value("SCAN_ANGLE").first().toDouble(); // FIXME check direction and origin
	else
		_angle = 0;

	xd = NVBDimension("m");
	yd = NVBDimension("m");
	zd = NVBDimension(di_data.at(di_headers.indexOf("Unit")));

	if (header.contains("BIAS") && !header.value("BIAS").isEmpty())
		setComment("Bias", NVBPhysValue(header.value("BIAS").first().toDouble(), NVBDimension("V")));

	if (header.contains("COMMENT") && !header.value("COMMENT").isEmpty())
		setComment("User comment", header.value("COMMENT").join(" "));

	if (header.contains("SCAN_TIME") && !header.value("SCAN_TIME").isEmpty())
		setComment("Line time", NVBPhysValue(header.value("SCAN_TIME").first().split(' ', QString::SkipEmptyParts).first().toDouble(),NVBDimension("s")));
	
	int data_points = _resolution.width() * _resolution.height();

	QDataStream ds(&file);
	ds.setVersion(QDataStream::Qt_4_0);
	float buffer;
	double * tdata = (double*) malloc(data_points * 8);
	data = (double*) malloc(data_points * 8);

	for (int i = 0; i < data_points; i++) {
		ds >> buffer;
		tdata[i] = (double)(buffer);
		}

	flipMem<double>(data, tdata, _resolution.width(), _resolution.height(), otherDirection, header.value("SCAN_DIR").first() != "down");

	free(tdata);
	getMinMax();

	// colors

	setColorModel(new NVBGrayRampContColorModel(0, 1, zMin, zMax));
	}
