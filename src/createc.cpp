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

#include "NVBLogger.h"
#include "createc.h"
#include "NVBDimension.h"
#include "NVBColorMaps.h"
#include <QtCore/QDir>
#include "NVBFileInfo.h"
#include <zlib.h>

QStringList CreatecFileGenerator::availableInfoFields() const {
    return QStringList() \
            << "Bias" \
            << "Setpoint" \
						<< "Z piezo constant" \ // ZPiezoconst=  7.33
						<< "X piezo constant" \ // Xpiezoconst= 30.92
						<< "Y piezo constant" \ // YPiezoconst= 30.92
// DAC-Type=20bit
						<< "STM preset" // Titel / Titel=LTSTM_5K_XYZmain
// Delta X / Delta X [Dac]=256
// Delta Y / Delta Y [Dac]=256
// Num.X / Num.X=256
// Num.Y / Num.Y=256
// Delay X+ / Delay X+=1
// Delay X- / Delay X-=1
// Delay Y / Delay Y=1
// D-DeltaX / D-DeltaX=4
// GainX / GainX=10
// GainY / GainY=10
// GainZ / GainZ=10
// Rotation / Rotation=0
// BiasVoltage / BiasVolt.[mV]=-200
// Gainpreamp / GainPre 10^=9
// Chan(1,2,4) / Chan(1,2,4)=1
// PlanDx / PlanDx=-0.0040
// PlanDy / PlanDy=-0.1050
// Scanrotoffx / OffsetX=-408932.2
// Scanrotoffy / OffsetY=435145.8
// MVolt_1 / MVolt_1=0
// MVolt_2 / MVolt_2=0
// MVolt_3 / MVolt_3=0
// MVolt_4 / MVolt_4=0
// MVolt_5 / MVolt_5=0
// MVolt_6 / MVolt_6=0
// RepeatRotinc / RepeatRotinc=0
// RptBVoltinc / RptBVoltinc=0
// Repeatinterval / Repeatinterval=0
// Repeatcounter / Repeatcounter=0
// RepeatXoffset / RepeatXoffset=0
// RepeatYoffset / RepeatYoffset=0
// Scantype / Scantype=1
// Scanmode / Scanmode=2
// Scancoarse / Scancoarse=0
// CHMode / CHMode=0
// Channels / Channels=2
// Preamptype / Preamptype=1
// VFBMode / VFBMode=0
// RotCMode / RotCMode=1
// CHModeZoff / CHModeZoff=0
// CHModeGainpreamp / CHModeGainpreamp=8
// Latmanmode=1
// LatmResist=2.000E+05
// LatmanVolt=-50
// Latmanlgi=2500
// Latmangain=7
// Latmanddx=256
// Latmandelay=1537
// Latm0Delay=2
// Latmanccdz=0
// Latmanextension=1.000
// TipForm_Volt=-3000
// TipForm_Z=300
// Tip_Delay=49
// Tip_Latddx=256
// Tip_LatDelay=192
// Tip_Gain=6
// Vertmangain=9
// Vertmandelay=366
// Vpoint0.t=0
// Vpoint1.t=4096
// Vpoint2.t=0
// Vpoint3.t=0
// Vpoint0.V=-200
// Vpoint1.V=1000
// Vpoint2.V=0
// Vpoint3.V=0
// Zpoint0.t=0
// Zpoint1.t=0
// Zpoint2.t=0
// Zpoint3.t=0
// Zpoint0.z=0
// Zpoint1.z=0
// Zpoint2.z=0
// Zpoint3.z=0
// Zoffset=0
// Zdrift=20
// VertSpecBack=1
// VertSpecAvrgnr=1
// VertRepeatCounter=1
// VertLineCount=2
// SpecXGrid=256
// SpecYGrid=100000
// SpecXYGridDelay=10
// SpecGridChan=2
// VerttreshImax=20000
// VerttreshImin=-20000
// VertAvrgdelay=100
// VertFBMode=4
// VertFBLogiset= 500.000
// Imageframe=0
// Imagegrayfactor=16.979
// Zoom=2.00
// OrgPlanX=0.12115
// OrgPlanY=0.10018
// OrgPlanOff=-463925.31250
// Dacto[A]xy=0.00059
// Dacto[A]z=0.00014
// Planoff=0
// Planx=0
// Plany=0
// Plany2=0
// Planavrgnr=17
// Planoffset2=0
// Plano2start=0
// Imagebackoffset=0
// Frameimageoffset=1000
// DigZoomX=1.00000
// DigZoomZ=1.00000
// FBLogIset=  30.000
// FBIntegral=-27.900000
// FBProp=  0.000000
// FBVoltRC=  0.000100
// FBVoltGain=10000.000
// CurrentRC=0.9000
// FBLog=1
// Imaxcurrent=1000000.0
// Imaxdelay=      1
// ImaxZret=  0.000
// SpecAvrgnr=32
// SpecFreq=5000.000
// FFTPoints=4096
// LockinFreq=1000
// LockinAmpl=20
// LockinPhase=-11
// LockinPhase2=71
// LockinRC= 0.00063
// LockinMode=0
// SBC_Clk[MHz]=150
// DSP_Clock=50000
// SRS_Frequency=1000.000
// SRS_ModVoltage=  1.0000
// SRS_InpGain[V]=1.0E+00
// SRS_InpTimeC[s]=1.0E+00
// LockinRefAmpl=0
// LockinRefPhase=0
// DAC5[V]=0.00000
// UserPreampCode=6:011000/8:101000/10:110000/12:111000/
// HPIB_Address=hpib7,9
// Upinc=60493
// Upcount=13
// Rotinc=60493
// Rotincquad=0
// Rotcount=13
// XYBurst=500
// RotBurst=2000
//  =10
// AproBurst=5
// AproPeriod=300
//  =0
// X-Puls-Count=694799
// Y-Puls-Count=-24143
// Z-Puls-Count=-3830973
// Autolevel=0
// Scandvinc[DAC]=0
// =
// Z-Res. [A]: +/- =3.039
// Length x[A]=386.5118
// Length y[A]=386.5118
// Biasvolt[mV]=-200
// Current[A]=3.0E-11
// Sec/line:=0.328
// Sec/Image:=167.772
// ActGainXYZ=10 10 10
// Channels=2
// T-STM:=0.00000
// =
// HP_Ch1=0.00000
// HP_Ch2=0.00000
// HP_Ch3=0.00000
// HP_Ch4=0.00000
// =// =
// T_ADC2[K]=396.242
// T_ADC3[K]=  0.000
	;
}

NVBAssociatedFilesInfo CreatecFileGenerator::associatedFiles(QString filename) const {
		static QRegExp multispec = QRegExp("[/\\\\](A[0-9]*\\.[0-9]*)\\.[XMLR][0-9].*\\.VERT$",Qt::CaseInsensitive,QRegExp::RegExp);
		if (!filename.contains(multispec))
//		if (multispec.indexIn(filename) == -1)
				return NVBFileGenerator::associatedFiles(filename);
		else {
			QString path = QFileInfo(filename).absolutePath();
			QStringList files = QDir(path,QString("%1*VERT").arg(multispec.cap(1)),QDir::Name,QDir::Files).entryList();
			for(QStringList::iterator it = files.begin(); it != files.end(); it++)
				*it = path + "/" + *it;
			return NVBAssociatedFilesInfo( multispec.cap(1)+".VERT", files, this);
			}
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
				if (!dimstr.isEmpty()) NVBOutputError("Dimensionised string in "+s);
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

NVBFile * CreatecFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	try {
		NVBFile * f = new NVBFile(info);
		}
	catch (...) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	// OK, here we have a couple of possibilities
	//     - we might have a (dat) file [just one]
	//     - we might have a bunch of (vert) files
	//     - we might have a (tspec) or a (lat) file [that we don't know how to read]

// TODO Here to fill in fileInfo.

	QString ffname = info.first();
	QString ext = ffname.right(ffname.size()-ffname.lastIndexOf('.')-1).toLower();
	if (ext == "dat") {
		f->addSource( loadAllChannelsFromDAT(ffname) );
		if (info.count() > 1)
			NVBOutputPMsg("Associated files include more that one *.dat file");
		}
	else if (ext == "lat")
		f->addSource( loadAllChannelsFromLAT(ffname) );
	else if (ext == "vert")
		f->addSource( loadAllChannelsFromVERT(info) );
	else if (ext == "tspec")
		f->addSource( loadAllChannelsFromTSPEC(info) );
	else {
		NVBOutputError(QString("Didn't recognise file format of %1").arg(ffname));
		}

	if (f->fileInfo.dataCount() == 0) {
		delete f;
		return 0;
		}

	return f;
}

NVBFileInfo * CreatecFileGenerator::loadFileInfo( const NVBAssociatedFilesInfo & info ) const throw()
{
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	if (!QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive))
		return 0;

	CreatecHeader header = getCreatecHeader(file);
	file.close();
	
	NVBFileInfo * fi = new NVBFileInfo(info);
	if (!fi) return 0;
	
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

QStringList CreatecFileGenerator::DATchannelNames = QStringList( QStringList("Topography") + QStringList("Current") + QStringList("ADC1") + QStringList("ADC2") );

NVBDataSource* CreatecFileGenerator::loadAllChannelsFromDAT(QString filename) {

	NVBDataSource* result = 0;
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return result;
		}
	if ( !QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive) ) {
		NVBOutputError(QString("Unknown file format in %1").arg(filename));
		return result;
		}

  CreatecHeader file_header = getCreatecHeader(file);

    // initialise data

  int nchannels = file_header.value("Channels").toInt();

  int data_points = file_header.value("Num.X",0).toInt() * file_header.value("Num.Y",0).toInt();

	if (data_points == 0) {
		NVBOutputError(QString("Zero data size in %1").arg(filename));
		return result;
		}

// There are always only 2 axes : X & Y

	result = new NVBDataSource();

	result->addAxis("X",file_header.value("Num.X",0).toInt());
	result->addAxisMap(new NVBAxisPhysMap(NVBPhysValue(0,"m"),NVBPhysValue(,"m")),0);
	result->addAxis("Y",file_header.value("Num.Y",0).toInt());

  file.seek(0);

  QString format(file.readLine(20));
  format.chop(2);

  if (format == "[Parameter]") {
		NVBOutputError("No idea how to deal with [Parameter]");
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
			NVBOutputError(QString("Uncompressing failed with error %1. Buffer size %2").arg(errorcode).arg(unzsize));
      }
    else
			NVBOutputPMsg(QString("Uncompressed buffer to %1 bytes").arg(unzsize));

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
		NVBOutputError("No idea how to deal with " + format);
    }

  return result;
}

CreatecDatPage::CreatecDatPage( CreatecHeader file_header, int channel, double * bulk_data):header(file_header)
{

  int magic = header.value("Chan(1,2,4)").toInt();

  pagename = channelNames.at(channel%magic) + ((channel/magic == 0) ? " (forward)" : " (backward)");

  // size
  
	// I don't know why, but not only
	// Dacto[A]xy is in nanometers, but also
	// the coordinates don't correspond to the middle top of the picture, but to the left corner

  _resolution = QSize(header.value("Num.X",0).toInt(), header.value("Num.Y",0).toInt());
	_position = QRectF( // FIXME I saw Scanoffx somewhere
								-header.value("Scanrotoffx",0).toInt()*header.value("Dacto[A]xy",0).toDouble()*1e-9,
	//								-0.5*header.value("Length x",0).toPhysValue().getValue()*1e-10,
								-header.value("Scanrotoffy",0).toInt()*header.value("Dacto[A]xy",0).toDouble()*1e-9,
								header.value("Length x",0).toPhysValue().getValue()*1e-10,
								header.value("Length y",0).toPhysValue().getValue()*1e-10
                );
  _position.translate(-_position.width()/2,0);

  int data_points = _resolution.width()*_resolution.height();

	xd = NVBDimension("m");
	yd = NVBDimension("m");
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
		default: {// ADC1 & ADC2 -> STM dependent, rely on NVBScript.
      zd = NVBDimension("DAC",false);
      data = bulk_data;
      break;
    }
  }
      
  getMinMax();

  // colors
  
  setColorModel(new NVBGrayRampContColorModel(0,1,zMin,zMax));
}

CreatecVertPage::CreatecVertPage():NVBSpecPage()
{
}

CreatecVertPage::CreatecVertPage( CreatecHeader file_header, int channel, double * bulk_data ):NVBSpecPage() // :header(file_header)
{
}

QList<NVBDataSource*> CreatecVertPage::loadAllChannels(QStringList filenames) {

	/*
		So, there are different types of *.vert files

		1) -- A000000.000000.VERT

				. One datapoint - load it

		2) -- A000000.000000.R0000.VERT

				. Measured several times on one datapoint

		3) -- A000000.000000.L0000.[R0000.]VERT

				. It's a line

		4) -- A000000.000000.X0000.Y0000.[R0000.]VERT

				. It's a grid
	*/

	filenames.sort();

	QString reffname = filenames.last();
	int nx=0, ny=0, nr=0;

	if (reffname.length() > 19) {
		int nameX = reffname.lastIndexOf("/");
		QStringList tokens = reffname.mid(nameX+16,reffname.length()-nameX-21).split('.');
		foreach (QString token, tokens) {
			bool ok = false;
			int value = token.mid(1).toInt(&ok,10)+1;
			if (ok)
				switch (token[0].toLatin1()) {
					case 'R' : {
						nr = value;
						break;
						}
					case 'X' : {
						nx = value;
						break;
						}
					case 'Y' : {
						ny = value;
						break;
						}
					case 'M' :
					case 'L' : {
						nx = value;
						break;
						}
					}
			else
				NVBOutputError(QString("Unknown token in filename: %1").arg(token));
			}
		}
// Actually all that was rather unnecessary - this information is only needed for the new NVBAxedData
	QList<CreatecVertPage*> result;
	QVector<QColor> pcolors(12); // Colors are taken from the STMAFM program by Createc

	for (int i = 0; i<12; i++)
		result << new CreatecVertPage();

	result[0]->pagename = "I";
	result[0]->zd = NVBDimension("A");
	pcolors[0] = Qt::green;
	result[1]->pagename = "dI";
	result[1]->zd = NVBDimension("V");
	pcolors[1] = Qt::blue;
	result[2]->pagename = "U";
	result[2]->zd = NVBDimension("V");
	pcolors[2] = Qt::green;
	result[3]->pagename = "z";
	result[3]->zd = NVBDimension("nm");
	pcolors[3] = Qt::blue;
	result[4]->pagename = "dI2";
	result[4]->zd = NVBDimension("V");
	pcolors[4] = Qt::red;
	result[5]->pagename = "dI_q";
	result[5]->zd = NVBDimension("DAC",false);
	pcolors[5] = Qt::green;
	result[6]->pagename = "dI2_q";
	result[6]->zd = NVBDimension("DAC",false);
	pcolors[6] = Qt::green;
	result[7]->pagename = "AD0";
	result[7]->zd = NVBDimension("A");
	pcolors[7] = Qt::gray;
	result[8]->pagename = "AD1";
	result[8]->zd = NVBDimension("V");
	pcolors[8] = Qt::darkRed;
	result[9]->pagename = "AD2";
	result[9]->zd = NVBDimension("V");
	pcolors[9] = Qt::green;
	result[10]->pagename = "AD3";
	result[10]->zd = NVBDimension("DAC",false);
	pcolors[10] = Qt::green;
	result[11]->pagename = "Dac0";
	result[11]->zd = NVBDimension("DAC",false);
	pcolors[11] = Qt::green;


	foreach (QString filename, filenames) {
		QFile file(filename);

		if (!file.open(QIODevice::ReadOnly)) {
				NVBOutputFileError(&file);
				return QList<NVBDataSource*>();
				}

		CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);

		file.seek(0);

		QString format(file.readLine(20));
		format.chop(2);

		if (format != "[Parameter]") {
			NVBOutputError(QString("Don't know how to deal with format %1").arg(format));
			return QList<NVBDataSource*>();
		}

    // initialise data

		file.seek(0x4006);
		QStringList sizes(QString(file.readLine(200)).split(' ',QString::SkipEmptyParts));

		int npts = sizes.at(0).toInt();
		double Xdac = sizes.at(1).toInt() - file_header.value("Scanrotoffx").toDouble();
		double Ydac = sizes.at(2).toInt() - file_header.value("Scanrotoffy").toDouble();
		double dacFactor = file_header.value("Dacto[A]xy").toDouble()*1e-9;
		QPointF pos = QPointF(Xdac*dacFactor,Ydac*dacFactor);

		NVBOutputDMsg(QString("X: %1, Y: %2 : DAC %3 -> Point %4 x %5").arg(Xdac).arg(Ydac).arg(dacFactor,8).arg(pos.x(),8).arg(pos.y(),8));

//	int nchannels = file_header.value("Channels").toInt();
//  int data_points = file_header.value("Num.X",0).toInt() * file_header.value("Num.Y",0).toInt();
//  if (data_points == 0) return result;

		QVector<double> factor(12,1);

		//-// DAC factors
		// Current
		factor[0] = file_header.value("Dacto[A]z").toDouble()*
								pow(10,-file_header.value("Gainpreamp").toInt())/
								file_header.value("ZPiezoconst").toDouble();
		factor[7] = factor[0];

		// Z
		factor[3] = file_header.value("Dacto[A]z").toDouble();

		// V
		factor[1] = file_header.value("Dacto[A]z").toDouble()*0.1/
								file_header.value("ZPiezoconst").toDouble();
		factor[2] = factor[1];
		factor[4] = factor[1];
		factor[8] = factor[1];
		factor[9] = factor[1];

		QVector<double> xs(0);
		xs.reserve(npts);

		QVector< QVector<double> > ys(12);
		for (int i = 0; i<12; i++)
			ys[i].reserve(npts);

		while (!file.atEnd()) {
			QList<QByteArray> pt_data = file.readLine(800).split('\t');
			xs << pt_data.first().toDouble();
			for (int i = 0; i<12; i++)
				ys[i] << pt_data.at(i+1).toDouble()*factor.at(i);
			}

		for (int i = 0; i<12; i++) {
			result[i]->_data << new QwtArrayData(xs,ys.at(i));
			result[i]->_positions << pos;
			}

		}

	for (int i = 0; i<12; i++) {
		result[i]->setColorModel(new NVBConstDiscrColorModel(pcolors.at(i)));
		result[i]->_datasize = QSize(result.first()->_data.first()->size(),filenames.count());
		result[i]->xd = NVBDimension("m");
		result[i]->yd = NVBDimension("m");
		result[i]->td = NVBDimension("p");
		}

	QList<NVBDataSource*> rconv;
	foreach(NVBDataSource * s, result)
		rconv << s;
	return rconv;
}

CreatecLatPage::CreatecLatPage( QFile & file ):header(CreatecFileGenerator::getCreatecHeader(file))
{
}

CreatecTSpecPage::CreatecTSpecPage( QFile & file ):header(CreatecFileGenerator::getCreatecHeader(file))
{
}

Q_EXPORT_PLUGIN2(createc, CreatecFileGenerator)
