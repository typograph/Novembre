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

class NVBExpandableSpecPage : public NVBSpecPage {
	public:
		NVBExpandableSpecPage(QString name, QString taxis): NVBSpecPage() {
			_datasize = QSize(0, 0);
			pagename = name.left(name.lastIndexOf(' '));
			zd = NVBDimension(name.mid(name.lastIndexOf('(') + 1, name.lastIndexOf(')') - name.lastIndexOf('(') - 1));
			xd = NVBDimension("m");
			yd = NVBDimension("m");
			td = NVBDimension(taxis.mid(taxis.lastIndexOf('(') + 1, taxis.lastIndexOf(')') - taxis.lastIndexOf('(') - 1));
			setComment("X axis label", taxis.left(taxis.lastIndexOf(' ')));
			setComment("Y axis label", pagename);
			setColorModel(new NVBRandomDiscrColorModel());
			}

		void addNewSpecPoint(double x, double y, QwtData * pdata) {
			if (! _datasize.isNull() && (int)pdata->size() != _datasize.width()) {
				NVBOutputError(QString("Wrong size of added data: %1 when main size is %2").arg(pdata->size()).arg(_datasize.width()));
				return;
				};

			emit dataAboutToBeChanged();

			_positions << QPointF(x, -y);

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
	       << "Line time" \
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

NVBFile * NanonisFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
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

	if (info.first().right(3) == "3ds")
		return load3DS(info);

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
		return loadSpecAggregationInfo(info);

	if (info.first().right(3) == "3ds")
		return load3DSInfo(info);

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

NVBFile * NanonisFileGenerator::loadSpecAggregation(const NVBAssociatedFilesInfo & info) const {
	QList<NVBExpandableSpecPage*> pages;

	QFile f(info.first());

	if (! f.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&f);
		return 0;
		}

	QTextStream first(&f);

	while (first.readLine() != "[DATA]")
		if (first.atEnd()) {
			NVBOutputError("File format error: no DATA section");
			return 0;
			}

	QStringList names = first.readLine().split('\t', QString::SkipEmptyParts);

	for (int i = 1; i < names.count(); i++)
		pages << new NVBExpandableSpecPage(names.at(i), names.first());

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
			while (specdata.readLine() != "[DATA]")
				if (specdata.atEnd()) {
					NVBOutputError("File format error: no DATA section");
					return 0;
					}

			QString clmns = specdata.readLine(); // Column names
			QList<QVector<double> > cdata;

			for (int i = -1; i < pages.count(); i++)
				cdata << QVector<double>();

			while (!specdata.atEnd()) {
				QStringList vdata = specdata.readLine().split('\t', QString::SkipEmptyParts);

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

			for (int i = 0; i < pages.count(); i++)
				pages[i]->addNewSpecPoint(xpos, ypos, new QwtArrayData(cdata.at(0), cdata.at(i + 1)));

			// TODO Do something with z;
			}
		}

	NVBFile * result = new NVBFile(info);

	foreach(NVBExpandableSpecPage * p, pages)
	result->addSource(p);

	return result;
	}

NVBFileInfo * NanonisFileGenerator::loadSpecAggregationInfo(const NVBAssociatedFilesInfo & info) const {
	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) {
		NVBOutputError("NVBFileInfo creation failed");
		return 0;
		}

	QFile f(info.first());

	if (!(f.open(QIODevice::ReadOnly))) {
		NVBOutputFileError(&f);
		return 0;
		}

	QTextStream first(&f);

	while (first.readLine() != "[DATA]")
		if (first.atEnd()) {
			NVBOutputError("File format error: no DATA section");
			return 0;
			}

	QStringList names = first.readLine().split('\t', QString::SkipEmptyParts);
	int nxs = 0;

	while (!first.atEnd()) {
		first.readLine();
		nxs += 1;
		}

	for (int i = 1; i < names.count(); i++)
		fi->pages.append(NVBPageInfo(names.at(i), NVB::SpecPage, QSize(nxs, info.count()), NVBPhysValue(nxs, NVBDimension()), NVBPhysValue(), QMap<QString, NVBVariant>()));

	f.close();

	return fi;

	}

// What follows is backported from 0.1

#define checkHeaderParam(x)	\
	if (!h.contains(x) || h.value(x).isEmpty()) return 0
#define ifHeaderParam(x)	\
	if (h.contains(x) && !h.value(x).isEmpty())

QPair<QString, NVBDimension> channelFromStr(QString value) {
	static QRegExp chNameCompiled("([^\\(]*)(?:\\s\\((.*)\\))?");
	
	// QRegExp is not thread-safe, but I don't want to compile it every time
	QRegExp chName(chNameCompiled);
	
	if (chName.exactMatch(value)) {
//		if (chName.capturedText().count());
		return QPair<QString, NVBDimension>(chName.cap(1), NVBDimension(chName.cap(2)));
		}
	else {
		NVBOutputError(QString("Channel name format mismatch at %1").arg(value));
		return QPair<QString, NVBDimension>(value, NVBDimension());
		}
	}

void reverse_float_byte_order(char * ptr) {
	asm(
	  "bswap %%eax"
	  : "=a"(*((quint32*)ptr))
	  : "0"(*((quint32*)ptr))
	);
	}

void reverseFloatByteOrder(float * ptr, unsigned int count) {
	if (!ptr) {
		NVBOutputError("NULL pointer");
		return;
		}

	for (unsigned int i = 0; i < count; i++, ptr++)
		reverse_float_byte_order((char*)ptr);
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

		if (midp < tmp.length() - 1 && tmp.at(midp + 1) == '"')
			h.insert(tmp.left(midp), QStringList(tmp.mid(midp + 2, tmp.length() - midp - 3))); // 5688."999" L:10 mp:4 need 6,3
		else
			h.insert(tmp.left(midp), QStringList(tmp.mid(midp + 1)));
		}

	return h;
	}

class NanonisPage_topoC : public NVB3DPage {
	public:
		NanonisPage_topoC(QString name, double * _data, QSize size, QRectF pos, double angle,
		                  NVBDimension x, NVBDimension y, NVBDimension z,
		                  QMap<QString, NVBVariant> cs)
			: NVB3DPage() {

			data = _data;
			_resolution = size;
			_position = pos;
			_angle = angle;
			xd = x;
			yd = y;
			zd = z;
			pagename = name;
			getMinMax();
			setColorModel(new NVBGrayRampContColorModel(0, 1, zMin, zMax));
			foreach(QString key, cs)
				setComment(key, cs[key]);
			}
	};

class NanonisPage_specC : public NVBSpecPage {
		double *xdata, *zdata;
	public:
		NanonisPage_specC(QString name, double * _xdata, double * _zdata, QSize size, QList<QPointF> poss,
		                  NVBDimension t, NVBDimension z, QMap<QString, NVBVariant> cs)
			: NVBSpecPage() {
			pagename = name;
			foreach(QString key, cs)
			comments.insert(key, cs[key]);
			_positions = poss;
			_datasize = size;
			zd = z;
			td = t;
			xdata = _xdata;
			zdata = _zdata;
			xd = NVBDimension("m");
			yd = NVBDimension("m");

			for (int i = 0; i < size.height(); i++) {
				_data.append(new QwtCPointerData(xdata, zdata + i * size.width(), size.width()));
				}

			setColorModel(new NVBConstDiscrColorModel(Qt::black));
			}
		~NanonisPage_specC() { free(xdata); free(zdata); }
	};

NVBFile * NanonisFileGenerator::load3DS(const NVBAssociatedFilesInfo & info) const {
	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	NanonisHeader h = TDSHeader(file);

	checkHeaderParam("Grid dim");
	checkHeaderParam("Grid settings");
	checkHeaderParam("Sweep Signal");
	checkHeaderParam("Fixed parameters");
// 	checkHeaderParam("Experiment parameters");
//	checkHeaderParam("");

	QString gd = h.value("Grid dim").first();
	int xp = gd.indexOf(" x ");

	if (xp <= 0) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	bool ok;
	int nx = gd.left(xp).toInt(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	int ny = gd.mid(xp + 3, gd.length() - xp - 3).toInt(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	QStringList gparams = h.value("Grid settings").first().split(';');

	double gcx = gparams.at(0).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(0)));
		return 0;
		}

	double gcy = gparams.at(1).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(1)));
		return 0;
		}
		
	double gw = gparams.at(2).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(2)));
		return 0;
		}

	double gh = gparams.at(3).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(3)));
		return 0;
		}

	double ga = gparams.at(4).toDouble(&ok);
	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(4)));
		return 0;
		}

	QPair<QString, NVBDimension> sweep;
	QList< QPair<QString, NVBDimension> > fixed_params;
	QList< QPair<QString, NVBDimension> > exp_params;

	sweep = channelFromStr(h.value("Sweep Signal").first());

	foreach(QString channel, h.value("Fixed parameters").first().split(';', QString::SkipEmptyParts))
		fixed_params << channelFromStr(channel);

	foreach(QString channel, h.value("Experiment parameters").first().split(';', QString::SkipEmptyParts))
		exp_params << channelFromStr(channel);

	QList< QPair<QString, NVBDimension> > channels;

	foreach(QString channel, h.value("Channels").first().split(';', QString::SkipEmptyParts))
		channels << channelFromStr(channel);

	int nParam = h.value("# Parameters (4 byte)").first().toInt();

	if (fixed_params.count() + exp_params.count() != nParam) {
		NVBOutputError("Declared number of paramenters doesn't match the length of parameter lists");
		return 0;
		}

	int iSweepStart = fixed_params.indexOf(QPair<QString, NVBDimension>("Sweep Start", NVBDimension()));
	int iSweepEnd = fixed_params.indexOf(QPair<QString, NVBDimension>("Sweep End", NVBDimension()));

	if (iSweepStart < 0 || iSweepEnd < 0) {
		NVBOutputError("Sweep limits not in fixed parameters");
		return 0;
		}

	Q_ASSERT(sizeof(float) == 4);

	// Values of fixed parameters - read once
	float * fxParams = (float*)calloc(fixed_params.count(), sizeof(float));
	file.peek((char*)fxParams, 4 * fixed_params.count());
	reverseFloatByteOrder(fxParams, fixed_params.count());

	// Buffer to read parameters
	float * paramBuf = (float*)calloc(nParam, sizeof(float));

	// Data arrays for parameter datasets
	QVector<double *> paramData(exp_params.count());

	for (int i = 0; i < exp_params.count(); i++)
		paramData[i] = (double*)calloc(nx * ny, sizeof(double));

	int nPoints = h.value("Points").first().toInt();

	// Data arrays for channels
	float * channelBuf = (float*)calloc(nPoints, sizeof(float));
	QVector<double *> channelData(channels.count());

	for (int i = 0; i < channels.count(); i++)
		channelData[i] = (double*)calloc(nx * ny * nPoints, sizeof(double));

	// Load data - note: file end doesn't stop the process - if some data could be read, it should be delivered
	for (int i = 0; i < nx * ny && !file.atEnd(); i++) {
		if (file.read((char*)paramBuf, nParam * sizeof(float)) != nParam * sizeof(float))
			NVBOutputError("File ended before data could be read");

		reverseFloatByteOrder(paramBuf, nParam);

		for (int j = 0; j < exp_params.count(); j++)
			paramData[j][i] = paramBuf[j + fixed_params.count()];

		scaler<float, double> sscaler(0, 1);

		foreach(double * dptr, channelData) {
			if (file.read((char*)channelBuf, nPoints * sizeof(float)) != nPoints * sizeof(float))
				NVBOutputError("File ended before data could be read");

			reverseFloatByteOrder(channelBuf, nPoints);
			scaleMem<float, double>(dptr + i * nPoints, sscaler, channelBuf, nPoints);
			}

		}

	free(paramBuf);
	free(channelBuf);

	double * sweep_data = (double*)calloc(nPoints, sizeof(double));
	double factor = (fxParams[iSweepEnd] - fxParams[iSweepStart]) / (nPoints - 1);

	for (int i = 0; i < nPoints; i++)
		sweep_data[i] = fxParams[iSweepStart] + factor * i;

	QMap<QString, NVBVariant> comments;

	ifHeaderParam("Experiment")
	comments.insert("Experiment name", h.value("Experiment"));
	ifHeaderParam("Date") {
		QDateTime dt = QDateTime::fromString(h.value("Date").first(), "\"MM.dd.yyyy hh:mm:ss\"");

		if (dt.isValid()) comments.insert("Aquisition date & time", dt);
		else comments.insert("Aquisition date & time", h.value("Date").first());
		}
	ifHeaderParam("User")
	comments.insert("User name", h.value("User"));
	ifHeaderParam("Comment")
	comments.insert("User comment", h.value("Comment"));

	NVBFile * f = new NVBFile(info);

	if (!f) {
		NVBOutputError("File object creation failed");
		return 0;
		}

	QSize tSize(nx, ny);

	for (int j = 0; j < exp_params.count(); j++)
		if (exp_params.at(j).first != "X" && exp_params.at(j).first != "Y") // Skip useless X and Y
			f->addSource(new NanonisPage_topoC(exp_params.at(j).first, paramData[j], tSize, QRectF(gcx-gw/2, -gcy-gh/2, gw, gh), ga, NVBDimension("m"), NVBDimension("m"), exp_params.at(j).second, comments));


	QSize sSize(nPoints, nx * ny);
	QList<QPointF> specPos;
	int chX = exp_params.indexOf(QPair<QString, NVBDimension>("X", NVBDimension("m")));
	int chY = exp_params.indexOf(QPair<QString, NVBDimension>("Y", NVBDimension("m")));

	if (chX >= 0 && chY >=0) // Use data from the table
		for (int i = 0; i < nx * ny; i++)
			specPos << QPointF(paramData[chX][i], -paramData[chY][i]);
	else {
		ga *= 3.1415/180;
		for (int i = 0; i < ny; i++)
			for (int j = 0; j < nx; j++) // FIXME is nx really the fast axis?
				specPos << QPointF(gcx - ((nx-1)/2 - j)*gw/nx*cos(ga) - ((ny-1)/2 - i)*gh/ny*sin(ga),
				                  -gcy - ((nx-1)/2 - j)*gw/nx*sin(ga) + ((ny-1)/2 - i)*gh/ny*cos(ga));
		}

	for (int j = 0; j < channels.count(); j++)
		f->addSource(new NanonisPage_specC(channels.at(j).first, sweep_data, channelData[j], sSize, specPos, sweep.second, channels.at(j).second, comments));

	free(fxParams);
	return f;
	}

NVBFileInfo * NanonisFileGenerator::load3DSInfo(const NVBAssociatedFilesInfo & info) const {

	QFile file(info.first());

	if (!file.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&file);
		return 0;
		}

	NanonisHeader h = TDSHeader(file);

	if (h.isEmpty()) {
		return 0;
		}

//------------
	QString gd = h.value("Grid dim").first();
	int xp = gd.indexOf(" x ");

	if (xp <= 0) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	bool ok;
	int nx = gd.left(xp).toInt(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	int ny = gd.mid(xp + 3, gd.length() - xp - 3).toInt(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid dimension format mismatch %1").arg(gd));
		return 0;
		}

	QStringList gparams = h.value("Grid settings").first().split(';');
	double gw = gparams.at(2).toDouble(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(2)));
		return 0;
		}

	double gh = gparams.at(3).toDouble(&ok);

	if (!ok) {
		NVBOutputError(QString("Grid settings format mismatch %1").arg(gparams.at(3)));
		return 0;
		}

	QPair<QString, NVBDimension> sweep;
	QList< QPair<QString, NVBDimension> > fixed_params;
	QList< QPair<QString, NVBDimension> > exp_params;

	sweep = channelFromStr(h.value("Sweep Signal").first());

	foreach(QString channel, h.value("Fixed parameters").first().split(';', QString::SkipEmptyParts))
	fixed_params << channelFromStr(channel);

	foreach(QString channel, h.value("Experiment parameters").first().split(';', QString::SkipEmptyParts))
	exp_params << channelFromStr(channel);

	QList< QPair<QString, NVBDimension> > channels;

	foreach(QString channel, h.value("Channels").first().split(';', QString::SkipEmptyParts))
	channels << channelFromStr(channel);

	int nParam = h.value("# Parameters (4 byte)").first().toInt();

	if (fixed_params.count() + exp_params.count() != nParam) {
		NVBOutputError("Declared number of paramenters doesn't match the length of parameter lists");
		return 0;
		}

	// Values of fixed parameters - read once
	float * fxParams = (float*)calloc(fixed_params.count(), sizeof(float));
	file.peek((char*)fxParams, 4 * fixed_params.count());
	reverseFloatByteOrder(fxParams, fixed_params.count());

	QMap<QString, NVBVariant> comments;
	ifHeaderParam("Experiment")
	comments.insert("Experiment name", h.value("Experiment").first());
	ifHeaderParam("Date") {
		QDateTime dt = QDateTime::fromString(h.value("Date").first(), "\"MM.dd.yyyy hh:mm:ss\"");

		if (dt.isValid()) comments.insert("Aquisition date & time", dt);
		else comments.insert("Aquisition date & time", h.value("Date").first());
		}
	ifHeaderParam("User")
	comments.insert("User name", h.value("User").first());
	ifHeaderParam("Comment")
	comments.insert("User comment", h.value("Comment").first());

	for (int j = 0; j < fixed_params.count(); j++)
		comments.insert(fixed_params.at(j).first, NVBPhysValue(fxParams[j], fixed_params.at(j).second));

	int nPoints = h.value("Points").first().toInt();

	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) {
		NVBOutputError("NVBFileInfo allocation failure");
		return 0;
		}

	//
	ifHeaderParam("Sweep Signal") {
		int iSweepStart = fixed_params.indexOf(QPair<QString, NVBDimension>("Sweep Start", NVBDimension()));
		int iSweepEnd = fixed_params.indexOf(QPair<QString, NVBDimension>("Sweep End", NVBDimension()));

		if (iSweepStart < 0 || iSweepEnd < 0) {
			NVBOutputError("Sweep limits not in fixed parameters");
			delete fi;
			free(fxParams);
			return 0;
			}

		QPair<QString, NVBDimension>	sweep = channelFromStr(h.value("Sweep Signal").first());
		QSize sSize(nPoints, nx * ny);

		// Add spectroscopy pages
		for (int j = 0; j < channels.count(); j++)
			fi->pages.append(NVBPageInfo(channels.at(j).first, NVB::SpecPage, sSize, NVBPhysValue(fxParams[iSweepEnd] - fxParams[iSweepStart], sweep.second), NVBPhysValue(nx * ny, NVBDimension()), comments));

		}

	// Reminder to self:
	// (QString _name, NVB::PageType _type, QSize size, NVBPhysValue xspan, NVBPhysValue yspan,
	//		QMap<QString,NVBVariant> _comments)

	QSize tSize(nx, ny);

	// Add topography pages
	for (int j = 0; j < exp_params.count(); j++)
		if (exp_params.at(j).first != "X" && exp_params.at(j).first != "Y") // Skip useless X and Y
			fi->pages.append(NVBPageInfo(exp_params.at(j).first, NVB::TopoPage, tSize, NVBPhysValue(gw, NVBDimension("m")), NVBPhysValue(gh, NVBDimension("m")), comments));

	free(fxParams);

	return fi;

	}

Q_EXPORT_PLUGIN2(nanonis, NanonisFileGenerator)
