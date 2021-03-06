//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#include "createc.h"

#include <QtCore/QDir>
#include <zlib.h>

#include "NVBLogger.h"
#include "NVBUnits.h"
#include "NVBScaler.h"
#include "NVBColorMaps.h"
#include "NVBFileInfo.h"
#include "NVBFile.h"
#include "NVBAxisMaps.h"
#include "NVBPlugin.h"

#define SCANTYPE_SPECTROSCOPY    0 // Spectroscopy
#define SCANTYPE_SINGLEVOLTAGE   1 // Single-Voltage
#define SCANTYPE_MULTIVOLTAGE    2 // Multi-Voltage
#define SCANTYPE_ZRAMPING        3 // Z-Ramping
#define SCANTYPE_NANOSTRUCTURING 4 // Nanostructuring

static QStringList DATchannelNames =
  QStringList()
  << "Topography"
  << "Current"
  << "ADC1"
  << "ADC2"
  << "ADC3"
  << "dI/dV"
  << "d2I/dV2"
  << "df"
  << "Damping"
  << "Amplitude"
  << "CP"
  << "AUX1"
  ;

static QList<NVBUnits> DATchannelDims =
  QList<NVBUnits>()
  << NVBUnits("nm")
  << NVBUnits("A")
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  ;

static QStringList VERT3channelNames =
  QStringList()
  << "Current"
  << "dI/dV"
  << "d2I/dV2"
  << "ADC0"
  << "ADC1"
  << "ADC2"
  << "ADC3"
  << "df"
  << "Damping"
  << "Amplitude"
  << "dI_q"
  << "dI2_q"
  << "Topography"
  << "CP"
  ;

static QList<NVBUnits> VERT3channelDims =
  QList<NVBUnits>()
  << NVBUnits("A")
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("DAC", false)
  << NVBUnits("nm")
  ;

QStringList CreatecFileGenerator::availableInfoFields() const {
	return QStringList()
	       << "Bias"
	       << "Setpoint"
	       << "Z piezo constant" // ZPiezoconst=  7.33
	       << "X piezo constant" // Xpiezoconst= 30.92
	       << "Y piezo constant" // YPiezoconst= 30.92
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
// Scanmode / Scanmode=2 // Forward / Fwd+Bck scans
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

NVBDataComments CreatecFileGenerator::commentsFromHeader(const CreatecHeader & header) const {
	NVBDataComments c;

	c.insert("Bias", header.value("BiasVoltage").toPhysValue());
	c.insert("Setpoint", header.value("Current").toPhysValue());
	c.insert("Z piezo constant", header.value("ZPiezoconst").toPhysValue());
	c.insert("X piezo constant", header.value("XPiezoconst").toPhysValue());
	c.insert("Y piezo constant", header.value("YPiezoconst").toPhysValue());
	c.insert("STM preset", header.value("Titel").toPhysValue());

	return c;
	}

NVBAssociatedFilesInfo CreatecFileGenerator::associatedFiles(QString filename) const {
	static QRegExp multispec = QRegExp("[/\\\\](A[0-9]*\\.[0-9]*)\\.[XMLR][0-9].*\\.VERT$", Qt::CaseInsensitive, QRegExp::RegExp);

	if (!filename.contains(multispec))
//		if (multispec.indexIn(filename) == -1)
		return NVBFileGenerator::associatedFiles(filename);
	else {
		QString path = QFileInfo(filename).absolutePath();
		QStringList files = QDir(path, QString("%1*VERT").arg(multispec.cap(1)), QDir::Name, QDir::Files).entryList();

		for(QStringList::iterator it = files.begin(); it != files.end(); it++)
			*it = path + "/" + *it;

		return NVBAssociatedFilesInfo( multispec.cap(1) + ".VERT", files, this);
		}
	}

CreatecHeader CreatecFileGenerator::getCreatecHeader( QFile & file ) {
	CreatecHeader h;

	QByteArray b;
	QString pname1, value;
	QRegExp dimmatch("^(.*)\\[(.*)\\]$");
	NVBVariant tval;

	while (true) {

		b = file.readLine(100);

		if ( b[0] == '\0' ) break;

		b.chop(2); // 0x0D 0x0A

		QString s = QString(b);
		int i = s.indexOf('=');

		if (i <= 0) continue;

		QString sleft = s.left(i);
		QString dimstr;

		if (dimmatch.indexIn(sleft) == 0) {
			sleft = dimmatch.cap(1);
			dimstr = dimmatch.cap(2);
			}

		value = s.right(s.size() - i - 1);
		bool ok;
		int ival = value.toInt(&ok);

		if (ok) {
			if (dimstr.isEmpty())
				tval = ival;
			else
				tval = NVBPhysValue(ival, NVBUnits(dimstr));
			}
		else {
			double dval = value.toDouble(&ok);

			if (ok) {
				if (dimstr.isEmpty())
					tval = dval;
				else if (dimstr == "A")
					tval = NVBPhysValue(dval, NVBUnits(QString("m"), 1e-10));
				else
					tval = NVBPhysValue(dval, NVBUnits(dimstr));
				}
			else {
				if (!dimstr.isEmpty()) NVBOutputError("Dimensionised string in " + s);
				else
					tval = value;
				}
			}

		pname1 = sleft.section(" / ", 0, 0);
		h.insert(pname1, tval);
//     pname2 = s.left(i).section(" / ",0,0);
		}

	return h;
	}

NVBFile * CreatecFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	NVBFile * f = new NVBFile(info);

	if (!f)	{
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	// OK, here we have a couple of possibilities
	//     - we might have a (dat) file [just one]
	//     - we might have a bunch of (vert) files
	//     - we might have a (tspec) or a (lat) file [that we don't know how to read]

	QString ffname = info.first();
	QString ext = ffname.right(ffname.size() - ffname.lastIndexOf('.') - 1).toLower();

	if (ext == "dat") {
		loadAllChannelsFromDAT(ffname, f);

		if (info.count() > 1)
			NVBOutputPMsg("Associated files include more that one *.dat file");
		}
	else if (ext == "lat")
		loadAllChannelsFromLAT(ffname, f);
	else if (ext == "vert")
		// FIXME we suppose here, that all the vert files in AFI belong to the same set
		loadAllChannelsFromVERT(info, f);
	else if (ext == "tspec")
		loadAllChannelsFromTSPEC(ffname, f);
	else {
		NVBOutputError(QString("Didn't recognise file format of %1").arg(ffname));
		}

	if (f->count() == 0) {
		delete f;
		return 0;
		}

	return f;
	}

void addDataInfoFB(bool fandb, NVBFileInfo * fi, QString name, const NVBUnits & u, const QList<NVBAxisInfo> & axes) {
	if (fandb) {
		fi->append(NVBDataInfo(name + " (forward)", u, axes, NVBDataComments(), NVBDataSet::Topography));
		fi->append(NVBDataInfo(name + " (backward)", u, axes, NVBDataComments(), NVBDataSet::Topography));
		}
	else
		fi->append(NVBDataInfo(name, u, axes, NVBDataComments(), NVBDataSet::Topography));
	}

NVBFileInfo * CreatecFileGenerator::loadFileInfo( const NVBAssociatedFilesInfo & info ) const throw() {
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

	/*
		Possible first strings
		[Paramet32] - raw topography
		[Paramco32] - compressed topography
		[Parameter] - spectroscopy
		[ParVERT30] - spectroscopy version 3.0
	 */


	if (!QString(file.readLine(100)).contains(QRegExp("\[Par(VERT30|am(et32|co32|eter))]", Qt::CaseInsensitive)))
		return 0;

	CreatecHeader header = getCreatecHeader(file);
	NVBDataComments comments = commentsFromHeader(header);

	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) return 0;

	fi->filterAddComments(comments); // The comments are the same for all

	//  QString name;
	QString filename = info.first();
	QString ext = filename.right(filename.size() - filename.lastIndexOf('.') - 1).toLower();

	if (ext == "dat") {
		QList<NVBAxisInfo> axes;
		axes << NVBAxisInfo("X", header.value("Num.X").toInt(), header.value("Length x").toPhysValue())
		     << NVBAxisInfo("Y", header.value("Num.Y").toInt(), header.value("Length y").toPhysValue());

		if (header.value("Scantype").toInt() == SCANTYPE_MULTIVOLTAGE) {
			int multiVoltagePages = 0;

			for(int vch = 1; vch <= 30; vch++)
				if (header.value(QString("MVolt_%1").arg(vch)).toDouble() != 0)
					multiVoltagePages += 1;
				else
					break; // A single 0 blocks all the rest

			axes << NVBAxisInfo("Bias", multiVoltagePages,
			                    NVBPhysValue(abs(header.value("MVolt_1").toDouble() - header.value(QString("MVolt_%1").arg(multiVoltagePages)).toDouble()), NVBUnits("V")));
			}


		bool backward = (header.value("Scanmode").toInt() == 2); // 1 means only forward
		int chanSelect = header.value("Channelselectval", 0).toInt();

		if (chanSelect == 0)
			switch(header.value("Chan(1,2,4)").toInt()) {
				case 1 : chanSelect = 0x1; break;

				case 2 : chanSelect = 0x3; break;

				case 4 : chanSelect = 0xF; break;

				default : chanSelect = 0x1; NVBOutputError("Weird number of channels");
				}

		for (int i = 0; i < DATchannelNames.count(); i++)
			if (chanSelect & (1 << i))
				addDataInfoFB(backward, fi, DATchannelNames.at(i), DATchannelDims.at(i), axes);
		}
	else if (ext == "vert") {

		file.seek(0x4006);
		QStringList dataParams = QString(file.readLine(200)).split(' ', QString::SkipEmptyParts);
		int npts = dataParams.first().toInt();

		QString reffname = info.last();
		int nx = 0, ny = 0, np = 0, nr = 0;

		int nameX = reffname.lastIndexOf("/");

		if (reffname.length() - nameX > 20) {
			QStringList tokens = reffname.mid(nameX + 16, reffname.length() - nameX - 21).split('.');
			foreach (QString token, tokens) {
				bool ok = false;
				int value = token.mid(1).toInt(&ok, 10) + 1;

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
							np = value;
							break;
							}
						}
				else
					NVBOutputError(QString("Unknown token in filename: %1").arg(token));
				}
			}

		QList<NVBAxisInfo> axes;

		axes << NVBAxisInfo("Time", npts, NVBPhysValue(npts * header.value("Vertmandelay").toDouble() / header.value("DSP_Clock").toDouble(), NVBUnits("sec")));

		if (nr != 0)
			axes << NVBAxisInfo("Measurement", nr);

		if (nx != 0) {
			axes << NVBAxisInfo("X", nx, header.value("Length x").toPhysValue()); // X

			if (ny != 0)
				axes << NVBAxisInfo("Y", ny, header.value("Length y").toPhysValue()); // Y
			}

		if (np != 0)
			axes << NVBAxisInfo("Point", np); // "Point"

		if (dataParams.count() < 4) { // No channel select parameter - ver 2
			fi->append(NVBDataInfo("I",    NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("dI",   NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("U",    NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("z",    NVBUnits("nm"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("dI2",  NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("dI_q", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("dI2_q", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("AD0",  NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("AD1",  NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("AD2",  NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("AD3",  NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("Dac0", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			}
		else { // ver 3
			int chanSelect = dataParams.at(3).toInt();

			fi->append(NVBDataInfo("U",    NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			fi->append(NVBDataInfo("z",    NVBUnits("nm"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x1)
				fi->append(NVBDataInfo("I",    NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x2)
				fi->append(NVBDataInfo("dI/dV",   NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x4)
				fi->append(NVBDataInfo("d2I/dV2",  NVBUnits("A/V^2"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x8)
				fi->append(NVBDataInfo("ADC0",  NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x10)
				fi->append(NVBDataInfo("ADC1",  NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x20)
				fi->append(NVBDataInfo("ADC2",  NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x40)
				fi->append(NVBDataInfo("ADC3",  NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x80)
				fi->append(NVBDataInfo("df", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x100)
				fi->append(NVBDataInfo("Damping", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x200)
				fi->append(NVBDataInfo("Amplitude", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x400)
				fi->append(NVBDataInfo("dI_q", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x800)
				fi->append(NVBDataInfo("dI2_q", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x1000)
				fi->append(NVBDataInfo("Topography", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (chanSelect & 0x2000)
				fi->append(NVBDataInfo("CP", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
			}
		}
	else if (ext == "lat") {
		file.seek(0x4006);
		int npts = QString(file.readLine(200)).split(' ', QString::SkipEmptyParts).first().toInt();
		QList<NVBAxisInfo> axes;
		axes << NVBAxisInfo("Motion", npts,
		                    NVBPhysValue(npts * header.value("Latmandelay").toDouble() / header.value("DSP_Clock").toDouble(), "s"));
		fi->append(NVBDataInfo("Z", NVBUnits("nm"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
		fi->append(NVBDataInfo("I", NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
		}
	else if (ext == "tspec") {
		file.seek(0x4002);
		int nparam = file.readLine(800).split('\t').count() - 3;
		QList<NVBAxisInfo> axes;
		axes << NVBAxisInfo("Time", header.value("FFTPoints").toInt(),
		                    NVBPhysValue(header.value("FFTPoints").toInt() / header.value("SpecFreq").toDouble(), "s"));

		fi->append(NVBDataInfo("Z", NVBUnits("nm"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
		fi->append(NVBDataInfo("I", NVBUnits("A"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

		if (nparam > 4) {
			fi->append(NVBDataInfo("dI", NVBUnits("V"), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (nparam > 6) {
				fi->append(NVBDataInfo("ADC2", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
				fi->append(NVBDataInfo("ADC3", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
				}
			}

		fi->append(NVBDataInfo("FFT Z", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
		fi->append(NVBDataInfo("FFT I", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

		if (nparam > 4) {
			fi->append(NVBDataInfo("FFT dI", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));

			if (nparam > 6) {
				fi->append(NVBDataInfo("FFT ADC2", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
				fi->append(NVBDataInfo("FFT ADC3", NVBUnits("DAC", false), axes, NVBDataComments(), NVBDataSet::Spectroscopy));
				}
			}
		}
	else {
		NVBOutputError(QString("Unrecognizable file extension in %1").arg(filename));
		}

	file.close();

	return fi;
	}

void CreatecFileGenerator::loadAllChannelsFromDAT(QString filename, NVBFile* sources) const {

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return;
		}

	if ( !QString(file.readLine(100)).contains("[param", Qt::CaseInsensitive) ) {
		NVBOutputError(QString("Unknown file format in %1").arg(filename));
		return;
		}

	CreatecHeader file_header = getCreatecHeader(file);
	NVBDataComments comments = commentsFromHeader(file_header);

	// initialise data

	int nchannels = file_header.value("Channels").toInt();

	int data_points = file_header.value("Num.X", 0).toInt() * file_header.value("Num.Y", 0).toInt();

	if (data_points == 0) {
		NVBOutputError(QString("Zero data size in %1").arg(filename));
		return;
		}

	int page_data_volume = data_points * sizeof(double); // including multi-voltage

	bool backward = (file_header.value("Scanmode").toInt() == 2); // 1 means only forward
	int chanSelect = file_header.value("Channelselectval", 0).toInt();

	if (chanSelect == 0)
		switch(file_header.value("Chan(1,2,4)").toInt()) {
			case 1 : chanSelect = 0x1; break;

			case 2 : chanSelect = 0x3; break;

			case 4 : chanSelect = 0xF; break;

			default : chanSelect = 0x1; NVBOutputError("Weird number of channels");
			}

	// Fill in Z factors

	QList<double> factors;

// Topography. Scale by Dacto[A]z, taking into account that they are nanometers.
	if (chanSelect & 0x1)
		factors << file_header.value("Dacto[A]z").toDouble();

// Current. Scale by piezo constant, Dacto[A]z, taking into account that they are Angstroems now.
	if (chanSelect & 0x2)
		factors << file_header.value("Dacto[A]z").toDouble()*
		        pow(10, -file_header.value("Gainpreamp").toInt()) /
		        file_header.value("ZPiezoconst").toDouble();

// ADC1 -> STM dependent, rely on NVBScript.
	if (chanSelect & 0x4)
		factors << 1;

// ADC2
	if (chanSelect & 0x8)
		factors << 1;

// ADC3
	if (chanSelect & 0x10)
		factors << 1;

// dI/dV ?
	if (chanSelect & 0x20)
		factors << 1;

// d2I/dV2
	if (chanSelect & 0x40)
		factors << 1;

// df
	if (chanSelect & 0x80)
		factors << 1;

// Damping - FIXME AFM (should be possible to convert to physical units)
	if (chanSelect & 0x100)
		factors << 1;

// Amplitude - FIXME AFM (should be possible to convert to physical units)
	if (chanSelect & 0x200)
		factors << 1;

// CP
	if (chanSelect & 0x400)
		factors << 1;

// AUX1
	if (chanSelect & 0x800)
		factors << 1;

	QList<int> invIndexes;

	for (int i = 0; i < DATchannelNames.count(); i++)
		if (chanSelect & (1 << i))
			invIndexes << i;

// There are always only 2 axes : X & Y

// FIXME Add rotation
// angle = -header.value("Rotation",0).toDouble();

	NVBConstructableDataSource* result = new NVBConstructableDataSource(sources);

	result->addAxis("X", file_header.value("Num.X", 0).toInt());
	result->addAxisMap(
	  new NVBAxisPhysMap(
	    NVBPhysValue(
	      -1e-9 * file_header.value("Scanrotoffx", 0).toInt()*
	      file_header.value("Dacto[A]xy", 0).toDouble()
	      - 0.5 * 1e-10 * file_header.value("Length x", 0).toPhysValue().getValue()
	      , NVBUnits("m")),
	    NVBPhysValue(
	      file_header.value("Dacto[A]xy", 0).toDouble()*
	      file_header.value("Delta X", 0).toPhysValue().getValue()*
	      10e-9, NVBUnits("m"))
	  ));

	result->addAxis("Y", file_header.value("Num.Y", 0).toInt());
	result->addAxisMap(
	  new NVBAxisPhysMap(
	    NVBPhysValue(
	      -file_header.value("Scanrotoffy", 0).toInt()*
	      file_header.value("Dacto[A]xy", 0).toDouble()*
	      1e-9, NVBUnits("m")),
	    NVBPhysValue(
	      file_header.value("Dacto[A]xy", 0).toDouble()*
	      file_header.value("Delta Y", 0).toPhysValue().getValue()*
	      10e-9, NVBUnits("m"))
	  ));

	int multiVoltagePages = 1;

	if (file_header.value("Scantype").toInt() == SCANTYPE_MULTIVOLTAGE) {
		QList<double> mvolt;

		multiVoltagePages = 0;

		for(int vch = 1; vch <= 30; vch++) {
			double value = file_header.value(QString("MVolt_%1").arg(vch), 0).toDouble();

			if (value == 0)	break; // A single 0 blocks all the rest

			multiVoltagePages += 1;
			mvolt << value;
			}

		nchannels /= multiVoltagePages;
		page_data_volume *= multiVoltagePages;

		result->addAxis("Bias", multiVoltagePages);
		result->addAxisMap(new NVBAxisPhysMap(mvolt, NVBUnits("mV")));
		}

	int magic = backward ? nchannels / 2 : nchannels;

	file.seek(0);

	QString format(file.readLine(20));
	format.chop(2);

	if (format == "[Parameter]") {
		NVBOutputError("No idea how to deal with [Parameter]");
		delete result;
		return;
		}

	QVector<double*> pages(nchannels);

	for(int i = 0; i < nchannels; i++)
		pages[i] = (double*) malloc(page_data_volume);

	if (format == "[Paramet32]") {
		// this is uncompressed
		file.seek(0x4004);
		NVBValueScaler<quint32, double> intscaler;

		quint32 * idata = (quint32*)malloc(data_points * 4);

		if (!idata) {
			NVBOutputError("Memory allocation failure");
			delete result;
			foreach(double * p, pages) delete p;
			return;
			}

		for (int mvolt = 0; mvolt < multiVoltagePages; mvolt++)
			for (int channel = 0; channel < nchannels; channel++) {
				file.read((char*)idata, data_points * 4);
				intscaler.overwrite_params(0, factors.at(channel % magic));
				intscaler.scaleMem(pages[channel] + mvolt * data_points, idata, data_points);
				}

		free(idata);
		}
	else if (format == "[Paramco32]") {
		// this is compressed
		qint64 zsize = file.size() - 0x4000;
		uLongf unzsize = data_points * 800; // * nchannels

		qint8 *zbuf = (qint8*)malloc(zsize);

		if (!zbuf) {
			NVBOutputError("Memory allocation failure");
			delete result;
			return;
			}

		float *buf  = (float*)malloc(unzsize);

		if (!buf) {
			NVBOutputError("Memory allocation failure");
			free(zbuf);
			delete result;
			return;
			}

		// start of gzdata
		file.seek(0x4000);
		file.read((char*) zbuf, zsize);
		int errorcode;

		if ((errorcode = uncompress((Bytef *)buf, &unzsize, (Bytef *)zbuf, zsize)) != Z_OK)
			NVBOutputError(QString("Uncompressing failed with error %1. Buffer size %2").arg(errorcode).arg(unzsize));
		else
			NVBOutputVPMsg(QString("Uncompressed buffer to %1 bytes").arg(unzsize));

		free(zbuf);

		NVBValueScaler<float, double> intscaler;

		for (int mvolt = 0; mvolt < multiVoltagePages; mvolt++)
			for (int channel = 0; channel < nchannels; channel++) {
				intscaler.overwrite_params(0, factors.at(channel % magic));
				intscaler.scaleMem(pages[channel] + mvolt * data_points, buf + 1 + data_points * (channel + mvolt * nchannels), data_points);
				}

		free(buf);
		}
	else {
		NVBOutputError("No idea how to deal with " + format);
		delete result;
		foreach(double * p, pages) delete p;
		return;
		}

	NVBDataComments emptyComments;
	QVector<axisindex_t> axes;
	axes << 0 << 1;

	if (result->nAxes() == 3) // multivoltage
		axes << 2;

	axes.squeeze();

	for (int channel = 0; channel < nchannels; channel++)
		result->addDataSet(
		  backward ?
		  ( DATchannelNames.at(invIndexes.at(channel % magic)) + ((channel < magic) ? " (forward)" : " (backward)"))
			  : DATchannelNames.at(invIndexes.at(channel)),
			  pages[channel],
			  DATchannelDims.at(invIndexes.at(channel % magic)),
			  emptyComments,
			  axes,
			  NVBDataSet::Topography
			);

	sources->filterAddComments(comments);
	result->filterAddComments(comments);
	*sources << result;
	}

void CreatecFileGenerator::loadAllChannelsFromVERT(QStringList filenames, NVBFile* sources) const {

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
	int nx = 0, ny = 0, np = 0, nr = 0;

	if (reffname.length() > 19) {
		int nameX = reffname.lastIndexOf("/");
		QStringList tokens = reffname.mid(nameX + 16, reffname.length() - nameX - 21).split('.');
		foreach (QString token, tokens) {
			bool ok = false;
			int value = token.mid(1).toInt(&ok, 10);

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
						np = value;
						break;
						}
					}
			else
				NVBOutputError(QString("Unknown token in filename: %1").arg(token));
			}
		}


	NVBConstructableDataSource * result;
	result = new NVBConstructableDataSource(sources);

	if (!result) {
		NVBOutputError("NVBDataSource allocation failed");
		return;
		}

	// Before we add axes, we need to know some parameters

	QFile ffile(filenames.first());

	if (!ffile.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&ffile);
		delete result;
		return;
		}

	CreatecHeader header = CreatecFileGenerator::getCreatecHeader(ffile);
	NVBDataComments comments = commentsFromHeader(header);
	ffile.seek(0x4006);
	QStringList sizes(QString(ffile.readLine(200)).split(' ', QString::SkipEmptyParts));

	int npts = sizes.at(0).toInt();
	int datasize = npts;
	double X0dac = sizes.at(1).toInt() - header.value("Scanrotoffx").toDouble();
	double Y0dac = sizes.at(2).toInt() - header.value("Scanrotoffy").toDouble();
	double dacFactor0 = header.value("Dacto[A]xy").toDouble() * 1e-9;

	ffile.close();

	QVector<axisindex_t> axes;

	result->addAxis("Time", npts);
	result->addAxisMap(
	  new NVBAxisPhysMap(
	    0,
	    header.value("Vertmandelay").toDouble() / header.value("DSP_Clock").toDouble(),
	    NVBUnits("s")
	  )
	);

	if (nr != 0) {
		datasize *= nr;
		result->addAxis("Measurement", nr); // No map here -- these are just measurements
		}

	if (nx != 0) {
		datasize *= nx;
		result->addAxis("X", nx);
		result->addAxisMap(
		  new NVBAxisPhysMap(
		    X0dac * dacFactor0,
		    header.value("Length x").toDouble() / nx,
		    NVBUnits("nm")
		  )
		);

		if (ny != 0) {
			datasize *= ny;
			result->addAxis("Y", ny);
			result->addAxisMap(
			  new NVBAxisPhysMap(
			    Y0dac * dacFactor0,
			    header.value("Length y").toDouble() / ny,
			    NVBUnits("nm")
			  )
			);
			}
		}

	if (np != 0) {
		datasize *= np;
		result->addAxis("Point", np);
		}

	// Colors are taken from the STMAFM program by Createc
	// colors << Qt::green << Qt::blue << Qt::green << Qt::blue
	//        << Qt::red << Qt::green << Qt::green << Qt::gray
	//        << Qt::gray << Qt::darkRed << Qt::green << Qt::green << Qt::green

	int chanSelect = 0;
	int nChannels = 12;

	if (sizes.count() > 3) {
		nChannels = 2;
		chanSelect = sizes.at(3).toInt();

		for (int k = 0; k < 16; k++)
			if (chanSelect & (1 << k))
				nChannels += 1;
		}

	QVector<double*> datae(nChannels);

	for (int i = 0; i < nChannels; i++)
		datae[i] = (double*)malloc(datasize * sizeof(double));

	if (!chanSelect) {
		result->addDataSet("I",    datae.at(0), NVBUnits("A"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("dI",   datae.at(1), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("U",    datae.at(2), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("z",    datae.at(3), NVBUnits("nm"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("dI2",  datae.at(4), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("dI_q", datae.at(5), NVBUnits("DAC", false), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("dI2_q", datae.at(6), NVBUnits("DAC", false), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("ADC0",  datae.at(7), NVBUnits("A"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("ADC1",  datae.at(8), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("ADC2",  datae.at(9), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("ADC3",  datae.at(10), NVBUnits("DAC", false), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("Dac0", datae.at(11), NVBUnits("DAC", false), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		}
	else {
		result->addDataSet("U",    datae.at(0), NVBUnits("V"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		result->addDataSet("z",    datae.at(1), NVBUnits("nm"), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		int nextChannel = 2;

		for (int k = 0; k < 16; k++)
			if (chanSelect & (1 << k))
				result->addDataSet(VERT3channelNames.at(k), datae.at(nextChannel++), VERT3channelDims.at(k), NVBDataComments(), QVector<axisindex_t>(), NVBDataSet::Spectroscopy);
		}

	// Positions

	QList< NVBPhysPoint > points;

	int specix = 0;

	foreach (QString filename, filenames) {
		QFile file(filename);

		if (!file.open(QIODevice::ReadOnly)) {
			NVBOutputFileError(&file);
			return;
			}

		CreatecHeader file_header = CreatecFileGenerator::getCreatecHeader(file);

		file.seek(0);

		QString format(file.readLine(20));
		format.chop(2);

		if (format != "[Parameter]" && format != "[ParVERT30]") {
			NVBOutputError(QString("Don't know how to deal with format %1").arg(format));
			return;
			}

		// initialise data

		file.seek(0x4006);
		QStringList sizes(QString(file.readLine(200)).split(' ', QString::SkipEmptyParts));

		int npts = sizes.at(0).toInt();
		double Xdac = sizes.at(1).toInt() - file_header.value("Scanrotoffx").toDouble();
		double Ydac = sizes.at(2).toInt() - file_header.value("Scanrotoffy").toDouble();
		double dacFactor = file_header.value("Dacto[A]xy").toDouble() * 1e-9;
		points << NVBPhysPoint(Xdac * dacFactor, Ydac * dacFactor, NVBUnits("m"));

		NVBOutputDMsg(QString("X: %1, Y: %2 : DAC %3 -> Point %4 x %5").arg(Xdac).arg(Ydac).arg(dacFactor, 8).arg(points.last().x().getValue(), 8).arg(points.last().y().getValue(), 8));

		QVector<double> factor(nChannels, 1);

		//-// DAC factors
		if (chanSelect == 0) {
			// Current
			factor[0] = file_header.value("Dacto[A]z").toDouble() *
			            pow(10, -file_header.value("Gainpreamp").toInt()) /
			            file_header.value("ZPiezoconst").toDouble();
			factor[7] = factor[0];

			// Z
			factor[3] = file_header.value("Dacto[A]z").toDouble();

			// V
			factor[1] = file_header.value("Dacto[A]z").toDouble() * 0.1 /
			            file_header.value("ZPiezoconst").toDouble();
			factor[2] = factor[1];
			factor[4] = factor[1];
			factor[8] = factor[1];
			factor[9] = factor[1];
			}
		else {
			// V
			factor[0] = file_header.value("Dacto[A]z").toDouble() * 0.1 /
			            file_header.value("ZPiezoconst").toDouble();
			// Z
			factor[1] = file_header.value("Dacto[A]z").toDouble();
			int nextFactor = 2;

			if (chanSelect & 0x1)
				factor[nextFactor++] =
				  file_header.value("Dacto[A]z").toDouble() *
				  pow(10, -file_header.value("Gainpreamp").toInt()) /
				  file_header.value("ZPiezoconst").toDouble();

			if (chanSelect & 0x2) // dI/dV
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x4) // d2I/dV2
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x8) // ADC0
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x10) // ADC1
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x20) // ADC2
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x40) // ADC3
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x80) // df
				factor[nextFactor++] = 1;

			if (chanSelect & 0x100) // Damping
				factor[nextFactor++] = 1;

			if (chanSelect & 0x200) // Amplitude
				factor[nextFactor++] = 1;

			if (chanSelect & 0x400) // dI_q
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x800) // dI2_q
				factor[nextFactor++] = factor[0];

			if (chanSelect & 0x800) // Topography
				factor[nextFactor++] = factor[1];

			if (chanSelect & 0x800) // CP
				factor[nextFactor++] = 1;
			}

		int line = 0;

		while (!file.atEnd() && line < npts) {
//			QList<QByteArray> pt_data = file.readLine(800).split('\t');
			QByteArray thisline = file.readLine(800);
			QList<QByteArray> pt_data = thisline.split('\t');

			if (pt_data.count() < nChannels)
				NVBOutputError(QString("Line %1 too short : %2").arg(line).arg(QString(thisline)));

//			xs << pt_data.first().toDouble();
			for (int i = 0; i < nChannels; i++)
				datae.at(i)[npts * specix + line] = pt_data.at(i + 1).toDouble() * factor.at(i);

			line += 1;
			}

		specix += 1;
		}

	// Line/Set is the last axis added

	if (np > 1)
		result->addAxisMap(new NVBAxisPointMap(points));
	else // We suppose all points are the same, as they should be
		result->addComment("Position", points.first());

	sources->filterAddComments(comments);
	result->filterAddComments(comments);
	*sources << result;
	}

void CreatecFileGenerator::loadAllChannelsFromLAT(QString filename, NVBFile* sources) const {

	NVBConstructableDataSource * result = new NVBConstructableDataSource(sources);

	if (!result) {
		NVBOutputError("NVBDataSource allocation failed");
		return;
		}

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		delete result;
		return;
		}

	QString format(file.readLine(20));
	format.chop(2);

	if (format != "[Parameter]") {
		NVBOutputError(QString("Don't know how to deal with format %1").arg(format));
		delete result;
		return;
		}

	CreatecHeader header = CreatecFileGenerator::getCreatecHeader(file);
	NVBDataComments comments = commentsFromHeader(header);
	file.seek(0x4006);

	QStringList sizes(QString(file.readLine(200)).split(' ', QString::SkipEmptyParts));

	int npts = sizes.at(0).toInt();
	// The second parameter is <latmanmode>,
	// It describes how data was taken (const. current, const height etc.)
	// int latmanmode = sizes.at(1).toInt();

	file.seek(0x4014);

	result->addAxis("Motion", npts);
	result->addAxisMap(
	  new NVBAxisPhysMap(
	    0,
	    header.value("Latmandelay").toDouble() / header.value("DSP_Clock").toDouble(),
	    NVBUnits("s")
	  )
	);

	double* zs = (double*)malloc(npts * sizeof(double));
	double* is = (double*)malloc(npts * sizeof(double));

	result->addDataSet("Z", zs, NVBUnits("nm"));
	result->addDataSet("I", is, NVBUnits("A"));

	// Positions

	QList< NVBPhysPoint > points;

	//-// DAC factors
	// Z
	double zfactor = header.value("Dacto[A]z").toDouble() * header.value("GainZ").toDouble();

	// Current (exp(-latmangain*ln(10))*10.0/imagedacrange)
	double ifactor;

	if (header.value("Dac-Type").toString() == "20bit")
		ifactor = pow(10, -header.value("Latmangain").toInt()) / 0x7FFF0; // Techn. should be 0x7FFFF, but STMAFM source says so
	else
		ifactor = pow(10, -header.value("Latmangain").toInt()) / 0x7FFF;

	// XY
	double xyfactor = header.value("Dacto[A]xy").toDouble() * header.value("GainX").toDouble();

	int line = 0;

	while (!file.atEnd() && line < npts) {
		QString thisline = file.readLine(800);
		QStringList pt_data = thisline.split(' ', QString::SkipEmptyParts);

		if (pt_data.count() < 5)
			NVBOutputError(QString("Line %1 too short : %2").arg(line).arg(thisline));
		else {
			zs[line] = pt_data.at(1).toDouble() * zfactor;
			is[line] = pt_data.at(2).toDouble() * ifactor;
			points << NVBPhysPoint(
			         pt_data.at(3).toDouble()*xyfactor,
			         pt_data.at(4).toDouble()*xyfactor,
			         NVBUnits("nm")
			       );
			}

		line += 1;
		}

	result->addAxisMap(new NVBAxisPointMap(points));

	sources->filterAddComments(comments);
	result->filterAddComments(comments);
	*sources << result;
	}

void CreatecFileGenerator::loadAllChannelsFromTSPEC(QString filename, NVBFile* sources) const {

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return;
		}

	QString format(file.readLine(20));
	format.chop(2);

	if (format != "[Parameter]") {
		NVBOutputError(QString("Don't know how to deal with format %1").arg(format));
		return;
		}

	NVBConstructableDataSource * result = new NVBConstructableDataSource(sources);

	if (!result) {
		NVBOutputError("NVBDataSource allocation failed");
		return;
		}

	CreatecHeader header = CreatecFileGenerator::getCreatecHeader(file);
	NVBDataComments comments = commentsFromHeader(header);
	file.seek(0x4002);
	int nparam = file.readLine(800).split('\t').count() - 3;
	file.seek(0x4002);

	int npts = header.value("FFTPoints").toInt();

	result->addAxis("Time", npts);
	result->addAxisMap(
	  new NVBAxisPhysMap(
	    0,
	    1 / header.value("SpecFreq").toDouble(),
	    NVBUnits("s")
	  )
	);

	QVector<double*> datae(nparam);

	for (int i = 0; i < nparam; i++)
		datae[i] = (double*)malloc(npts * sizeof(double));

	int freeaxis = 0;
	result->addDataSet("Z",    datae.at(freeaxis++), NVBUnits("nm"));
	result->addDataSet("I",    datae.at(freeaxis++), NVBUnits("A"));

	if (nparam > 4) {
		result->addDataSet("dI",   datae.at(freeaxis++), NVBUnits("V"));

		if (nparam > 6) {
			result->addDataSet("ADC2",  datae.at(freeaxis++), NVBUnits("DAC", false));
			result->addDataSet("ADC3",  datae.at(freeaxis++), NVBUnits("DAC", false));
			}
		}

	result->addDataSet("FFT Z",    datae.at(freeaxis++), NVBUnits("DAC", false));
	result->addDataSet("FFT I",    datae.at(freeaxis++), NVBUnits("DAC", false));

	if (nparam > 4) {
		result->addDataSet("FFT dI",   datae.at(freeaxis++), NVBUnits("DAC", false));

		if (nparam > 6) {
			result->addDataSet("FFT ADC2",  datae.at(freeaxis++), NVBUnits("DAC", false));
			result->addDataSet("FFT ADC3",  datae.at(freeaxis++), NVBUnits("DAC", false));
			}
		}

	//-// DAC factors

	QVector<double> factor(nparam, 1);

	//-// DAC factors
	freeaxis = 0;
	// Z
	factor[freeaxis++] = header.value("Dacto[A]z").toDouble() * header.value("GainZ").toDouble();
	// Current
	factor[freeaxis++] = header.value("Dacto[A]z").toDouble() *
	                     pow(10, -header.value("Gainpreamp").toInt()) /
	                     header.value("ZPiezoconst").toDouble();

	if (nparam > 4)
		// V
		factor[freeaxis++] = header.value("Dacto[A]z").toDouble() * 0.1 /
		                     header.value("ZPiezoconst").toDouble();

	int line = 0;

	while (!file.atEnd() && line < npts) {
		QByteArray thisline = file.readLine(800);
		QList<QByteArray> pt_data = thisline.split('\t');

		if (pt_data.count() < nparam / 2 + 3)
			NVBOutputError(QString("Line %1 too short : %2").arg(line).arg(QString(thisline)));

		for (int i = 0; i < nparam / 2; i++)
			datae.at(i)[line] = pt_data.at(i + 2).toDouble() * factor.at(i);

		for (int i = nparam / 2; i < nparam; i++)
			datae.at(i)[line] = pt_data.at(i + 3).toDouble() * factor.at(i);

		line += 1;
		}

	sources->filterAddComments(comments);
	result->filterAddComments(comments);
	*sources << result;
	}

NVB_EXPORT_FILEPLUGIN(createc,CreatecFileGenerator)
