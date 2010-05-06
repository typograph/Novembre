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

#include "createc.h"
#include "dimension.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
#include <QDir>
#include "NVBFileInfo.h"
#include <QDebug>
#include <zlib.h>

QStringList CreatecFileGenerator::availableInfoFields() const {
    return QStringList() \
            << "Bias" \
            << "Setpoint" \
            ;
}

NVBAssociatedFilesInfo CreatecFileGenerator::associatedFiles(QString filename) const {
    static QRegExp multispec = QRegExp("(A[0-9]*\\.[0-9]*)\\.[XMR][0-9].*\\.vert",Qt::CaseInsensitive,QRegExp::RegExp);
    if (!multispec.exactMatch(filename))
        return NVBFileGenerator::associatedFiles(filename);
    else
        return NVBAssociatedFilesInfo( \
                   multispec.cap(1)+".vert", \
                   QDir(filename,QString("%1*[vV][eE][rR][tT]").arg(multispec.cap(1)),QDir::Name,QDir::Files).entryList(), \
                   this);
}

CreatecHeader CreatecFileGenerator::getCreatecHeader( QFile & file )
{
  CreatecHeader h;

  QByteArray b;
  QString pname1,value;
  QRegExp dimmatch("^(.*)\\[(.*)\\]$");
  NVBVariant tval;
  while (true) {
  
    b = file.readLine(100);
    if ( b[0] == '\0' ) break;
    b.chop(2); // 0x0D 0x0A
    
    QString s = QString(b);
    int i = s.indexOf('=');
    if (i<=0) continue;
    
    QString sleft = s.left(i);
    QString dimstr;
    if (dimmatch.indexIn(sleft) == 0) {
      sleft = dimmatch.cap(1);
      dimstr = dimmatch.cap(2);
      }
    
    value = s.right(s.size()-i-1);
    bool ok;
    int ival = value.toInt(&ok);
    if (ok) {
      if (dimstr.isEmpty())
        tval = ival;
      else
        tval = NVBPhysValue(ival,NVBDimension(dimstr));
      }
    else {
      double dval = value.toDouble(&ok);
      if (ok) {
        if (dimstr.isEmpty())
          tval = dval;
        else
          tval = NVBPhysValue(dval,NVBDimension(dimstr));
          } 
      else {
        if (!dimstr.isEmpty()) NVBOutputError("CreatecFileGenerator::getCreatecHeader","Dimensionised string in "+s);
        else
          tval = value;
        }
      }
         
    pname1 = sleft.section(" / ",0,0);
    h.insert(pname1,tval);
//     pname2 = s.left(i).section(" / ",0,0);
    }
    
  return h;
}

/*
bool CreatecFileGenerator::canLoadFile( QString filename )
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  if (!QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive))
    return false;
  return true;
}
*/

NVBFileStruct * CreatecFileGenerator::loadFile(QFile & file) const
{
	if (!QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive))
		return 0;

	QString filename = file.fileName();
	NVBFileStruct * s = new NVBFileStruct(filename);
	if (!s) throw nvberr_not_enough_memory;

	QString ext = filename.right(filename.size()-filename.lastIndexOf('.')-1).toLower();
	if (ext == "dat")
		s->pages.append( CreatecDatPage::loadAllChannels(file) );
	else if (ext == "lat")
		s->pages.append( new CreatecLatPage(file) );
	else if (ext == "vert")
		s->pages.append( CreatecVertPage::loadAllChannels(file) );
	else if (ext == "tspec")
		s->pages.append( new CreatecTSpecPage(file) );
	else {
		delete s;
		return 0;
		}

	return s;
}

NVBFileInfo * CreatecFileGenerator::loadFileInfo( QFile & file ) const
{
	if (!QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive))
		return 0;

	CreatecHeader header = getCreatecHeader(file);
	file.close();
	
	NVBFileInfo * fi = new NVBFileInfo(file);
	if (!fi) throw nvberr_not_enough_memory;
	
	//  QString name;
	NVB::PageType type;
	QString filename = file.fileName();
	QString ext = filename.right(filename.size()-filename.lastIndexOf('.')-1).toLower();
	if (ext == "dat")
		type = NVB::TopoPage;
	else if (ext == "lat")
		type = NVB::SpecPage;
	else if (ext == "vert")
		type = NVB::SpecPage;
	else if (ext == "tspec")
		type = NVB::SpecPage;
	else {
		type = NVB::InvalidPage;
		}
		
	//  QSize size;
	QMap<QString,NVBVariant> comments;
	
	comments.insert("Bias",header.value("BiasVoltage").toPhysValue());
	comments.insert("Setpoint",header.value("Current").toPhysValue());
	
	int nchannels = header.value("Channels").toInt();
	for (int i = 0; i < nchannels; i++)
		fi->pages.append(NVBPageInfo(header.value("BiasVoltage",QString()).toString(),type,QSize(header.value("Num.X",0).toInt(),header.value("Num.Y",0).toInt()),comments));
	
	return fi;
}

QStringList CreatecDatPage::channelNames = QStringList( QStringList("Topography") + QStringList("Current") + QStringList("ADC1") + QStringList("ADC2") );

QList<NVBDataSource*> CreatecDatPage::loadAllChannels(QFile & file) {

  QList<NVBDataSource*> result;

  CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);

    // initialise data

  int nchannels = file_header.value("Channels").toInt();

  int data_points = file_header.value("Num.X",0).toInt() * file_header.value("Num.Y",0).toInt();

  if (data_points == 0) return result;

  file.seek(0);

  QString format(file.readLine(20));
  format.chop(2);

  if (format == "[Parameter]") {
    NVBOutputError("CreatecDatPage::loadAllChannels","No idea how to deal with [Parameter]");
    return result;
    }
  else if (format == "[Paramet32]") {
    // this is uncompressed
    file.seek(0x4004);

    quint32 * idata = (quint32*)malloc(data_points*4);

    scaler<quint32,double> intscaler;

    for (int i = 0; i < nchannels; i++) {
      file.read((char*)idata,data_points*4);
      double * data = (double*) malloc(data_points*8);
      scaleMem<quint32,double>(data,intscaler,idata,data_points);
      result << new CreatecDatPage(file_header,i,data);
    }

    free(idata);
    }
  else if (format == "[Paramco32]") {
    // this is compressed
    qint64 zsize = file.size() - 0x4000;
    uLongf unzsize = data_points * 800; // * nchannels

    qint8 *zbuf = (qint8*)malloc(zsize);
    float *buf  = (float*)malloc(unzsize);

    // start of gzdata
    file.seek(0x4000);
    file.read((char*) zbuf, zsize);
    int errorcode;
    if ((errorcode = uncompress ((Bytef *) buf, &unzsize, (Bytef *) zbuf, zsize)) != Z_OK) {
      NVBOutputError("CreatecDatPage::loadAllChannels","Uncompressing failed with error %d. Buffer size %d",errorcode, unzsize);
      }
    else
      NVBOutputPMsg("CreatecDatPage::loadAllChannels","Uncompressed buffer to %d bytes", unzsize);

    free(zbuf);

    scaler<float,double> intscaler;
    for (int i = 0; i < nchannels; i++) {
     double * data = (double*) malloc(data_points*8);
      scaleMem<float,double>(data,intscaler,buf+1+data_points*i,data_points);
      result << new CreatecDatPage(file_header,i,data);
    }

    free(buf);
    }
  else {
    NVBOutputError("CreatecDatPage::loadAllChannels","No idea how to deal with " + format);
    }

  return result;
}

CreatecDatPage::CreatecDatPage( CreatecHeader file_header, int channel, double * bulk_data):header(file_header)
{

  int magic = header.value("Chan(1,2,4)").toInt();

  pagename = channelNames.at(channel%magic) + ((channel/magic == 0) ? " (forward)" : " (backward)");

  // size
  
  _resolution = QSize(header.value("Num.X",0).toInt(), header.value("Num.Y",0).toInt());
  _position = QRectF( // FIXME I saw Scanoffx somewhere
                -header.value("Scanrotoffx",0).toInt()*header.value("Dacto[A]xy",0).toDouble()*0.1,
                -header.value("Scanrotoffy",0).toInt()*header.value("Dacto[A]xy",0).toDouble()*0.1,
                header.value("Length x",0).toPhysValue().getValue()*0.1,
                header.value("Length y",0).toPhysValue().getValue()*0.1
                );
  _position.translate(-_position.width()/2,0);

  int data_points = _resolution.width()*_resolution.height();

  xd = NVBDimension("nm");
  yd = NVBDimension("nm");
  switch (channel%magic) {
    case 0: { // Topography. Scale by Dacto[A]z, taking into account that they are nanometers.
      zd = NVBDimension("nm");
      scaler<double,double> DACscaler(0,file_header.value("Dacto[A]z").toDouble());
      data = (double*) malloc(data_points*8);
      scaleMem<double,double>(data,DACscaler,bulk_data,data_points);
      free(bulk_data);
      break;
    }
    case 1: { // Current. Scale by piezo constant, Dacto[A]z, taking into account that they are Angstroems now.
      zd = NVBDimension("A");
      scaler<double,double> DACscaler( 0,
                                       file_header.value("Dacto[A]z").toDouble()*
                                       pow(10,-file_header.value("Gainpreamp").toInt())/
                                       file_header.value("ZPiezoconst").toDouble()
                                       );
      data = (double*) malloc(data_points*8);
      scaleMem<double,double>(data,DACscaler,bulk_data,data_points);
      free(bulk_data);
      break;
    }
    default: {// ADC1 & ADC2 -> STM dependent, relay on NVBScript.
      zd = NVBDimension("DAC",false);
      data = bulk_data;
      break;
    }
  }
      
  getMinMax();

  // colors
  
  setColorModel(new NVBGrayRampContColorModel(0,1,zMin,zMax));
}

CreatecVertPage::CreatecVertPage( CreatecHeader file_header, int channel, double * bulk_data ):header(file_header)
{
}

QList<NVBDataSource*> CreatecVertPage::loadAllChannels(QFile & file) {

  QList<NVBDataSource*> result;

  CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);

    // initialise data

  int nchannels = file_header.value("Channels").toInt();

  int data_points = file_header.value("Num.X",0).toInt() * file_header.value("Num.Y",0).toInt();

  if (data_points == 0) return result;

  file.seek(0);

  QString format(file.readLine(20));
  format.chop(2);

  if (format != "[Parameter]") {
    NVBOutputError("CreatecVertPage::loadAllChannels",QString("Don�t know how to deal with format %1").arg(format));
    return result;
  }

  file.seek(0x400C);
  while (!file.atEnd()) {
   QList<QByteArray> pt_data = file.readLine(800).split('\t');


  }

  return result;
}

CreatecLatPage::CreatecLatPage( QFile & file ):header(CreatecFileGenerator::getCreatecHeader(file))
{
}

CreatecTSpecPage::CreatecTSpecPage( QFile & file ):header(CreatecFileGenerator::getCreatecHeader(file))
{
}



Q_EXPORT_PLUGIN2(createc, CreatecFileGenerator)
