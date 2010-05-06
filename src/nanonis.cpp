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
#include <QDebug>
#include <QRectF>

NanonisHeader NanonisFileGenerator::getNanonisHeader( QFile * const file )
{
  NanonisHeader h;

  QString key;
  QString tmp;
  QStringList value;

  while (true) {
    tmp = QString(file->readLine(100)).trimmed();
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

  file->read(4); // 0x0a 0x0a 0x1a 0x04 -- Whatever that means

  return h;
}

bool NanonisFileGenerator::canLoadFile( QString filename )
{
#ifdef WITH_SPEC_AGGR
  if (filename.right(4).toLower() != ".sxm" && filename.right(4).toLower() != ".nns") return false;
#else
  if (filename.right(4).toLower() != ".sxm") return false;
#endif

/*  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
    return false;*/
  return true;
}

NVBFileStruct * NanonisFileGenerator::loadFile( QString filename )
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return 0;
#ifdef WITH_SPEC_AGGR
  if (filename.right(4).toLower() == ".nns")
    return loadSpecAggregation(file);
  else {
#endif
  if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
    return 0;
    
  NVBFileStruct * fs = new NVBFileStruct(filename);
  if (!fs) throw nvberr_not_enough_memory;
  
  NanonisHeader h = getNanonisHeader(&file);

  QStringList l = h.value("DATA_INFO");
  QStringList headers = l.takeFirst().split('\t'); // Headers
  while (l.last().isEmpty()) l.removeLast();

  int dirIndex = headers.indexOf("Direction");

  // Now we know the number of pages
  foreach(QString s, l) {
    QStringList e = s.split('\t');
    if (e.at(dirIndex) == "both") {
      fs->pages.append( new NanonisPage(&file, h, headers, e) );
      fs->pages.append( new NanonisPage(&file, h, headers, e, true) );
      }
    else
      fs->pages.append( new NanonisPage(&file, h, headers, e) );
    }

  file.close();

  return fs;
#ifdef WITH_SPEC_AGGR
  }
#endif
}

NVBFileInfo * NanonisFileGenerator::loadFileInfo( QString filename )
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return 0;
#ifdef WITH_SPEC_AGGR
  if (filename.right(4).toLower() == ".nns")
    return loadSpecAggregationInfo(file);
  else {
#endif
  if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive))
    return 0;
    
  NVBFileInfo * fi = new NVBFileInfo(filename);
  if (!fi) throw nvberr_not_enough_memory;
  
  NanonisHeader h = getNanonisHeader(&file);
  file.close();

  if (!h.contains("DATA_INFO"))
    throw nvberr_invalid_format;

  QStringList l = h.value("DATA_INFO");

  if (l.isEmpty())
    throw nvberr_invalid_format;

  QStringList headers = l.takeFirst().split('\t'); // Headers
  while (l.last().isEmpty()) l.removeLast();

  int dirIndex = headers.indexOf("Direction");

  // Now we know the number of pages
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
#ifdef WITH_SPEC_AGGR
  }
#endif
}

NanonisPage::NanonisPage(QFile * const file, const NanonisHeader & header, const QStringList & di_headers, const QStringList & di_data, bool otherDirection):NVB3DPage()
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

  QDataStream ds(file);
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

#ifdef WITH_SPEC_AGGR

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
    if (! _datasize.isNull() && pdata->size() != _datasize.width()) {
      NVBOutputError("NVBExpandableSpecPage","Wrong size of added data: %d when main size is %d", pdata->size(), _datasize.width() );
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

QStringList filesFromNNS(QFile & file) {
  QStringList format = QString(file.readLine()).split('\t');
  if (format.count() < 2 || format.first() != "Format") {
    NVBOutputError("NanonisFileGenerator::loadSpecAggregation","Incomplete file format");
    return QStringList();
    }

  QStringList spec_files;

  QString path = QFileInfo(file).canonicalPath() + '/';

  if (format.at(1) == "List") {
    while(!file.atEnd())
      spec_files << path + QString(file.readLine()).trimmed();
    }
  else if (format.at(1) == "Range") {
    if (format.count() < 4) {
      NVBOutputError("NanonisFileGenerator::loadSpecAggregation","Not enough arguments to range format");
      return QStringList();
      }
    QByteArray mask = file.readLine();
    bool ok;
    int start = format.at(2).toInt(&ok);
    if (!ok) {
      NVBOutputError("NanonisFileGenerator::loadSpecAggregation","Undeciferable start of range");
      return QStringList();
      }
    int end = format.at(3).toInt(&ok)+1;
    if (!ok) {
      NVBOutputError("NanonisFileGenerator::loadSpecAggregation","Undeciferable end of range");
      return QStringList();
      }
    QString fname;
    for (int i = qMin(start,end); i < qMax(start,end); i++) {
      spec_files << path + fname.sprintf(mask,i).trimmed();
      }
    }
  else {
    NVBOutputError("NanonisFileGenerator::loadSpecAggregation","Unknown file format");
    return QStringList();
    }

  return spec_files;
}

NVBFileStruct * NanonisFileGenerator::loadSpecAggregation(QFile & file)
{
  QStringList spec_files = filesFromNNS(file);

  if (spec_files.isEmpty())
    return 0;

  QList<NVBExpandableSpecPage*> pages;

  QFile f(spec_files.first());
  if (! f.open(QIODevice::ReadOnly)) {
    NVBOutputFileError("NanonisFileGenerator::loadSpecAggregation",f.fileName());
    return 0;
    }
  QTextStream first(&f);
  while(first.readLine() != "[DATA]") {;}
  QStringList names = first.readLine().split('\t',QString::SkipEmptyParts);
  for(int i = 1; i < names.count(); i++)
    pages << new NVBExpandableSpecPage(names.at(i),names.first());

  foreach(QString filename, spec_files) {
    QFile f(filename);
    if (! f.open(QIODevice::ReadOnly)) {
      NVBOutputFileError("NanonisFileGenerator::loadSpecAggregation",f.fileName());
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
      double zpos = s.mid(s.indexOf('\t')).toDouble();
      while(specdata.readLine() != "[DATA]") {;}
      specdata.readLine(); // Column names
      QList<QVector<double> > cdata;
  
      for(int i = -1; i < pages.count(); i++)
        cdata << QVector<double>();
  
      while(!specdata.atEnd()) {
        QStringList vdata = specdata.readLine().split('\t',QString::SkipEmptyParts);
        for (int j = 0; j < vdata.size(); j++)
          cdata[j] << vdata.at(j).toDouble();
        }
  
      for(int i = 0; i < pages.count(); i++)
        pages[i]->addNewSpecPoint(xpos,ypos,new QwtArrayData(cdata.at(0),cdata.at(i+1)));
  
      // TODO Do something with z;
      }
    }

    NVBFileStruct * fs = new NVBFileStruct(file.fileName());

    foreach(NVBExpandableSpecPage * p, pages)
      fs->pages << p;

    return fs;
}

NVBFileInfo * NanonisFileGenerator::loadSpecAggregationInfo(QFile & file)
{
  QStringList spec_files = filesFromNNS(file);

  if (spec_files.isEmpty())
    return 0;

  NVBFileInfo * fi = new NVBFileInfo(file.fileName());
  if (!fi) throw nvberr_not_enough_memory;

  QFile f(spec_files.first());
  if (!(f.exists())) {
    NVBOutputError("NanonisFileGenerator::loadSpecAggregationInfo",QString("File %1 doesn't exist").arg(f.fileName()));
    return 0;
    }
  if ( !(f.open(QIODevice::ReadOnly))) {
    NVBOutputFileError("NanonisFileGenerator::loadSpecAggregationInfo",f.fileName());
    return 0;
    }
  QTextStream first(&f);
  while(first.readLine() != "[DATA]") {;}
  QStringList names = first.readLine().split('\t',QString::SkipEmptyParts);
  int nxs = 0;
  while (!first.atEnd()) {
    first.readLine();
    nxs += 1;
    }
  for(int i = 1; i < names.count(); i++)
    fi->pages.append(NVBPageInfo(names.at(i),NVB::SpecPage,QSize(nxs,spec_files.count()),QMap<QString,NVBVariant>()));

  f.close();

  return fi;

}
#endif

Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
