/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov                                *
 *   Timofey.Balashov@pi.uka.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

#include "nanonis.h"
#include "dimension.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
#include <QDir>
#include "NVBFileInfo.h"
#include "NVBFile.h"
#include <QDebug>
#include <QRectF>

QStringList NanonisFileGenerator::availableInfoFields() const {
		return QStringList() \
						<< "Bias" \
						<< "User comment" \
						;
}

NanonisHeader NanonisFileGenerator::getNanonisHeader( QFile & file )
{
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
      h.insert(key,value);
      if (tmp == ":SCANIT_END:") break;
      value.clear();
      key = tmp.mid(1,tmp.length()-2);
      }
    }

	file.read(4); // 0x0a 0x0a 0x1a 0x04 -- Whatever that means

  return h;
}

NVBFile * NanonisFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("NanonisFileGenerator::loadFile","Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("NanonisFileGenerator::loadFile","No associated files");
		return 0;
		}

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputError("NanonisFileGenerator::loadFile",QString("Couldn't open file %1 : %2").arg(info.first(),file.errorString()));
		return 0;
		}

	if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
		return 0;

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
			f->addSource( new NanonisPage(file, h, headers, e) );
			f->addSource( new NanonisPage(file, h, headers, e, true) );
      }
    else
			f->addSource( new NanonisPage(file, h, headers, e) );
    }

  file.close();

	return f;
}

NVBFileInfo * NanonisFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo","Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo","No associated files");
		return 0;
		}

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo",QString("Couldn't open file %1 : %2").arg(info.first(),file.errorString()));
		return 0;
		}

	if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
		return 0;

	if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
    return 0;
    
	NanonisHeader h = getNanonisHeader(file);
  file.close();

	if (!h.contains("DATA_INFO")) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo","File format error: no DATA_INFO field");
		return 0;
		}

  QStringList l = h.value("DATA_INFO");

	if (l.isEmpty()) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo","File format error: DATA_INFO field empty");
		return 0;
		}

  QStringList headers = l.takeFirst().split('\t'); // Headers
  while (l.last().isEmpty()) l.removeLast();

  int dirIndex = headers.indexOf("Direction");

  // Now we know the number of pages

	NVBFileInfo * fi = new NVBFileInfo(info.first());
	if (!fi) {
		NVBOutputError("NanonisFileGenerator::loadFileInfo","NVBFileInfo allocation failure");
		return 0;
		}

  foreach(QString s, l) {
    QStringList e = s.split('\t');

    QSize psize;

    if (h.contains("SCAN_PIXELS") && !h.value("SCAN_PIXELS").isEmpty()) {
      QStringList s_size = h.value("SCAN_PIXELS").first().split(' ',QString::SkipEmptyParts);
      psize = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());
      }

    QMap<QString,NVBVariant> comments;
    if (h.contains("BIAS") && !h.value("BIAS").isEmpty())
      comments.insert("Bias",NVBPhysValue(h.value("BIAS").first().toDouble(),NVBDimension("V")));
    if (h.contains("COMMENT") && !h.value("COMMENT").isEmpty())
      comments.insert("User comment",h.value("COMMENT").first());

    if (e.at(dirIndex) == "both") {
      fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,comments));
      fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,comments));
      }
    else
      fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,comments));
    }

  return fi;
}

NanonisPage::NanonisPage(QFile & file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection):NVB3DPage()
{
  pagename = di_data.at(di_headers.indexOf("Name"));

  // size

  if (!header.contains("SCAN_PIXELS") && header.value("SCAN_PIXELS").isEmpty())
    throw nvberr_invalid_format;

  QStringList s_size = header.value("SCAN_PIXELS").first().split(' ',QString::SkipEmptyParts);
  _resolution = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());

  if (!header.contains("SCAN_RANGE") && header.value("SCAN_RANGE").isEmpty())
    throw nvberr_invalid_format;

  QStringList s_range = header.value("SCAN_RANGE").first().split(' ',QString::SkipEmptyParts);

  if (!header.contains("SCAN_OFFSET") && header.value("SCAN_OFFSET").isEmpty())
    throw nvberr_invalid_format;

  QStringList s_offset = header.value("SCAN_OFFSET").first().split(' ',QString::SkipEmptyParts);

  _position = QRectF(
                s_offset.at(0).toDouble() - s_range.at(0).toDouble()/2, - s_offset.at(1).toDouble() - s_range.at(1).toDouble()/2,
                s_range.at(0).toDouble(), s_range.at(1).toDouble()
                );

  xd = NVBDimension("m");
  yd = NVBDimension("m");
  zd = NVBDimension( di_data.at(di_headers.indexOf("Unit")) );

  if (header.contains("BIAS") && !header.value("BIAS").isEmpty())
    setComment("Bias",NVBPhysValue(header.value("BIAS").first().toDouble(),NVBDimension("V")));
  if (header.contains("COMMENT") && !header.value("COMMENT").isEmpty())
    setComment("User comment",header.value("COMMENT").first());

  int data_points = _resolution.width() * _resolution.height();

	QDataStream ds(&file);
  float buffer;
  double * tdata = (double*) malloc(data_points*8);
  data = (double*) malloc(data_points*8);

  for (int i = 0; i < data_points; i++ ) {
    ds >> buffer;
    tdata[i] = (double)(buffer);
    }

  flipMem<double>(data,tdata,_resolution.width(),_resolution.height(),otherDirection,header.value("SCAN_DIR").first() != "down");

  getMinMax();

  // colors

  setColorModel(new NVBGrayRampContColorModel(0,1,zMin,zMax));
}

Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
