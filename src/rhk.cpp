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

#include "NVBFileInfo.h"
#include "NVBFile.h"
#include "NVBLogger.h"
#include "NVBAxisMaps.h"
#include "NVBColorMaps.h"
#include "NVBAxisSelector.h"
#include "rhk.h"
#include <stdlib.h>

#ifndef FILEGENERATOR_NO_GUI
#include "NVBSettingsWidget.h"
#endif

#include <QtCore/QDateTime>
#include "NVBSettings.h"
#include <QtGui/QCheckBox>

#define RHK_TOPOPAGE 0
#define RHK_SPECPAGE 1
#define RHK_UNKPAGE 2
#define RHK_ANNOTATEDSPECPAGE 3

#ifndef FILEGENERATOR_NO_GUI
class RHKSettingsWidget : public NVBSettingsWidget {
public:
	explicit RHKSettingsWidget(RHKFileGenerator * g, QWidget* parent = 0) : NVBSettingsWidget(parent) {
		setGroup("RHK");
		addCheckBox("subtractBias","Subtract bias voltage","In case 'Bias mode' was not selected in XPMPro, I(U) spectroscopy voltage is shifted by the applied bias. This setting will apply to any spectroscopic data with X axis in volts.");
		}
		
private:
	void onWrite() {
		// TODO : think about what happens with threads... (this is probably atomic, so it's fine)
		g->subtractBias = entries.first().checkBox->isChecked();
		}
};

NVBSettingsWidget* RHKFileGenerator::configurationPage() const
{
	static NVBSettingsWidget * w = new RHKSettingsWidget();
	return w;
}
#endif

RHKFileGenerator::RHKFileGenerator() :NVBFileGenerator()
{
	// Load subtractBias from settings;
	QSettings * conf = NVBSettings::getGlobalSettings();
	if (!conf)
		subtractBias = false;
	else {
		conf->beginGroup(NVBSettings::pluginGroup());
		subtractBias = conf->value("RHK/subtractBias",false);
		conf->endGroup();
		}
}

/*

void TRHKFile::load( FILE * rf) {
	const char MAGIC_SM2[] = {
	0x9E, 0x00, 0x53, 0x00, 0x54, 0x00, 0x69, 0x00, 0x4d, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00, 0x30, 0x00, 0x34, 0x00,
	0x2E, 0x00, 0x30, 0x00, 0x30, 0x00, 0x31, 0x00, 0x20, 0x00, 0x31, 0x00
	};
	
	const char MAGIC_SM3[] = {
	0xAA, 0x00, 0x53, 0x00, 0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00, 0x30, 0x00, 0x34, 0x00,
	0x2E, 0x00, 0x30, 0x00, 0x30, 0x00, 0x32, 0x00, 0x20, 0x00, 0x31, 0x00
	};
	
	const char MAGIC_SIZE = sizeof(MAGIC_SM3);

	char test[44];
	qint64 size_of_file;
	
}
*/


QStringList RHKFileGenerator::availableInfoFields() const {
	return QStringList()
		<< "System note"
		<< "Session comment"
		<< "User comment"
		<< "Original path"
		<< "Aquisition date & time"
//		<< "X axis label"
//		<< "Y axis label"
		<< "Status channel"
		<< "Source type"
		<< "Image type"
		<< "Group ID"
		<< "Time per point"
		<< "Bias"
		<< "Setpoint"
		<< "GUID"
//		<< "Page type"
		<< "Scan direction"
		<< "Line type"
		;
}

void RHKFileGenerator::detectGrid(const TRHKHeader& header, const float * xposdata, const float * yposdata, int& np, int& nx, int& ny)
{
	nx = 1;
	ny = 1;
	np = 1;

// Basically, we can go on on two things
// First, some of the grid variants are encoded in header.page_type
// Second, header version 2 has grid_xsize and grid_ysize (which are, unfortunately, usually zero)
// Third, XPMPro measures curves one-by-one, first by point, then by X, then by Y, so the number of
// repeated positions in X gives point multiplicity and the number of repetitions in Y gives point times X.

	switch(header.page_type) {
		case 11 : { // Image IV 4x4
			nx = 4;
			ny = 4;
			break;
			}
		case 12 : { // Image IV 8x8
			nx = 8;
			ny = 8;
			break;
			}
		case 13 : { // Image IV 16x16
			nx = 16;
			ny = 16;
			break;
			}
		case 14 : { // Image IV 32x32
			nx = 32;
			ny = 32;
			break;
			}
		case 15 : { // Image IV center
			nx = 1;
			ny = 1;
			break;
			}
		case 23 : { // Image IV 64x64
			nx = 64;
			ny = 64;
			break;
			}
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 19:
		case 20:
			NVBOutputError(QString("Weird page type for spectroscopy : %1").arg(RHKFileGenerator::getPageTypeString(header.page_type)));
			break;
		}

	if (nx == 1) {
		if (header.grid_xsize > 0) {
			nx = header.grid_xsize;
			ny = header.grid_ysize;
			}
		else {
			for(np = 1; np < header.y_size && xposdata[np] == xposdata[0]; np++) {;}
			for(nx = 1; nx*np < header.y_size && yposdata[nx*np] == yposdata[0]; nx++) {;}
			for(ny = 1; ny*nx*np < header.y_size && xposdata[ny*nx*np] == xposdata[0]; ny++) {;}
			}
		}
		
}

NVBFile * RHKFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw()
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

	NVBFile * f = new NVBFile(info);

	if (!f) {
		NVBOutputError("Memory allocation for NVBFile failed.");
		return 0;
		}

// how to do it?
// 
// The logical way would be to load datasets one-by-one.
// While the axes are the same, we keep the same datasource.

	while(!file.atEnd() && loadNextPage(file,f));

	if (!f->isEmpty())
	return f;
	else {
		delete f;
		return 0;
		}
}

bool RHKFileGenerator::RHKHeaderIsSane(const TRHKHeader & header, QString filename) {
	// Number of strings is positive and at least 10 (at most 18, as of last specification)
	if (header.string_count < 10 || header.string_count > 30) {
		NVBOutputError(QString("Page header has unexpected number of strings. File %1 is probably corrupted.").arg(filename));
		return false;
		}

	// Reasonable size
	if (header.x_size < 0 || header.y_size < 0 || header.page_data_size < header.x_size || header.page_data_size < header.y_size || header.page_data_size != abs(4*header.x_size*header.y_size)) {
		NVBOutputError(QString("Page has unexpected dimentions. File %1 is probably corrupted.").arg(filename));
		return false;
		}

	return true;
}

NVBFileInfo * RHKFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw()
{
	const char MAGIC[] = {  0x53, 0x00,
	0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00,
	0x30, 0x00, 0x34, 0x00, 0x2E, 0x00, 0x30, 0x00,
	0x30, 0x00, 0x32, 0x00, 0x20, 0x00, 0x31, 0x00,
	0x00, 0x00}; // STImage 004.002 1

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

	if (file.atEnd()) {
		NVBOutputError("Empty file");
		return 0;
		}

	NVBFileInfo * fi = 0;
	fi = new NVBFileInfo(info);
	if (!fi) {
		NVBOutputError("NVBFileInfo creation failed");
		return 0;
		}

	TRHKHeader header;

	// Read pages 1by1
	while(!file.atEnd()) {

		file.peek((char*)&header,44);
		if (memcmp(header.version, MAGIC, 28) != 0) {
			NVBOutputError(QString("New page does not have recognizable RHK format. A shift must have been introduced due to incorect format implementation. Please, send the file %1 to Timofey").arg(file.fileName()));
			delete fi;
			return NULL;
		}

		header = getRHKHeader(file);

		// Sanity checks

		if (!RHKHeaderIsSane(header,file.fileName())) {
			if (fi->count() > 0)
				return fi;
			delete fi;
			return 0;
			}

		QStringList strings = loadRHKStrings(file,header.string_count);
		if (strings.isEmpty()) {
			if (fi->count() > 0)
				return fi;
			delete fi;
			return 0;
			}

		NVBDataComments comments;
		RHKFileGenerator::CommentsFromString(comments,strings);
		RHKFileGenerator::CommentsFromHeader(comments,header);
	
		file.seek(file.pos() + header.page_data_size); // New position directly after data

		NVBDataSet::Type type = NVBDataSet::Undefined;
		QList<NVBAxisInfo> axes;
		
		switch (header.type) {
			case 0 : { // Topography - color_info starts here - skip it
				if (header.colorinfo_count > 1)
					NVBOutputError(QString("Multiple coloring detected. Please, send a copy of %1 to the developer").arg(file.fileName()));
				quint16 cs;
				file.peek((char*)&cs,2);
				file.seek(file.pos() + header.colorinfo_count*(cs+2));
				type = NVBDataSet::Topography;
				axes << NVBAxisInfo(strings.at(10).isEmpty() ? "X" : strings.at(10),header.x_size,NVBUnits(strings.at(7)))
				     << NVBAxisInfo(strings.at(11).isEmpty() ? "Y" : strings.at(11),header.x_size,NVBUnits(strings.at(8)));
				break;
				}
			case 1 : { // Spectroscopy
				// Silently ignore non-zero colorinfo_count -- all RHK spec pages I encountered have colorinfo_count == 1
//				if (header.colorinfo_count != 0) 
//					NVBOutputError(QString("Coloring specified for a spectroscopy page. The file %1 might be corrupted. If not, please, send a copy of %1 to the developer").arg(file.fileName()));

				QString nameT = strings.at(10);
				if (nameT.isEmpty() || nameT == "X") { // TODO it might be interesting to move this method to NVBUnits
					NVBUnits tu = NVBUnits(strings.at(7));
					if (tu.isComparableWith("V"))
						nameT = "Voltage";
					else if (tu.isComparableWith("sec"))
						nameT = "Time";
					else if (tu.isComparableWith("A"))
						nameT = "Current";
					else
						nameT = "T";
					}
				axes << NVBAxisInfo(nameT,header.x_size,NVBUnits(strings.at(7)));

				int np=1,nx=1,ny=1;
				
				// Skip curve position data
				if (header.page_type != 7 && header.page_type != 31) {
					float * posdata = (float*)malloc(2*sizeof(float)*header.y_size);
					file.read((char*)posdata,2*sizeof(float)*header.y_size);
					detectGrid(header,posdata,posdata + header.y_size,np,nx,ny);
					}

				if (np > 1) axes << NVBAxisInfo("Samples",np);
				if (nx > 1) axes << NVBAxisInfo("X",nx,NVBUnits(strings.at(7)));
				if (ny > 1) axes << NVBAxisInfo("Y",ny,NVBUnits(strings.at(8)));;
				if (nx < 2 && ny < 2 && (header.y_size / np) > 1)
						axes << NVBAxisInfo("Points", header.y_size / np, NVBUnits("Point",false));
				
				type = NVBDataSet::Spectroscopy;
				break;
				}
			case 3 : { // Annotated spectroscopy, whatever that means
				NVBOutputError(QString("Annotated spectroscopy page found. No information on such a page exists. Please send the file %1 to the developer").arg(file.fileName()));
//       type = NVB::SpecPage;
				// Skip positions
				file.seek(file.pos() + 2*sizeof(float)*header.y_size);
				}
			case 2 : // RHK lists this case as RESERVED
			default : {
				NVBOutputError(QString("Non-existing page found (type %1). Your file might be corrupted. If not, please send the file %2 to the developer").arg(header.type).arg(file.fileName()));
//        type = NVB::InvalidPage;
				}
			}
			
		if (type != NVBDataSet::Undefined) {
			fi->filterAddComments(comments);
			fi->append(NVBDataInfo(strings.at(0).trimmed(),NVBUnits(strings.at(9)),axes,comments,type));
			}
		}

	return fi;
}

bool RHKFileGenerator::loadNextPage(QFile& file, NVBFile * sources)
{
	const char MAGIC[] = {  0x53, 0x00,
	0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00,
	0x30, 0x00, 0x34, 0x00, 0x2E, 0x00, 0x30, 0x00,
	0x30, 0x00, 0x32, 0x00, 0x20, 0x00, 0x31, 0x00,
	0x00, 0x00}; // STImage 004.002 1

// This is just one version of the header, very specific one,
// including the "Unicode" part (that I have no idea about).
// Technically, it should be simple to extend this code to
// other versions FIXME
// At the moment, the comparison goes only up to byte 27,
// which is just before the '2'

	TRHKHeader h;
	file.peek((char*)&h,44);

	if (memcmp(h.version, MAGIC, 28) != 0) {
		NVBOutputError(QString("Page does not have recognizable RHK format. A shift must have been introduced due to incorect format implementation. Please, send the file %1 to the developer").arg(file.fileName()));
		return false;
		}
	switch (h.type) {
		case 0 : {
			NVBOutputVPMsg("Topography page found");
			return loadTopoPage(file,sources);
			}
		case 1 : {
			NVBOutputVPMsg("Spectroscopy page found");
			return loadSpecPage(file,sources);
			}
		case 3 : {
			NVBOutputError(QString("Annotated spectroscopy page found. No information on such a page exists. Please send the file %1 to the developer").arg(file.fileName()));

			TRHKHeader header = RHKFileGenerator::getRHKHeader(file);
			if (!RHKHeaderIsSane(header,file.fileName()))
				return false;
			QStringList strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
			if (strings.isEmpty())
				return false;
			file.seek(file.pos() + header.page_data_size);
			file.seek(file.pos() + 2*sizeof(float)*header.y_size);
			return true;
			}
		case 2 :
		default : {
			NVBOutputError(QString("Non-existing page found (type %1). Your file might be corrupted. If not, please send the file %2 to the developer").arg(h.type).arg(file.fileName()));
			return false;
			}
		}
}

bool RHKFileGenerator::loadTopoPage(QFile& file, NVBFile * sources)
{
	TRHKHeader header = RHKFileGenerator::getRHKHeader(file);
	if (!RHKHeaderIsSane(header,file.fileName()))
		return false;

	NVBDataComments comments;
	
	int version = header.version[14]-0x30; // 0,1,..9 [version in 004.00V]
	if (version > 2) {
		NVBOutputError(QString("Page header has a higher version number (%1) than supported by this plugin. Please, inform the developper about this.").arg(version));
		return false;
		}

	QStringList strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
	if (strings.isEmpty())
		return false;
	RHKFileGenerator::CommentsFromString(comments,strings);
	RHKFileGenerator::CommentsFromHeader(comments,header);
	sources->filterAddComments(comments);

// Before we start doing anything else, we have to make sure if we want a new datasource

	NVBAxisSelector s;
	s.addAxisByLength(header.x_size).byUnits(NVBUnits(strings.at(7)));
	s.addAxisByLength(header.y_size).byUnits(NVBUnits(strings.at(8)));

	NVBSelectorDataInstance inst = s.instantiateOneDataset(sources);
	NVBConstructableDataSource * ds;
	QVector<axisindex_t> ia(2);
	
	if(!inst.isValid()) {
		NVBOutputDMsg("No suitable existing datasource found");
		ds = new NVBConstructableDataSource(sources);
		sources->append(ds);
		
		// Add X axis
		ia[0] = 0;
		ds->addAxis(strings.at(10).isEmpty() ? "X" : strings.at(10),header.x_size);
		ds->addAxisMap(new NVBAxisPhysMap(header.x_offset,header.x_scale,NVBUnits(strings.at(7))));

		// Add Y axis
		ia[1] = 1;
		ds->addAxis(strings.at(11).isEmpty() ? "Y" : strings.at(11),header.y_size);
		ds->addAxisMap(new NVBAxisPhysMap(header.y_offset,header.y_scale,NVBUnits(strings.at(8))));

	// TODO Angle in topography -> we have to check first if the X&Y are rotated or not, and if not, create a new ds
	//	ds->addAxisMap(new NVBAxes2DGridMap(NVBPhysPoint(header.x_offset,header.y_offset,NVBUnits(strings.at(7))),QTransform().rotate(header.angle)),ia);
		}
	else {
		ds = qobject_cast<NVBConstructableDataSource*>(inst.matchingData()->dataSource());
		ia = inst.matchedAxes();
		NVBOutputDMsg(QString("Found a datasource with matching axes at %1 and %2").arg(ia.at(0)).arg(ia.at(1)));
		if (ia.at(0) == -1) {
			// Add X axis
			ia[0] = ds->nAxes();
			ds->addAxis(strings.at(10).isEmpty() ? "X" : strings.at(10),header.x_size);
			ds->addAxisMap(new NVBAxisPhysMap(header.x_offset,header.x_scale,NVBUnits(strings.at(7))));
			}
		if (ia.at(1) == -1) {
			ia[1] = ds->nAxes();
			ds->addAxis(strings.at(11).isEmpty() ? "Y" : strings.at(11),header.y_size);
			ds->addAxisMap(new NVBAxisPhysMap(header.y_offset,header.y_scale,NVBUnits(strings.at(8))));
			}
		}

//  comments.insert("Page type",RHKFileGenerator::getPageTypeString(header.page_type));

	qint32 * dataRHK = (qint32*)malloc(header.page_data_size);
	double * data;
	
	if (file.read((char*)dataRHK,header.page_data_size) < header.page_data_size) {
		NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
		free(dataRHK);
		return false;
		}
	else {
		double * tdata = (double*)calloc(sizeof(double),header.x_size*header.y_size);

		NVBValueScaler<qint32,double> intscaler(header.z_offset,header.z_scale);
		scaleMem<qint32,double>(tdata,intscaler,dataRHK,header.x_size*header.y_size);
		free(dataRHK);

		data = (double*)calloc(sizeof(double),header.x_size*header.y_size);
		// RHK y axis is inverted with respect to Qt's y axis
		flipMem<double>(data, tdata, header.x_size, header.y_size, !(header.x_scale > 0), header.y_scale > 0 );
		free(tdata);
		}

	ds->filterAddComments(comments);
	NVBDataSet * dset = ds->addDataSet(strings.at(0).trimmed(),data,NVBUnits(strings.at(9)),comments,ia,NVBDataSet::Topography);

	if (header.colorinfo_count == 1) {
		TRHKColorInfo cInfo;
		if (file.read((char*)&(cInfo.parameter_count),2) != 2) { // read header size
			NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
			return false;
			}
		if (cInfo.parameter_count+2 <= (int)sizeof(TRHKColorInfo)) {
			if (file.read((char*)&(cInfo.start_h),cInfo.parameter_count) != cInfo.parameter_count) {
				NVBOutputError(QString("File ended abruptly. File %1 probably corrupted.").arg(file.fileName()));
				return false;
				}
			}
		else {
			NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
			if (file.read((char*)&(cInfo.start_h),(int)sizeof(TRHKColorInfo)) != (int)sizeof(TRHKColorInfo)) {
				NVBOutputError(QString("File ended abruptly. File %1 probably corrupted.").arg(file.fileName()));
				return false;
				}
			file.seek(file.pos() + cInfo.parameter_count+2 - sizeof(TRHKColorInfo));
			}

		if (data) {
			dset->setColorMap(new NVBHSVWheelColorMap(cInfo.start_h/360, cInfo.end_h/360, cInfo.start_s, cInfo.end_s, cInfo.start_b, cInfo.end_b));
			}
		}
	else {
		NVBOutputError(QString("Multiple coloring detected. Please, send a copy of %1 to Timofey").arg(file.fileName()));
// FIXME  Multiple coloring :  NVBSetContColorModel * m = new NVBSetContColorModel();
		for (int i = 0; i < header.colorinfo_count; i++) {
			qint16 len;
			if (file.read((char*)&len,2) != 2 || len < 2) {
				NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
				return false; // read past end offile
				}
			file.seek(file.pos() + len);
			}
		}

	return true;
}

bool RHKFileGenerator::loadSpecPage(QFile & file, NVBFile * sources)
{
	
	TRHKHeader header = RHKFileGenerator::getRHKHeader(file);
	if (!RHKHeaderIsSane(header,file.fileName()))
		return false;

	NVBDataComments comments;

	int version = header.version[14]-0x30; // 0,1,..9 [version in 004.00V]
	if (version > 2) {
		NVBOutputError(QString("Page header has a higher version number (%1) than supported by this plugin. Please, inform the developper about this.").arg(version));
		return false;
		}

	QStringList strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
	if (strings.isEmpty())
		return false;
	RHKFileGenerator::CommentsFromString(comments,strings);
	RHKFileGenerator::CommentsFromHeader(comments,header);
	sources->filterAddComments(comments);

// We have to deduce the shape of the data
	
// To get to the points we have to load the data first

	double * ys;
	switch (header.line_type) { // Some of pages have float type data, and some have the DAC values
		case 19: // Gdatalog
		case 21: // Gechem
		case 13: // Givnorm
		case  5: // Gfft_power # CORRECT (this is the actual value in the files)
		case  6: // Gfft_power # INCORRECT (this is the value listed in the manual)
		case 11: // Gautocorr
		case 18: // Goutspec
			{
			float * tdata = (float*)malloc(header.page_data_size);
			if (file.read((char*)tdata,header.page_data_size) < header.page_data_size) {
				NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
				free(tdata);
				return false;
				}

			NVBValueScaler<float,double> floatscaler(0,1); //### Suppose there's no scaling
			ys = floatscaler.scaleMem(tdata,header.x_size*header.y_size);

			free(tdata);
			break;
			}
		default : {
			qint32 * tdata = (qint32*)malloc(header.page_data_size);
			if (file.read((char*)tdata,header.page_data_size) < header.page_data_size) {
				NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
				free(tdata);
				return false;
				}

			ys = NVBValueScaler<qint32,double>(header.z_offset,header.z_scale).scaleMem(tdata,header.x_size*header.y_size);

			free(tdata);
			break;
			}
		}
		
	if (header.x_scale < 0)
		flipMem<double>(ys,header.x_size,header.y_size,true,false);

	// Load position data

	float * posdata = (float*)calloc(sizeof(float),2*header.y_size);
	float * xposdata = posdata;
	float * yposdata = posdata + header.y_size;
	if ( header.page_type != 7 && header.page_type != 31 && file.read((char*)posdata,2*sizeof(float)*header.y_size) < (qint64)(2*sizeof(float)*header.y_size)) {
		NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
		}
//      _positions.append(QPointF(xposdata[i],-yposdata[i]));
//     _data.append(new QwtCPointerData(xs,ys+i*header.x_size,header.x_size));
	
	// OK, let's find out
	int nx=1, ny=1, np=1;
	if (header.page_type != 7 && header.page_type != 31) {
		detectGrid(header,xposdata,yposdata,np,nx,ny);
		}

	// Now, the axes
	// If np > 1 -> "samples" axis
	// If nx > 1 -> X and Y axes

	// I need an intelligent approach!!!
	// The easiest way would be to use NVBAxisSelector with a datasource.
		
	NVBAxisSelector s;
	unsigned char status = 0; // Bit 1 - np > 1 , Bit 2 - there's a grid.
	
	// FIXME can X or Y have length 1? If yes, this should be taken into account by _not_ looking for these axes
	
	QVector<axisindex_t> ia(1);

	s.addAxisByLength(header.x_size).byUnits(NVBUnits(strings.at(7)));
	if (np > 1) {
		s.addAxisByLength(np);
		status |= 1;
		ia.resize(ia.size()+1);
		}
	if (nx > 1) {
		s.addAxisByLength(nx);
		s.addAxisByLength(ny);
		status |= 2;
		ia.resize(ia.size()+2);
		}
	else { // Instead of a grid we have some points
		s.addAxisByLength(header.y_size / np);
		ia.resize(ia.size()+1);
		}
	
	NVBSelectorDataInstance inst = s.instantiateOneDataset(sources);
	NVBConstructableDataSource * ds = 0;
	
	if (!inst.isValid()) { // We have to create the axes
		ds = new NVBConstructableDataSource(sources);
		sources->append(ds);
		// For some reason, this axis gets name "X" by default, which is unfortunate,
		// as it clashes with grid and anyway is not very descriptive
		QString nameT = strings.at(10);
		if (nameT.isEmpty() || nameT == "X") { // TODO it might be interesting to move this method to NVBUnits
			NVBUnits tu = NVBUnits(strings.at(7));
			if (tu.isComparableWith("V"))
				nameT = "Voltage";
			else if (tu.isComparableWith("sec"))
				nameT = "Time";
			else if (tu.isComparableWith("A"))
				nameT = "Current";
			else
				nameT = "T";
			}
		ds->addAxis(nameT,header.x_size);
		if (subtractBias && tu.isComparableWith("V"))
			ds->addAxisMap(new NVBAxisPhysMap(header.x_offset + header.bias,header.x_scale,NVBUnits(strings.at(7))));
		else
			ds->addAxisMap(new NVBAxisPhysMap(header.x_offset,header.x_scale,NVBUnits(strings.at(7))));
		
		if (status & 1)
			ds->addAxis("Samples",np);
		if (status & 2) {
			ds->addAxis("X",nx);
			ds->addAxisMap(new NVBAxisPhysMap(xposdata[0],xposdata[np] - xposdata[0],NVBUnits(strings.at(7))));
			ds->addAxis("Y",ny);
			ds->addAxisMap(new NVBAxisPhysMap(yposdata[0],yposdata[nx*np] - yposdata[0],NVBUnits(strings.at(8))));
			}
		else {
			ds->addAxis("Points", header.y_size / np);
			ds->addAxisMap(new NVBAxisPointMap(pointsFromXY(header.y_size/np,xposdata,yposdata),NVBUnits("m")));
			}
		for(int i=0;i<ia.size();i+=1)
			ia[i] = i;
		}
	else {
		ds = qobject_cast< NVBConstructableDataSource* >( inst.matchingData()->dataSource() );
		ia = inst.matchedAxes();
		if (ia.at(0) == -1) {
			ds->addAxis(strings.at(10).isEmpty() ? "t" : strings.at(10),header.x_size);
			ds->addAxisMap(new NVBAxisPhysMap(header.x_offset,header.x_scale,NVBUnits(strings.at(7))));
			}
		if (status & 1 && ia.at(1) == -1) { // No samples axis matched
			ia[0] = ds->nAxes();
			ds->addAxis("Samples",np);
			}
		if (status & 2) { 
			if (ia.at(1 + (status & 1)) == -1) { // No X axis match
				ia[status & 1] = ds->nAxes();
				ds->addAxis("X",nx);
				ds->addAxisMap(new NVBAxisPhysMap(xposdata[0],xposdata[np] - xposdata[0],NVBUnits(strings.at(7))));
				}
			if (ia.at(2 + (status & 1)) == -1) { // No Y axis match
				ia[1 + (status & 1)] = ds->nAxes();
				ds->addAxis("Y",ny);
				ds->addAxisMap(new NVBAxisPhysMap(yposdata[0],yposdata[nx*np] - yposdata[0],NVBUnits(strings.at(8))));
				}
			}
		else if (ia.at(1 + (status & 1)) == -1) {
			ds->addAxis("Points", header.y_size / np);
			ds->addAxisMap(new NVBAxisPointMap(pointsFromXY(header.y_size/np,xposdata,yposdata),NVBUnits("m")));
			}
	}

	// TODO Is there angle in spectroscopy. If so, we should pre-rotate X and Y positions.
		
	// TODO Find out how to get this dimension
//  xd = NVBUnits("m");
//  yd = NVBUnits("m");
//  td = NVBUnits(strings.at(7));
//  zd = NVBUnits(strings.at(9));

	ds->filterAddComments(comments);
	ds->addDataSet(strings.at(0).trimmed(),ys,NVBUnits(strings.at(9)),comments,ia,NVBDataSet::Spectroscopy);
	
/* // Silently ignore this -- all RHK spec pages I encountered have colorinfo_count == 1
	if (header.colorinfo_count != 0) 
		NVBOutputError(QString("Coloring specified for a spectroscopy page. The file %1 might be corrupted. If not, please, send a copy of %1 to Timofey").arg(file.fileName()));
*/

	return true;
}

QString RHKFileGenerator::getGUIDString(RHK_GUID id) {
	return QString("%1-%2-%3-%4").arg(id.Data1,0,16).arg(id.Data2,0,16).arg(id.Data3,0,16).arg(id.Data4,0,16);
}

QString RHKFileGenerator::getLineTypeString(qint32 type) {
	switch(type) {
		case  0: return "Not a line";
		case  1: return "Histogram";
		case  2: return "Cross section";
		case  3: return "Line test";
		case  4: return "Oscilloscope";
		case  5: return "Unknown";
		case  6: return "Noise power spectrum";
		case  7: return "I-V spectrum";
		case  8: return "I-Z spectrum";
		case  9: return "Image X Average";
		case 10: return "Image Y Average";
		case 11: return "Noise autocorrelation spectrum";
		case 12: return "Multichannel analyzer data";
		case 13: return "Renormalized IV data from variable gap IV";
		case 14: return "Image histogram spectra";
		case 15: return "Image cross section";
		case 16: return "Image average";
		case 17: return "Image cross section"; // What the hell?
		case 18: return "Image out spectra";
		case 19: return "Datalog spectrum";
		case 20: return "Gxy";
		case 21: return "Electro chemistry";
		case 22: return "Discrete spectroscopy";
		default : {
			NVBOutputError(QString("Unknown line type %1").arg(type));
			return QString();
			}
		}
}

QString RHKFileGenerator::getSourceTypeString(qint32 type) {
	switch(type) {
		case 0 : return "Raw page";
		case 1 : return "Processed page";
		case 2 : return "Calculated page";
		case 3 : return "Imported page";
		default : {
			NVBOutputError(QString("Unknown source type %1").arg(type));
			return QString();
			}
		}
}

QString RHKFileGenerator::getDirectionString(qint32 type) {
	switch(type) {
		case 0 : return "Right";
		case 1 : return "Left";
		case 2 : return "Up";
		case 3 : return "Down";
		default : {
			NVBOutputError(QString("Unknown scan direction %1").arg(type));
			return QString();
			}
		}
}

QString RHKFileGenerator::getImageTypeString(qint32 type) {
	switch(type) {
		case 0 : return "Normal image";
		case 1 : return "Autocorrelation image";
		default : {
			NVBOutputError(QString("Unknown image type %1").arg(type));
			return QString();
			}
		}
}

QString RHKFileGenerator::getPageTypeString(qint32 type) {
	switch(type) {
		case  0 : return "Undefined";
		case  1 : return "Topographic image";
		case  2 : return "Current image";
		case  3 : return "Aux image";
		case  4 : return "Force image";
		case  5 : return "Signal image";
		case  6 : return "Image FFT transform";
		case  7 : return "Noise power spectrum";
		case  8 : return "Line test";
		case  9 : return "Oscilloscope";
		case 10 : return "IV spectra";
		case 11 : return "Image IV 4x4";
		case 12 : return "Image IV 8x8";
		case 13 : return "Image IV 16x16";
		case 14 : return "Image IV 32x32";
		case 15 : return "Image IV center";
		case 16 : return "Image interactive spectra";
		case 17 : return "Autocorrelation page";
		case 18 : return "IZ spectra";
		case 19 : return "4 gain topography";
		case 20 : return "8 gain topography";
		case 21 : return "4 gain current";
		case 22 : return "8 gain current";
		case 23 : return "Image IV 64x64";
		case 24 : return "Autocorrelation spectrum";
		case 25 : return "Counter data";
		case 26 : return "Multichannel analyzer data";
		case 27 : return "AFM data using RHK AFM-100";
		case 28 : return "CITS";
		case 29 : return "GPIB";
		case 30 : return "Video channel";
		case 31 : return "Image out spectra";
		case 32 : return "Image data log";
		case 33 : return "Image ECSet";
		case 34 : return "Image ECData";
		case 35 : return "Image DSP AD";
		case 36 : return "Discrete spectroscopy";
		case 37 : return "Image + Discrete spectroscopy";
		case 38 : return "Ramp spectroscopy at relative points";
		case 39 : return "Discrete spectroscopy at relative points";
		default : {
			NVBOutputError(QString("Invalid page type %1 found").arg(type));
			return QString();
			}
		}
}

TRHKHeader RHKFileGenerator::getRHKHeader(QFile & file)
{
	TRHKHeader header;
	file.read((char*)&header.parameter_size,2); // read header size
	file.read((char*)header.version,header.parameter_size);
	if (header.parameter_size+2 > (int)sizeof(TRHKHeader))
		file.seek(file.pos() + header.parameter_size+2-sizeof(TRHKHeader));
	else
		memset(((char*)header.version)+header.parameter_size,0,sizeof(TRHKHeader) - header.parameter_size - 2);
	int version = header.version[14]-0x30;
	if (version == 1) {
		header.colorinfo_count = 1;
		header.grid_ysize = 0;
		header.grid_xsize = 0;
		}
	return header;
}

QStringList RHKFileGenerator::loadRHKStrings(QFile & file, qint16 nstrings)
{
	QStringList r;
	quint32 * s;
	qint16 slen;
	for(int i = 0; i<nstrings; i++) {
		if (file.read((char*)&slen,2) != 2) {
			NVBOutputFileError(&file);
			return QStringList();
			}
		// Check string length. Minimum is 0
		// I cannot check the maximum, since it's unlimited
		// And going over the file edge will be caugth.
		if (slen < 0) {
			NVBOutputError(QString("Negative string length. File %1 is probably corrupted.").arg(file.fileName()));
			return QStringList();
			}
		s = (quint32*)calloc(4,slen+1);
		for (int j = 0; j < slen; j++)
			if (file.read((char*)(s+j),2) != 2) {
				NVBOutputFileError(&file);
				return QStringList();
				}
		r << QString::fromUcs4(s,slen);
		free(s);
		}

// Check X and Y labels

	if (r.at(10).isEmpty())
		r[10] = "X";
	if (r.at(11).isEmpty())
		r[11] = "Y";

return r;
}

void RHKFileGenerator::CommentsFromString(NVBDataComments& comments, const QStringList& strings) {
	comments.insert("System note",strings.at(1));
	comments.insert("Session comment",strings.at(2));
	comments.insert("User comment",strings.at(3));
	comments.insert("Original path",strings.at(4));

	QDateTime dt(QDate::fromString(strings.at(5),"MM/dd/yy"),QTime::fromString(strings.at(6),"hh:mm:ss"));
	if (dt.isValid()) comments.insert("Aquisition date & time",dt);
	else comments.insert("Aquisition date & time",strings.at(5) + " " + strings.at(6));

	if (strings.size() >= 13) comments.insert("Status channel",strings.at(12));

	for (int i = 13; i<strings.size(); i++) {
		comments.insert(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
		}
}


void RHKFileGenerator::CommentsFromHeader(NVBDataComments& comments, const TRHKHeader& header)
{
		comments.insert("Source type",getSourceTypeString(header.source_type));
		comments.insert("Image type",getImageTypeString(header.image_type));
		comments.insert("Group ID",QString::number(header.group_ID));
		switch (header.type) {
			case 0:
				comments.insert("Page type",getPageTypeString(header.page_type));
				comments.insert("Scan direction",getDirectionString(header.scan));
				break;
			case 1:
				comments.insert("Line type",getLineTypeString(header.line_type));
				break;
			}
		comments.insert("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
		comments.insert("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
		comments.insert("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));
		comments.insert("GUID",getGUIDString(header.page_ID));
		comments.insert("Group ID",QString::number(header.group_ID));

}

QList< QPointF > RHKFileGenerator::pointsFromXY(int length, float* x, float* y) {
	QList<QPointF> points;
	for (int i = 0; i < length; i++)
		points << QPointF(x[i],y[i]);
	return points;
}

Q_EXPORT_PLUGIN2(rhk, RHKFileGenerator);
