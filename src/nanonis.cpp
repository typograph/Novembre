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

class NVBExpandableSpecPage : public NVBSpecPage {
public:
  NVBExpandableSpecPage(QString name, QString taxis ):NVBSpecPage() {
    _datasize = QSize(0,0);
    pagename = name.left(name.lastIndexOf(' '));
    zd = NVBDimension(name.mid(name.lastIndexOf('(')+1,name.lastIndexOf(')')-name.lastIndexOf('(')-1));
    xd = NVBDimension("m");
    yd = NVBDimension("m");
    td = NVBDimension(taxis.mid(taxis.lastIndexOf('(')+1,taxis.lastIndexOf(')')-taxis.lastIndexOf('(')-1));
    setComment("X axis label",taxis.left(taxis.lastIndexOf(' ')));
    setComment("Y axis label",pagename);
    setColorModel(new NVBRandomDiscrColorModel());
    }

  void addNewSpecPoint(double x, double y, QwtData * pdata) {
		if (! _datasize.isNull() && (int)pdata->size() != _datasize.width()) {
      NVBOutputError(QString("Wrong size of added data: %1 when main size is %2").arg(pdata->size()).arg(_datasize.width()) );
      return;
      };
    emit dataAboutToBeChanged();
    _positions << QPointF(x,-y);
    _data << pdata;
    _datasize.rheight() += 1;
    _datasize.rwidth() = pdata->size();
    getMinMax();
    emit dataChanged();
    };
};

QStringList NanonisFileGenerator::availableInfoFields() const {
		return QStringList() \
						<< "Bias" \
						<< "User comment" \
						;
}

NVBAssociatedFilesInfo NanonisFileGenerator::associatedFiles(QString filename) const {
//		static QRegExp multispec = QRegExp("[/\\\\]([^/\\\\]*[^/\\\\0-9])\\d+\\.dat$",Qt::CaseInsensitive,QRegExp::RegExp);
//		if (!filename.contains(multispec))
				return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), QStringList(filename), this);
//				return NVBFileGenerator::associatedFiles(filename);
/*
else {
			QString path = QFileInfo(filename).absolutePath();
			// Workaround for files with spaces in names
			QDir dir  = QDir(path,QString(),QDir::Name,QDir::Files);
			dir.setNameFilters(QStringList(QString("%1*.dat").arg(multispec.cap(1))));
			QStringList files = dir.entryList();
			for(QStringList::iterator it = files.begin(); it != files.end(); it++)
				*it = path + "/" + *it;
			if (!files.count()) return NVBAssociatedFilesInfo();
			return NVBAssociatedFilesInfo( multispec.cap(1)+".dat", files, this);
			}
*/
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
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	if (info.count() > 1 || info.first().right(3) == "dat")
		return loadSpecAggregation(info);

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
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
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	if (info.count() > 1 || info.first().right(3) == "dat")
		return loadSpecAggregationInfo(info);

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
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
      QStringList s_size = h.value("SCAN_PIXELS").first().split(' ',QString::SkipEmptyParts);
      psize = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());
      }

    QMap<QString,NVBVariant> comments;
    if (h.contains("BIAS") && !h.value("BIAS").isEmpty())
      comments.insert("Bias",NVBPhysValue(h.value("BIAS").first().toDouble(),NVBDimension("V")));
    if (h.contains("COMMENT") && !h.value("COMMENT").isEmpty())
      comments.insert("User comment",h.value("COMMENT").first());

		NVBPhysValue xspan, yspan;
		if (h.contains("SCAN_RANGE") && !h.value("SCAN_RANGE").isEmpty()) {
			QStringList s_range = h.value("SCAN_RANGE").first().split(' ',QString::SkipEmptyParts);
			xspan = NVBPhysValue(s_range.at(0).toDouble(),NVBDimension("m"));
			yspan = NVBPhysValue(s_range.at(1).toDouble(),NVBDimension("m"));
			}

    if (e.at(dirIndex) == "both") {
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,xspan,yspan,comments));
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,xspan,yspan,comments));
      }
    else
			fi->pages.append(NVBPageInfo(e.at(headers.indexOf("Name")),NVB::TopoPage,psize,xspan,yspan,comments));
    }

  return fi;
}

NanonisPage::NanonisPage(QFile & file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection):NVB3DPage()
{
  pagename = di_data.at(di_headers.indexOf("Name"));

  // size

	if (!header.contains("SCAN_PIXELS") && header.value("SCAN_PIXELS").isEmpty()) {
		NVBOutputError("No scan dimensions available");
		return;
		}

  QStringList s_size = header.value("SCAN_PIXELS").first().split(' ',QString::SkipEmptyParts);
  _resolution = QSize(s_size.at(0).toInt(), s_size.at(1).toInt());

	if (!header.contains("SCAN_RANGE") && header.value("SCAN_RANGE").isEmpty()) {
		NVBOutputError("No scan size available");
		return;
		}

  QStringList s_range = header.value("SCAN_RANGE").first().split(' ',QString::SkipEmptyParts);

	if (!header.contains("SCAN_OFFSET") && header.value("SCAN_OFFSET").isEmpty()) {
		NVBOutputError("No scan offset available");
		return;
		}

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
  ds.setVersion(QDataStream::Qt_4_0);
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

NVBFile * NanonisFileGenerator::loadSpecAggregation(const NVBAssociatedFilesInfo & info) const
{
  QList<NVBExpandableSpecPage*> pages;

  QFile f(info.first());
  if (! f.open(QIODevice::ReadOnly)) {
    NVBOutputFileError(&f);
    return 0;
    }
  QTextStream first(&f);
	while(first.readLine() != "[DATA]")
		if (first.atEnd()) { 
			NVBOutputError("File format error: no DATA section");
			return 0;
			}
  QStringList names = first.readLine().split('\t',QString::SkipEmptyParts);
  for(int i = 1; i < names.count(); i++)
    pages << new NVBExpandableSpecPage(names.at(i),names.first());

  foreach(QString filename, info) {
    QFile f(filename);
    if (! f.open(QIODevice::ReadOnly)) {
      NVBOutputFileError(&f);
      }
    else {
      QTextStream specdata(&f);
      specdata.readLine(); // Experiment \t ....
      specdata.readLine(); // Date ...
      specdata.readLine(); // User ...
      QString s = specdata.readLine();
      double xpos = s.mid(s.indexOf('\t')).toDouble();
      s = specdata.readLine();
      double ypos = s.mid(s.indexOf('\t')).toDouble();
      s = specdata.readLine();
//      double zpos = s.mid(s.indexOf('\t')).toDouble();
      while(specdata.readLine() != "[DATA]")
			if (specdata.atEnd()) { 
				NVBOutputError("File format error: no DATA section");
				return 0;
				}
			QString clmns = specdata.readLine(); // Column names
      QList<QVector<double> > cdata;
  
      for(int i = -1; i < pages.count(); i++)
        cdata << QVector<double>();
  
      while(!specdata.atEnd()) {
        QStringList vdata = specdata.readLine().split('\t',QString::SkipEmptyParts);
				if (vdata.count() > cdata.count()) {
					NVBOutputError(QString("Unexpected columns.\nExpected : %1\nGot : %2").arg(names.join(" ")).arg(clmns));
					for (int j = 0; j < cdata.size(); j++)
						cdata[j] << vdata.at(j).toDouble();
					}
				else if (vdata.count() < cdata.count()) {
					NVBOutputError(QString("Missing columns.\nExpected : %1\nGot : %2").arg(names.join(" ")).arg(clmns));
					for (int j = 0; j < vdata.size(); j++)
						cdata[j] << vdata.at(j).toDouble();
					for (int j = vdata.size(); j < cdata.size(); j++)
						cdata[j] << 0;
					}
				else
					for (int j = 0; j < vdata.size(); j++)
						cdata[j] << vdata.at(j).toDouble();
        }
  
      for(int i = 0; i < pages.count(); i++)
        pages[i]->addNewSpecPoint(xpos,ypos,new QwtArrayData(cdata.at(0),cdata.at(i+1)));
  
      // TODO Do something with z;
      }
    }

    NVBFile * result = new NVBFile(info);

    foreach(NVBExpandableSpecPage * p, pages)
      result->addSource(p);

    return result;
}

NVBFileInfo * NanonisFileGenerator::loadSpecAggregationInfo(const NVBAssociatedFilesInfo & info) const
{
  NVBFileInfo * fi = new NVBFileInfo(info);
  if (!fi) {
		NVBOutputError("NVBFileInfo creation failed");
		return 0;
		}

  QFile f(info.first());
  if ( !(f.open(QIODevice::ReadOnly))) {
    NVBOutputFileError(&f);
    return 0;
    }

  QTextStream first(&f);
	while(first.readLine() != "[DATA]")
		if (first.atEnd()) { 
			NVBOutputError("File format error: no DATA section");
			return 0;
			}
  QStringList names = first.readLine().split('\t',QString::SkipEmptyParts);
  int nxs = 0;
  while (!first.atEnd()) {
    first.readLine();
    nxs += 1;
    }
  for(int i = 1; i < names.count(); i++)
		fi->pages.append(NVBPageInfo(names.at(i),NVB::SpecPage,QSize(nxs,info.count()),NVBPhysValue(nxs,NVBDimension()),NVBPhysValue(),QMap<QString,NVBVariant>()));

  f.close();

  return fi;

}

Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
