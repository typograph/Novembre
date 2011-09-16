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
#include "NVBFileInfo.h"
#include "NVBFile.h"
#include "NVBLogger.h"
#include "NVBAxisMaps.h"
#include "NVBColorMaps.h"
#include <QtCore/QDir>
#include <QtCore/QRectF>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <malloc.h>

#define checkHeaderParam(x)	\
	if (!h.contains(x) || h.value(x).isEmpty()) return
#define ifHeaderParam(x)	\
	if (h.contains(x) && !h.value(x).isEmpty())


QPair<QString, NVBUnits> channelFromStr(QString value) {
	static QRegExp chName("^([^\\(]*)(?: \\(([^\\)]*)\\))?$");
	if (chName.exactMatch(value)) {
//		if (chName.capturedText().count());
		return QPair<QString, NVBUnits>(chName.cap(1),NVBUnits(chName.cap(2)));
		}
	else {
		NVBOutputError(QString("Channel name format mismatch at %1").arg(value));
		return QPair<QString, NVBUnits>(value,NVBUnits());
		}
}

NanonisHeader DATHeader(QFile& file) {
	/*
		Experiment	bias spectroscopy	
		Date	18.08.2009 13:54:49	
		User		
		X (m)	77.1667E-9	
		Y (m)	33.8731E-9	
		Z (m)	15.2509E-9	
		Z offset (m)	0E+0	
		Settling time (s)	30E-3	
		Integration time (s)	10E-3	
		Z-Ctrl hold	TRUE	
		Filter type	None	
		Order		
		Cutoff frq 		
	 */
	NanonisHeader h;
		
  QString tmp;

	while (true) {
		tmp = QString(file.readLine(100)).trimmed();
		if (tmp.isEmpty()) break;
		int midp = tmp.indexOf('\t');
		h.insert(tmp.left(midp),tmp.mid(midp+1));
    }

	return h;
	
}
NVBDataComments commentsFromDATHeader(NanonisHeader h) {
/*
		X (m)	77.1667E-9	
		Y (m)	33.8731E-9	
		Z (m)	15.2509E-9	
		Z offset (m)	0E+0	
*/
	NVBDataComments c;

	ifHeaderParam("Experiment")
		c.insert("Experiment name",h.value("Experiment"));
	ifHeaderParam("Date") {
		QDateTime dt = QDateTime::fromString(h.value("Date"),"MM.dd.yyyy hh:mm:ss");
		if (dt.isValid()) c.insert("Aquisition date & time",dt);
		else c.insert("Aquisition date & time",h.value("Date"));
		}
	ifHeaderParam("User")
		c.insert("User name",h.value("User"));
	ifHeaderParam("Settling time (s)") // TODO extract units
		c.insert("Settling time",NVBPhysValue(h.value("Settling time (s)").toDouble(),NVBUnits("s")));
	ifHeaderParam("Integration time (s)") // TODO extract units
		c.insert("Integration time",NVBPhysValue(h.value("Integration time (s)").toDouble(),NVBUnits("s")));
	ifHeaderParam("Z-Ctrl hold")
		c.insert("Z controller status",QString(h.value("Z-Ctrl hold") == "TRUE" ? "held" : "active"));
	ifHeaderParam("Filter type")
		c.insert("Filter type",h.value("Filter type"));
	ifHeaderParam("Order") // TODO find out what that is
		c.insert("Order",h.value("Order"));
	ifHeaderParam("Cutoff frq ")
		c.insert("Cutoff frequency",h.value("Cutoff frq "));
	
	return c;
}

NanonisHeader SXMHeader( QFile& file ) {
  NanonisHeader h;

	char nsb;

	QString key;
  QStringList value;
	
	file.seek(0);
	
	while (true) {
		// First, expect a keyword
		key = QString(file.readLine(500)).trimmed();
		if (key == ":SCANIT_END:") break;
		if (key.isEmpty()) continue;
		if (key.at(0) != ':') {
			NVBOutputError(QString("Header keyword recognition failed around \'%1\'").arg(key));
			continue; //doesnt seem a valid reason to fail.
			}
		
		// remove colons
		key.remove(0,1);
		key.chop(1);
		
		value.clear();
		while(file.peek(&nsb,1) && nsb != ':' && nsb != '\n')
			value << QString(file.readLine(500)).trimmed();
		
		h.insert(key,value.join("\n"));
    }
	
	return h;
}
NVBDataComments commentsFromSXMHeader(NanonisHeader h) {
	NVBDataComments c;
/* Nanonis header example
	:NANONIS_VERSION:
	2
	:SCANIT_TYPE:
								FLOAT            MSBFIRST
	:REC_DATE:
	14.08.2009
	:REC_TIME:
	18:32:42
	:REC_TEMP:
				290.0000000000
	:ACQ_TIME:
				274.8
	:SCAN_PIXELS:
				256       256
	:SCAN_FILE:
	C:\nanonis\au111270.sxm
	:SCAN_TIME:
							5.000E-1             5.000E-1
	:SCAN_RANGE:
						3.000E-7           3.000E-7
	:SCAN_OFFSET:
							-3.596E-10         -4.381E-8
	:SCAN_ANGLE:
							0.000E+0
	:SCAN_DIR:
	down
	:BIAS:
							5.000E-1
	:Z-CONTROLLER:
		Name	on	Setpoint	P-gain	I-gain
		log Current	1	1.000E-9 A	7.743E-12 m	5.462E-8 m/s
	:COMMENT:
	227 ohm co island
	:NanonisMain>Session Path:
	C:\nanonis
	:NanonisMain>SW Version:
	Generic 4
	:NanonisMain>UI Release:
	1759
	:NanonisMain>RT Release:
	1759
	:DATA_INFO:
		Channel	Name	Unit	Direction	Calibration	Offset
		14	Z	m	both	1.092E-8	0.000E+0
		0	Current	A	both	1.000E-8	3.692E-11

	:SCANIT_END:
 */
	
///	:REC_TEMP: 				290.0000000000
///	:SCAN_TIME:						5.000E-1             5.000E-1
///	:NanonisMain>Session Path:	C:\nanonis
///	:NanonisMain>SW Version:	Generic 4
///	:NanonisMain>UI Release:	1759
///	:NanonisMain>RT Release:	1759

	ifHeaderParam("REC_DATE")
		ifHeaderParam("REC_TIME") {
			QDateTime dt(QDate::fromString(h.value("REC_DATE"),"MM.dd.yyyy"),QTime::fromString(h.value("REC_TIME"),"hh:mm:ss"));
			if (dt.isValid()) c.insert("Aquisition date & time",dt);
			else c.insert("Aquisition date & time",h.value("REC_DATE") + " " + h.value("REC_TIME"));
			}

	ifHeaderParam("Z-CONTROLLER") {
		QStringList tmp = h.value("Z-CONTROLLER").split('\n',QString::SkipEmptyParts);
		QStringList zheaders = tmp.at(0).split('\t',QString::SkipEmptyParts);
		QStringList values = tmp.at(1).split('\t',QString::SkipEmptyParts);
		
		int ni = zheaders.indexOf("Name");
		int oi = zheaders.indexOf("on");
		for(int i = 0; i < zheaders.count(); i++)
			if (i == ni)
				c.insert("Z controller type",values.at(i));
			else if (i == oi)
				c.insert("Z controller status",QString(values.at(i) == "1" ? "active" : "held"));
			else
				c.insert(zheaders.at(i),NVBPhysValue(values.at(i)));
		}

	ifHeaderParam("SCAN_FILE")
		c.insert("Original path", h.value("SCAN_FILE"));
	ifHeaderParam("ACQ_TIME")
    c.insert("Acquisition time",NVBPhysValue(h.value("ACQ_TIME").toDouble(),NVBUnits("s")));
	ifHeaderParam("BIAS")
    c.insert("Bias",NVBPhysValue(h.value("BIAS").toDouble(),NVBUnits("V")));
  ifHeaderParam("COMMENT")
    c.insert("User comment",h.value("COMMENT"));
	
	return c;
}

NanonisHeader TDSHeader(QFile& file) {
	/*
Grid dim="10 x 10"
Grid settings=6.679884E-10;-4.247490E-9;5.000000E-10;5.000000E-10;0.000000E+0
Sweep Signal="Bias (V)"
Fixed parameters="Sweep Start;Sweep End"
Experiment parameters="X (m);Y (m);Z (m);Z offset (m);Settling time (s);Integration time (s);Z-Ctrl hold"
# Parameters (4 byte)=9
Experiment size (bytes)=1600
Points=100
Channels="Current (A);Input 6 (V);Current [bwd] (A);Input 6 [bwd] (V)"
Delay before measuring (s)=0.000000E+0
Experiment="Grid Spectroscopy"
Date="16.10.2009 02:45:41"
User=
Comment="sens 0.1mv, mod 1mV, 1 s, 16.88 kHz 156.4 deg, 10^7 10x10 428 ohm\0AGd"
:HEADER_END:
	*/
	NanonisHeader h;
	
  QString tmp;

	while (true) {
		tmp = QString(file.readLine(500)).trimmed();
		if (tmp == ":HEADER_END:") break;
		int midp = tmp.indexOf('=');
		if (midp < tmp.length()-1 && tmp.at(midp+1) == '"')
			h.insert(tmp.left(midp),tmp.mid(midp+2,tmp.length()-midp-3)); // 5688."999" L:10 mp:4 need 6,3
		else
			h.insert(tmp.left(midp),tmp.mid(midp+1));
    }
	
	return h;
}
NVBDataComments commentsFrom3DSHeader(NanonisHeader h) {
/*
Grid dim="10 x 10"
Grid settings=6.679884E-10;-4.247490E-9;5.000000E-10;5.000000E-10;0.000000E+0
Sweep Signal="Bias (V)"
Fixed parameters="Sweep Start;Sweep End"
Experiment parameters="X (m);Y (m);Z (m);Z offset (m);Settling time (s);Integration time (s);Z-Ctrl hold"
# Parameters (4 byte)=9
Experiment size (bytes)=1600
Points=100
Channels="Current (A);Input 6 (V);Current [bwd] (A);Input 6 [bwd] (V)"
Delay before measuring (s)=0.000000E+0
*/

	NVBDataComments c;

	ifHeaderParam("Experiment")
		c.insert("Experiment name",h.value("Experiment"));
	ifHeaderParam("Date") {
		QDateTime dt = QDateTime::fromString(h.value("Date"),"\"MM.dd.yyyy hh:mm:ss\"");
		if (dt.isValid()) c.insert("Aquisition date & time",dt);
		else c.insert("Aquisition date & time",h.value("Date"));
		}
	ifHeaderParam("User")
		c.insert("User name",h.value("User"));
	ifHeaderParam("Comment")
		c.insert("User comment",h.value("Comment"));

	// FIXME Nanonis allows arbitrary comments, but we don't
		
	return c;
}

QStringList NanonisFileGenerator::availableInfoFields() const {
		return QStringList()
			      << "Bias"
			      << "User comment"
			      << "Aquisition date & time"
			      << "Original path"
			      << "Experiment name"
			      << "User name"
			      << "Settling time"
			      << "Integration time"
			      << "Filter type"
			      << "Order"
			      << "Cutoff frequency"
			      << "Z controller type"
			      << "Z controller status"
			      << "Original path"
			      << "Acquisition time" // Rename to scan time ?
			      << "Setpoint"
			      << "P-gain"
			      << "I-gain"
			      ;
}

NVBAssociatedFilesInfo NanonisFileGenerator::associatedFiles(QString filename) const {
	return NVBAssociatedFilesInfo(QFileInfo(filename).fileName(), QStringList(filename), this);
}

NVBFileInfo * NanonisFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() != 1) {
		NVBOutputError("Wrong number of files");
		return 0;
		}

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	NVBFileInfo * fi = new NVBFileInfo(info);
	if (!fi) {
		NVBOutputError("NVBFileInfo allocation failure");
		return 0;
		}

	QString ffname = info.first();
	QString ext = ffname.right(ffname.size()-ffname.lastIndexOf('.')-1).toLower();
	if (ext == "dat") {
		NanonisHeader h = DATHeader(file);
		if (h.isEmpty()) {
			delete fi;
			return 0;
			}
			
		NVBDataComments c = commentsFromDATHeader(h);
		fi->filterAddComments(c);

		// FIXME [DATA] might be one line below...
		if (!QString(file.readLine(100)).contains("[DATA]",Qt::CaseInsensitive)) {
			NVBOutputError("Format mismatch, [DATA] not found");
			delete fi;
			return 0;
			}
		
		QList< QPair<QString, NVBUnits> > channels;
		QRegExp chName("^(.*)(?: \\((.*)\\))?$");
		
		foreach(QString channel, QString(file.readLine(500)).trimmed().split('\t',QString::SkipEmptyParts)) {
			if (chName.exactMatch(channel))
				channels << QPair<QString, NVBUnits>(chName.cap(1),NVBUnits(chName.cap(2)));
			else {
				NVBOutputError(QString("Channel name format mismatch at %1").arg(channel));
				channels << QPair<QString, NVBUnits>(channel,NVBUnits());
				} 
			}
			
		// Euristics that might not work - there's approx. 12 symbols per channel per line
		QVector<axissize_t> npts = QVector<axissize_t>() << (axissize_t)round((file.size() - file.pos())/12.0/channels.count());
		
		for(int i = 1; i < channels.count(); i++)
			fi->append(NVBDataInfo(channels.at(i).first,channels.at(i).second,npts,c,NVBDataSet::Spectroscopy));
		
		}
	else if (ext == "sxm") {
		NanonisHeader h = SXMHeader(file);
		if (h.isEmpty()) {
			delete fi;
			return 0;
			}
			
		NVBDataComments c = commentsFromSXMHeader(h);
		fi->filterAddComments(c);
		QVector<axissize_t> axes;
		ifHeaderParam("SCAN_PIXELS") {
			QStringList px = h.value("SCAN_PIXELS").split(' ',QString::SkipEmptyParts);
			axes << px.first().toInt() << px.last().toInt();
			}
		else {
			NVBOutputError(QString("No scan size data in file %1").arg(ffname));
			axes << 1 << 1;
			}

		QStringList l = h.value("DATA_INFO").split('\n');
		QStringList headers = l.takeFirst().split('\t',QString::SkipEmptyParts); // Headers
		int dirIndex = headers.indexOf("Direction");
		if (dirIndex < 0) {
			NVBOutputError("Direction information not found");
			delete fi;
			return 0;
			}
		while (l.last().isEmpty()) l.removeLast();
	
		bool direction;
		char count;
		
		// Now we know the number of pages
		foreach(QString s, l) {
			QStringList e = s.split('\t',QString::SkipEmptyParts);
			if (e.at(dirIndex) == "both") {
				direction = true;
				count = 2;
				}
			else if (e.at(dirIndex) == "left") {
				direction = true;
				count = 1;
				}
			else { // right
				direction = false;
				count = 1;
				}
				
			for (int i = 0; i < count; i++, direction = !direction)
				fi->append(NVBDataInfo(e.at(headers.indexOf("Name")) + (direction ? " (forward)" : " (backward)"),NVBUnits(e.at(headers.indexOf("Unit"))),axes,c,NVBDataSet::Topography));
			}
			
		}
	else if (ext == "3ds") {
		NanonisHeader h = TDSHeader(file);
		if (h.isEmpty()) {
			delete fi;
			return 0;
			}
			
		NVBDataComments c = commentsFrom3DSHeader(h);
		fi->filterAddComments(c);

//------------
		QString gd = h.value("Grid dim");
		int xp = gd.indexOf(" x ");
		if (xp <= 0) {
			NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
			delete fi;
			return 0;
			}
		bool ok;
		int nx = gd.left(xp).toInt(&ok);
		if (!ok) {
			NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
			delete fi;
			return 0;
			}
		int ny = gd.mid(xp+3,gd.length()-xp-3).toInt(&ok);
		if (!ok) {
			NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
			delete fi;
			return 0;
			}
	
		QPair<QString, NVBUnits> sweep;
		QList< QPair<QString, NVBUnits> > fixed_params;
		QList< QPair<QString, NVBUnits> > exp_params;

		sweep = channelFromStr(h.value("Sweep Signal"));
	
		foreach(QString channel, h.value("Fixed parameters").split(';',QString::SkipEmptyParts))
			fixed_params << channelFromStr(channel);

		{
		int iss = fixed_params.indexOf(QPair<QString, NVBUnits>("Sweep Start",NVBUnits()));	
		int ise = fixed_params.indexOf(QPair<QString, NVBUnits>("Sweep End",NVBUnits()));

		if (iss >= 0) fixed_params[iss].second = sweep.second;
		if (ise >= 0) fixed_params[ise].second = sweep.second;
		}
		
		foreach(QString channel, h.value("Experiment parameters").split(';',QString::SkipEmptyParts))
			exp_params << channelFromStr(channel);
		
		QList< QPair<QString, NVBUnits> > channels;

		foreach(QString channel, h.value("Channels").split(';',QString::SkipEmptyParts))
			channels << channelFromStr(channel);
	
		int nParam = h.value("# Parameters (4 byte)").toInt();
		
		if (fixed_params.count() + exp_params.count() != nParam) {
			NVBOutputError("Declared number of paramenters doesn't match the length of parameter lists");
			delete fi;
			return 0;
			}

		// Values of fixed parameters - read once
		float * fxParams = (float*)calloc(fixed_params.count(),sizeof(float));
		file.peek((char*)fxParams,4*fixed_params.count());
		reverseByteOrder<float>(fxParams,fixed_params.count());

		for(int j = 0; j < fixed_params.count(); j++)
//			if (exp_params.at(j).first != "X" && exp_params.at(j).first != "Y") // Skip useless X and Y
			fi->comments.insert(fixed_params.at(j).first,NVBPhysValue(fxParams[j],fixed_params.at(j).second));
		
		free(fxParams);

		int nPoints = h.value("Points").toInt();	
		
		QVector<axissize_t> tAxes, sAxes;
		tAxes << nx << ny; // Axes for topography pages
		sAxes << nPoints << nx << ny; // Axes for spectroscopy pages
			
		for(int j = 0; j < exp_params.count(); j++)
			if (exp_params.at(j).first != "X" && exp_params.at(j).first != "Y") // Skip useless X and Y
				fi->append(NVBDataInfo(exp_params.at(j).first,exp_params.at(j).second,tAxes,c,NVBDataSet::Topography));

		// Big question - TODO should we load main 3DS data as topography or as spectroscopy? 
		// It might be a user setting, but atm. I leave it at Undefined
		for(int j = 0; j < channels.count(); j++)
			fi->append(NVBDataInfo(channels.at(j).first,channels.at(j).second,sAxes,c));		
	}
	else {
		NVBOutputError(QString("Didn't recognise file format of %1").arg(ffname));
		delete fi;
		return 0;
		}

  return fi;
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

	if (info.count() > 1) {
		NVBOutputPMsg("Associated files include more that one file");
		return 0;
	}

	NVBFile * f = new NVBFile(info);
	if (!f) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}
 
 	QString ffname = info.first();
	QString ext = ffname.right(ffname.size()-ffname.lastIndexOf('.')-1).toLower();
	if (ext == "dat")
		loadChannelsFromDAT(ffname,f);
	else if (ext == "sxm")
		loadChannelsFromSXM(ffname,f);
	else if (ext == "3ds")
		loadChannelsFrom3DS(ffname,f);
	else {
		NVBOutputError(QString("Didn't recognise file format of %1").arg(ffname));
		}

	if (f->count() == 0) {
		NVBOutputError("No pages were loaded");
		f->release();
		delete f;
		return 0;
		}

	return f;
}

void NanonisFileGenerator::loadChannelsFromDAT(QString filename, NVBFile * sources) const
{
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return;
		}

	NanonisHeader h = DATHeader(file);
	NVBDataComments c = commentsFromDATHeader(h);
	
	// FIXME [DATA] might be one line below...
	if (!QString(file.readLine(100)).contains("[DATA]",Qt::CaseInsensitive)) {
		NVBOutputError("Format mismatch, [DATA] not found");
		return;
		}
	
	QList< QPair<QString, NVBUnits> > channels;
	QRegExp chName("^(.*) \\(([^\\)]*)\\)$");
	
	foreach(QString channel, QString(file.readLine(500)).trimmed().split('\t',QString::SkipEmptyParts))
		channels << channelFromStr(channel);

	QList< QVector<double> * > data;
	for(int i = channels.count(); i > 0; i--)
		data << new QVector<double>();
	
	while (!file.atEnd()) {
		QStringList datastr = QString(file.readLine(500)).trimmed().split('\t',QString::SkipEmptyParts);
		if (datastr.count() != channels.count()) {
			NVBOutputError("Wrong amount of data columns");
			continue;
			}
		foreach(QVector<double> * dd, data)
			dd->append(datastr.takeFirst().toDouble());
		}
	
	NVBConstructableDataSource * ds = new NVBConstructableDataSource(sources);
	sources->filterAddComments(c);
	ds->filterAddComments(c);

	// We will use the first column as a map
	if (channels.first().first.right(4) == "calc") { // Let's assume this is always a marker for equally-spaced data
		ds->addAxis(channels.first().first.left(channels.first().first.length()-5),data.first()->count());
		ds->addAxisMap(new NVBAxisPhysMap(data.first()->first(),(data.first()->last()-data.first()->first())/(data.first()->count()-1),channels.first().second));
		}
	else {
		ds->addAxis(channels.first().first,data.first()->count());
		ds->addAxisMap(new NVBAxisPhysMap(*data.first(),channels.first().second));
		}
	
	channels.removeFirst();
	delete data.takeFirst();
	
	while (!channels.isEmpty()) {
		QPair<QString, NVBUnits> channel = channels.takeFirst();
		QVector<double> * v = data.takeFirst();
		double * dt = (double*)malloc(sizeof(double)*v->count());
		memcpy(dt,v->constData(),sizeof(double)*v->count());
		ds->addDataSet(channel.first,dt,channel.second,NVBDataSet::Spectroscopy);
		delete v;
		}

	sources->append(ds);
}

void NanonisFileGenerator::loadChannelsFromSXM(QString filename, NVBFile* sources) const {
	
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return;
		}

	if (!QString(file.readLine(100)).contains("NANONIS",Qt::CaseInsensitive)) {
		NVBOutputError("Nanonis header missing");
		return;
		}

	NanonisHeader h = SXMHeader(file);
	
  checkHeaderParam("SCAN_PIXELS");
  checkHeaderParam("SCAN_RANGE");
  checkHeaderParam("SCAN_OFFSET");
	
	// Go to the data
	
//	file.read(4); // 0x0a 0x0a 0x1a 0x04 -- Whatever that means
	while(!file.atEnd() && (file.read(1).at(0) != 0x1A || file.peek(1).at(0) != 0x04)) {;}
	if (file.atEnd()) {
		NVBOutputError("No data start marker found");
		return;
		}
	file.read(1);

	NVBConstructableDataSource * ds = new NVBConstructableDataSource(sources);

  QStringList s_size = h.value("SCAN_PIXELS").split(' ',QString::SkipEmptyParts);
  QStringList s_range = h.value("SCAN_RANGE").split(' ',QString::SkipEmptyParts);
  QStringList s_offset = h.value("SCAN_OFFSET").split(' ',QString::SkipEmptyParts);

	QSize resolution(s_size.at(0).toInt(), s_size.at(1).toInt());
	QSizeF range(s_range.at(0).toDouble(), s_range.at(1).toDouble());
	QPointF offset(s_offset.at(0).toDouble(), s_offset.at(1).toDouble());
	
	ds->addAxis("X",resolution.width());
	ds->addAxisMap(new NVBAxisPhysMap(offset.x()-range.width()/2, range.width()/resolution.width(), NVBUnits("m")));
	ds->addAxis("Y",resolution.height());
	ds->addAxisMap(new NVBAxisPhysMap(-offset.y()-range.height()/2, range.height()/resolution.height(), NVBUnits("m")));
	
  QStringList l = h.value("DATA_INFO").split('\n');
  QStringList headers = l.takeFirst().split('\t'); // Headers
	int dirIndex = headers.indexOf("Direction");
	int data_points = resolution.width() * resolution.height();
  while (l.last().isEmpty()) l.removeLast();

	int data_size = data_points*sizeof(float);
	float * tdata = (float*)calloc(sizeof(float),2*data_points);
	
	bool direction;
	char count;
	
  // Now we know the number of pages
  foreach(QString s, l) {
    QStringList e = s.split('\t');
		NVBOutputDMsg(e.at(dirIndex));
		if (e.at(dirIndex) == "both") {
			direction = true;
			count = 2;
			}
		else if (e.at(dirIndex) == "left") {
			direction = true;
			count = 1;
			}
		else { // right
			direction = false;
			count = 1;
			}
			
		for (int i = 0; i < count; i++, direction = !direction) {
			if (file.read((char*)tdata,data_size) < data_size) {
				NVBOutputError(QString("File %1 ended before the data could be fully read").arg(filename));
				free(tdata);
				delete ds;
				return;
				}
			reverseByteOrder<float>(tdata,data_points);
			flipMem<float>(tdata+data_points,tdata,resolution.width(),resolution.height(),direction,h.value("SCAN_DIR") != "down");

			double * data = (double*)calloc(sizeof(double),data_points);
			scaleMem<float,double>(data,tdata+data_points,data_points);
			ds->addDataSet(e.at(headers.indexOf("Name")) + (direction ? " (forward)" : " (backward)"),data,NVBUnits(e.at(headers.indexOf("Unit"))),NVBDataSet::Topography);
			}
    }

	free(tdata);

  file.close();

	NVBDataComments comments = commentsFromSXMHeader(h);
	
	sources->filterAddComments(comments);
	ds->filterAddComments(comments); // They are the sane for all datasets anyway
	sources->append(ds);
	
}


void NanonisFileGenerator::loadChannelsFrom3DS(QString filename, NVBFile* sources) const {
// Have to find format description.
// Looks like 
// [parameters] - 4x9 bytes
// [data] - 4x4x100 bytes (where does 100 come from ???)

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return;
		}

	NanonisHeader h = TDSHeader(file);
	
	checkHeaderParam("Grid dim");
	checkHeaderParam("Grid settings");
	checkHeaderParam("Sweep Signal");
	checkHeaderParam("Fixed parameters");
	checkHeaderParam("Experiment parameters");
//	checkHeaderParam("");

	QString gd = h.value("Grid dim");
	int xp = gd.indexOf(" x ");
	if (xp <= 0) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return;
		}
	bool ok;
	int nx = gd.left(xp).toInt(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return;		
		}
	int ny = gd.mid(xp+3,gd.length()-xp-3).toInt(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return;		
		}
	
	QStringList gparams = h.value("Grid settings").split(';');
	double gcx = gparams.at(0).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(0)));
		return;		
		}
	double gcy = gparams.at(1).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(1)));
		return;		
		}
	double gw = gparams.at(2).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(2)));
		return;		
		}
	double gh = gparams.at(3).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(3)));
		return;		
		}
	double ga = gparams.at(4).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(4)));
		return;		
		}

	if (ga != 0) { // FIXME
		NVBOutputPMsg("Angles not implemented");
		}

	QPair<QString, NVBUnits> sweep;
	QList< QPair<QString, NVBUnits> > fixed_params;
	QList< QPair<QString, NVBUnits> > exp_params;

	sweep = channelFromStr(h.value("Sweep Signal"));
	
	foreach(QString channel, h.value("Fixed parameters").split(';',QString::SkipEmptyParts))
		fixed_params << channelFromStr(channel);

	foreach(QString channel, h.value("Experiment parameters").split(';',QString::SkipEmptyParts))
		exp_params << channelFromStr(channel);
	
	QList< QPair<QString, NVBUnits> > channels;

	foreach(QString channel, h.value("Channels").split(';',QString::SkipEmptyParts))
		channels << channelFromStr(channel);
	
	int nParam = h.value("# Parameters (4 byte)").toInt();
	
	if (fixed_params.count() + exp_params.count() != nParam) {
		NVBOutputError("Declared number of paramenters doesn't match the length of parameter lists");
		return;
		}

	int iSweepStart = fixed_params.indexOf(QPair<QString, NVBUnits>("Sweep Start",NVBUnits()));
	int iSweepEnd = fixed_params.indexOf(QPair<QString, NVBUnits>("Sweep End",NVBUnits()));
	if (iSweepStart < 0 || iSweepEnd < 0) {
		NVBOutputError("Sweep limits not in fixed parameters");
		return;
		}

	Q_ASSERT(sizeof(float) == 4);

	// Values of fixed parameters - read once
	float * fxParams = (float*)calloc(fixed_params.count(),sizeof(float));
	file.peek((char*)fxParams,4*fixed_params.count());
	reverseByteOrder<float>(fxParams,fixed_params.count());
		
	// Buffer to read parameters
	float * paramBuf = (float*)calloc(nParam,sizeof(float));

	// Data arrays for parameter datasets
	QVector<double *> paramData(exp_params.count());
	for(int i = 0; i<exp_params.count(); i++)
		paramData[i] = (double*)calloc(nx*ny,sizeof(double));

	int nPoints = h.value("Points").toInt();
	
	// Data arrays for channels
	float * channelBuf = (float*)calloc(nPoints,sizeof(float));
	QVector<double *> channelData(channels.count());
	for(int i = 0; i<channels.count(); i++)
		channelData[i] = (double*)calloc(nx*ny*nPoints,sizeof(double));
	
	// Load data - note: file end doesn't stop the process - if some data could be read, it should be delivered
	for(int i = 0; i<nx*ny && !file.atEnd(); i++) {
		if (file.read((char*)paramBuf,nParam*sizeof(float)) != nParam*sizeof(float))
			NVBOutputError("File ended before data could be read");
		reverseByteOrder<float>(paramBuf,nParam);
		
		for(int j = 0; j < exp_params.count(); j++)
			paramData[j][i] = paramBuf[j+fixed_params.count()];
		
		foreach(double * dptr, channelData) {
			if (file.read((char*)channelBuf,nPoints*sizeof(float)) != nPoints*sizeof(float))
				NVBOutputError("File ended before data could be read");
			reverseByteOrder<float>(channelBuf,nPoints);
			scaleMem<float,double>(dptr+i*nPoints,channelBuf,nPoints);
			}
		
		}
		
	free(paramBuf);
	free(channelBuf);
	
	NVBConstructableDataSource * ds = new NVBConstructableDataSource(sources);
	ds->addAxis(sweep.first,nPoints);
	ds->addAxisMap(
		new NVBAxisPhysMap(
			fxParams[iSweepStart],
			(fxParams[iSweepEnd]-fxParams[iSweepStart]) / (nPoints-1),
			sweep.second
			)
		);
	
	// It is not quite clear to me why, but the grid is shifted half a pixel to larger values
	// as can be seen from X and Y param data
	// TODO Look at maps with different sizes, esp with odd sizes to find out.
	
	ds->addAxis("X",nx);
	ds->addAxisMap(new NVBAxisPhysMap(gcx - gw*(1-1.0/nx)/2, gw/nx, NVBUnits("m")));
	ds->addAxis("Y",ny);
	ds->addAxisMap(new NVBAxisPhysMap(gcy - gh*(1-1.0/ny)/2, gh/ny, NVBUnits("m")));
	
	QVector<axisindex_t> tAxes, sAxes;
	tAxes << 1 << 2; // Axes for topography pages
	sAxes << 0 << 1 << 2; // Axes for spectroscopy pages
	
	NVBDataComments c = commentsFrom3DSHeader(h);
	sources->filterAddComments(c);
	ds->filterAddComments(c);
	
	for(int j = 0; j < exp_params.count(); j++)
		if (exp_params.at(j).first != "X" && exp_params.at(j).first != "Y") // Skip useless X and Y
			ds->addDataSet(exp_params.at(j).first,paramData[j],exp_params.at(j).second,c,tAxes,NVBDataSet::Topography);

	// Big question - TODO should we load main 3DS data as topography or as spectroscopy? 
	// It might be a user setting, but atm. I leave it at Undefined
	for(int j = 0; j < channels.count(); j++)
		ds->addDataSet(channels.at(j).first,channelData[j],channels.at(j).second,c,sAxes);
	
	sources->append(ds);
	free(fxParams);
	return;
}

Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
