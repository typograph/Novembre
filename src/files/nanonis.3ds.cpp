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
//#include "../core/dimension.h"
#include "../core/NVBDiscrColoring.h"
#include "../core/NVBContColoring.h"
#include "../core/NVBFileInfo.h"
#include "../core/NVBFile.h"
//#include <QDebug>

// What follows is backported from 0.1

#define headerHasParam(x)    \
    (h.contains(x) && !h.value(x).isEmpty())
#define checkHeaderParam(x)	\
	if (!headerHasParam(x)) return 0

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
		tmp = QString(file.readLine(256)); // This is a reasonable length
		while (!tmp.endsWith("\r\n"))
			tmp.append(file.readLine(256));
		tmp.chop(2);

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

	if (headerHasParam("Experiment"))
        comments.insert("Experiment name", h.value("Experiment"));
	if (headerHasParam("Date")) {
		QDateTime dt = QDateTime::fromString(h.value("Date").first(), "\"MM.dd.yyyy hh:mm:ss\"");

		if (dt.isValid()) comments.insert("Aquisition date & time", dt);
		else comments.insert("Aquisition date & time", h.value("Date").first());
		}
	if (headerHasParam("User"))
        comments.insert("User name", h.value("User"));
	if (headerHasParam("Comment"))
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
	if (headerHasParam("Experiment"))
        comments.insert("Experiment name", h.value("Experiment").first());
	if (headerHasParam("Date")) {
		QDateTime dt = QDateTime::fromString(h.value("Date").first(), "\"MM.dd.yyyy hh:mm:ss\"");

		if (dt.isValid()) comments.insert("Aquisition date & time", dt);
		else comments.insert("Aquisition date & time", h.value("Date").first());
		}
	if (headerHasParam("User"))
        comments.insert("User name", h.value("User").first());
	if (headerHasParam("Comment"))
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
	if (headerHasParam("Sweep Signal")) {
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
