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
#include "dimension.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
#include <QDir>
#include "NVBFileInfo.h"
#include <QDebug>
#include <zlib.h>

#define TMP_VOLTAGE_MAP 1

QStringList CreatecFileGenerator::availableInfoFields() const {
    return QStringList() \
            << "Bias" \
            << "Setpoint" \
						<< "Spectroscopy feedback mode" \
						<< "Scanmode" \
						<< "CHMode" \
						<< "VFBMode" \
						<< "RotCMode" \
						<< "Latmanmode" \
						<< "LockinMode" \
						<< "z(V) setpoint current" \
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

	NVBFile * f = new NVBFile(info);
	if (!f) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	// OK, here we have a couple of possibilities
	//     - we might have a (dat) file [just one]
	//     - we might have a bunch of (vert) files
	//     - we might have a (tspec) or a (lat) file [that we don't know how to read]

	QString ffname = info.first();
	QString ext = ffname.right(ffname.size()-ffname.lastIndexOf('.')-1).toLower();
	if (ext == "dat") {
		f->addSources( CreatecDatPage::loadAllChannels(ffname) );
		if (info.count() > 1)
			NVBOutputPMsg("Associated files include more that one *.dat file");
		}
//	else if (ext == "lat")
//		f->addSource( new CreatecLatPage(file) );
	else if (ext == "vert")
		f->addSources( CreatecVertPage::loadAllChannels(info) );
//	else if (ext == "tspec")
//	f->addSource( new CreatecTSpecPage(file) );
	else {
		NVBOutputError(QString("Didn't recognise file format of %1").arg(ffname));
		}

	if (f->rowCount() == 0) {
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
	
	comments.insert("CHMode",header.value("CHMode").toInt());
	comments.insert("VFBMode",header.value("VFBMode").toInt());
	comments.insert("RotCMode",header.value("RotCMode").toInt());
	if (type == NVB::SpecPage) comments.insert("Latmanmode",header.value("Latmanmode").toInt());
	comments.insert("LockinMode",header.value("LockinMode").toInt());

	comments.insert("DAC resolution",NVBPhysValue(header.value("DAC-Type").toString(),false));
	//	Titel=default            //----- Never saw that set. Theoretically could be used to name pages
	//	Delta X = 512            //-[]--- Dac step in X
	//	Delta Y = 512            //-[]--- Dac step in Y
	//	Num.X = 256              //-[]--- Number of points in X
	//	Num.Y = 256              //-[]--- Number of points in Y
	//	Delay X+ = 1             //--[]-- ? Some time parameter
	//	Delay X- = 1             //--[]-- ? Some time parameter
	//	Delay Y  = 1             //--[]-- ? Some time parameter
	//	D-DeltaX = 8             //--[]-- X step size
	//	GainX = 10	           	 //-[]--- X Gain
	//	GainY = 10               //-[]--- Y Gain
	//	GainZ = 3                //-[]--- Z Gain
	//	Rotation = 0             //--[]-- Rotation angle
	comments.insert("Bias",header.value("BiasVoltage").toPhysValue());
	comments.insert("Amplifier setting",header.value("Gainpreamp").toInt());
	//	Chan(1,2,4) = 1          //-[]--- Channel number. Already used in number of pages
	//	PlanDx = 0.0000          //----- X Plane subtract param. Not very useful
	//	PlanDy = 0.0000          //----- X Plane subtract param. Not very useful
	//	Scanrotoffx = -99611.7   //-[]--- X Offset
	//	Scanrotoffy = -225437.0  //-[]--- Y Offset
	//	MVolt_1 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	MVolt_2 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	MVolt_3 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	MVolt_4 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	MVolt_5 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	MVolt_6 = 0	           	 //====== Multi-voltage measurement. (has to be treated somehow)
	//	RepeatRotinc = 0
	//	RptBVoltinc = 0
	//	Repeatinterval = 0
	//	Repeatcounter = 0
	//	RepeatXoffset = 0
	//	RepeatYoffset = 0
	//	Scantype = 1
	//	Scanmode = 2 //---- 1 means forward, 2 meand forward + backward
	//	Scancoarse = 1
	//	CHMode = 0
	//	Channels = 2
	//	Preamptype = 1
	//	VFBMode = 0
	//	RotCMode = 0
	//	CHModeZoff = 0
	//	CHModeGainpreamp = 6
	//	Latmanmode = 1
	//	LatmResist = 1.000E+00
	//	LatmanVolt = 1000
	//	Latmanlgi = 0
	//	Latmangain = 8
	//	Latmanddx = 32
	//	Latmandelay = 96
	//	Latm0Delay = 2
	//	Latmanccdz = 0
	//	Latmanextension = 1.000
	//	TipForm_Volt = -3000
	//	TipForm_Z = 100
	//	Tip_Delay = 100
	//	Tip_Latddx = 32
	//	Tip_LatDelay = 102
	//	Tip_Gain = 6
	//	Vertmangain = 8
	//	Vertmandelay = 10
	//	Vpoint0.t = 0
	//	Vpoint1.t = 400
	//	Vpoint2.t = 600
	//	Vpoint3.t = 1024
	//	Vpoint0.V = 0
	//	Vpoint1.V = -1000
	//	Vpoint2.V = -1000
	//	Vpoint3.V = 0
	//	Zpoint0.t = 0
	//	Zpoint1.t = 400
	//	Zpoint2.t = 600
	//	Zpoint3.t = 1024
	//	Zpoint0.z = 0
	//	Zpoint1.z = -100
	//	Zpoint2.z = -100
	//	Zpoint3.z = 0
	//	Zoffset = 0
	//	Zdrift = 0
	//	VertSpecBack = 0
	//	VertSpecAvrgnr = 1
	//	VertRepeatCounter = 1
	//	VertLineCount = 2
	//	SpecXGrid = 256
	//	SpecYGrid = 100000
	//	SpecXYGridDelay = 10
	//	SpecGridChan = 2
	//	VerttreshImax = 20000
	//	VerttreshImin = -20000
	//	VertAvrgdelay = 100
	if (type == NVB::SpecPage)
		switch(header.value("VertFBMode").toInt()) {
			case 4:
				comments.insert("Spectroscopy feedback mode",QString("I(V,z)"));
				break;
			case 1:
				comments.insert("Spectroscopy feedback mode",QString("z(V)"));
				break;
			default:
				NVBOutputError(QString("Unknown VertFBMode value: %1").arg(header.value("VertFBMode").toInt()));
			}
	comments.insert("z(V) setpoint current",NVBPhysValue(header.value("VertFBLogiset").toDouble()*exp10(-header.value("Vertmangain").toInt()),NVBDimension("mA")));
	//	Imageframe = 0
	//	Imagegrayfactor = 32.416
	//	Zoom = 2.00
	//	OrgPlanX = 0.00000
	//	OrgPlanY = 0.00000
	//	OrgPlanOff = 0.00000
	//	Dacto[A]xy = 0.00191
	//	Dacto[A]z = 0.00038
	//	Planoff = 0
	//	Planx = 0
	//	Plany = 0
	//	Plany2 = 0
	//	Planavrgnr = 17
	//	Planoffset2 = 0
	//	Plano2start = 0
	//	Imagebackoffset = 0
	//	Frameimageoffset = 1000
	//	DigZoomX = 1.00000
	//	DigZoomZ = 1.00000
	//	FBLogIset = 255.100
	//	FBIntegral = -28.481974
	//	FBProp = 0.000000
	//	FBVoltRC = 0.000100
	//	FBVoltGain = 10000.000
	//	CurrentRC = 0.9000
	//	FBLog = 1
	//	Imaxcurrent = 1000000.0
	//	Imaxdelay = 1
	//	ImaxZret = 0.000
	//	SpecAvrgnr = 32
	//	SpecFreq = 10000.000
	//	FFTPoints = 1024
	//	LockinFreq = 100
	//	LockinAmpl = 100
	//	LockinPhase = 0
	//	LockinPhase2 = 0
	//	LockinRC = 0.00100
	//	LockinMode = 0
	//	SBC_Clk[MHz] = 150
	//	DSP_Clock = 50000
	//	SRS_Frequency = 1000.000
	//	SRS_ModVoltage = 1.0000
	//	SRS_InpGain[V] = 1.0E+00
	//	SRS_InpTimeC[s] = 1.0E+00
	//	LockinRefAmpl = 0
	//	LockinRefPhase = 0
	//	DAC5[V]=0.00000
	//	UserPreampCode = 6:011000/8:101000/10:110000/12:111000/
	//	HPIB_Address = hpib7,9
	//	Upinc = 20164
	//	Upcount = 13
	//	Rotinc = 20164
	//	Rotincquad = 0
	//	Rotcount = 13
	//	XYBurst = 1000
	//	RotBurst = 1000
	//	AproBurst = 2
	//	AproPeriod = 300
	//	X-Puls-Count = -9300
	//	Y-Puls-Count = -12240
	//	Z-Puls-Count = -2988699
	//	Autolevel = 0
	//	Scandvinc[DAC] = 0
	//	Z-Res. [A]: +/- =4.749
	//	Length x[A] = 2500.0763
	//	Length y[A] = 2500.0763
	//	Biasvolt[mV] = 511 //---- Duplicate of BiasVoltage
	comments.insert("Setpoint",header.value("Current").toPhysValue());
	//	Sec/line: = 0.328
	//	Sec/Image: = 167.772
	//	ActGainXYZ = 10 10 3
	//	Channels = 2
	//	T-STM: = 0.00000
	//	HP_Ch1 = 0.00000
	//	HP_Ch2 = 0.00000
	//	HP_Ch3 = 0.00000
	//	HP_Ch4 = 0.00000
	//	ZPiezoconst = 20.00
	//	Xpiezoconst = 100.00
	//	YPiezoconst = 100.00
	//	T_ADC2[K]=  0.000
	//	T_ADC3[K]=  0.000

	int nchannels = header.value("Channels").toInt();
#ifdef TMP_VOLTAGE_MAP
	if (type == NVB::SpecPage) nchannels = 13;
#else
	if (type == NVB::SpecPage) nchannels = 12;
#endif

	QSize dataSize;
	if (type == NVB::SpecPage) {
		int vpmax = qMax(qMax(header.value("Vpoint0.t",0).toInt(),header.value("Vpoint1.t",0).toInt()),qMax(header.value("Vpoint2.t",0).toInt(),header.value("Vpoint3.t",0).toInt()));
		int zpmax = qMax(qMax(header.value("Zpoint0.t",0).toInt(),header.value("Zpoint1.t",0).toInt()),qMax(header.value("Zpoint2.t",0).toInt(),header.value("Zpoint3.t",0).toInt()));
		dataSize = QSize(qMax(vpmax,zpmax),info.count());
		}
	else
		dataSize = QSize(header.value("Num.X",0).toInt(),header.value("Num.Y",0).toInt());
	for (int i = 0; i < nchannels; i++)	
		fi->pages.append(NVBPageInfo(header.value("BiasVoltage",QString()).toString(),type,dataSize,comments));

	return fi;
}

QStringList CreatecDatPage::channelNames = QStringList( QStringList("Topography") + QStringList("Current") + QStringList("ADC1") + QStringList("ADC2") );

QList<NVBDataSource*> CreatecDatPage::loadAllChannels(QString filename) {

	QList<NVBDataSource*> result;
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return result;
		}
	if ( !QString(file.readLine(100)).contains("[param",Qt::CaseInsensitive) ) {
		NVBOutputError(QString("Unknown file format in %1").arg(filename));
		return result;
		}

  CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);

    // initialise data

  int nchannels = file_header.value("Channels").toInt();

  int data_points = file_header.value("Num.X",0).toInt() * file_header.value("Num.Y",0).toInt();

	if (data_points == 0) {
		NVBOutputError(QString("Zero data size in %1").arg(filename));
		return result;
		}

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

	comments.insert("Bias",header.value("BiasVoltage").toPhysValue());
	comments.insert("Setpoint",header.value("Current").toPhysValue());
	comments.insert("Scanmode",header.value("Scanmode").toInt());
	comments.insert("CHMode",header.value("CHMode").toInt());
	comments.insert("VFBMode",header.value("VFBMode").toInt());
	comments.insert("RotCMode",header.value("RotCMode").toInt());
	comments.insert("LockinMode",header.value("LockinMode").toInt());

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

	int nameX = reffname.lastIndexOf("/");
	if (reffname.length()-nameX > 20) {
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

#ifdef TMP_VOLTAGE_MAP
	CreatecHeader file_headerX;
#endif

	foreach (QString filename, filenames) {
		QFile file(filename);

		if (!file.open(QIODevice::ReadOnly)) {
				NVBOutputFileError(&file);
				return QList<NVBDataSource*>();
				}

		CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);
#ifdef TMP_VOLTAGE_MAP
		file_headerX = file_header;
#endif

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

		if (sizes.count() < 3) {
			NVBOutputError("Wrong file format - no datasize section after DATA");
			return QList<NVBDataSource*>();
		}

		int npts = sizes.at(0).toInt();
		double Xdac = sizes.at(1).toInt() - file_header.value("Scanrotoffx").toDouble();
		double Ydac = sizes.at(2).toInt() - file_header.value("Scanrotoffy").toDouble();
		double dacFactor = file_header.value("Dacto[A]xy").toDouble()*1e-9;
		QPointF pos = QPointF(Xdac*dacFactor,Ydac*dacFactor);

//		NVBOutputDMsg(QString("X: %1, Y: %2 : DAC %3 -> Point %4 x %5").arg(Xdac).arg(Ydac).arg(dacFactor,8).arg(pos.x(),8).arg(pos.y(),8));

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
		factor[2] = 1e-3;
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

#ifdef  TMP_VOLTAGE_MAP
/* Problems with this approach :
 * No forward/backward curves
 * No multi-step voltage curves
 * 
 */

		if (file_headerX.value("Vpoint0.t").toInt() == 0) {
			result << new CreatecVertPage();
			result.last()->setColorModel(new NVBConstDiscrColorModel(Qt::black));
			if (file_headerX.value("VertFBMode").toInt() == 1)
				result.last()->pagename = "z(U)";
			else
				result.last()->pagename = "dI/dV";
			result.last()->zd = NVBDimension("V");
			result.last()->xd = NVBDimension("m");
			result.last()->yd = NVBDimension("m");
			result.last()->td = NVBDimension("V");
			result.last()->_positions = result.first()->positions();

			int npts = file_headerX.value("Vpoint1.t").toInt();
			result.last()->_datasize = QSize(npts,filenames.count());

			for(int c = 0; c < filenames.count(); c++) {
				QVector<double> xs(npts), ys(npts);

				for(int i = 0; i<npts; i++) {
					xs[i] = result[2]->_data[c]->y(i);
					ys[i] = result[1]->_data[c]->y(i);
					}
				result.last()->_data << new QwtArrayData(xs,ys);
				}
			}
		
#endif

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
