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
#include "rhk4.h"
#include "NVBLogger.h"
#include <QSettings>

QStringList RHK4FileGenerator::availableInfoFields() const {
    return QStringList() \
            << "System note" \
            << "Session comment" \
            << "User comment" \
            << "Original path" \
            << "Aquisition date" \
            << "Aquisition time" \
            << "X axis label" \
            << "Y axis label" \
            << "Status channel" \
            << "Source type" \
            << "Image type" \
            << "Group ID" \
            << "Time per point" \
            << "Bias" \
            << "Setpoint" \
            << "GUID" \
            << "Page type" \
            << "Scan direction" \
            << "Line type" \
            ;
}

RHK4FileGenerator::RHK4FileGenerator()
	: NVBFileGenerator()
{
	subtractBias = qApp->property("NVBSettings").value<QSettings*>()->value("Plugins/RHK-SUBTRACT-BIAS",false).toBool();
}

NVBFile * RHK4FileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw()
{
	const char MAGIC[] = {  0x53, 0x00,
	0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00 }; // STiMage

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

	QMap<QString,NVBVariant> comments;
	RHKFile fileheader = getRHKHeader(file);
	QStringList strings;
	int version, version_minor, unicode;

	if (memcmp(fileheader.header.version, MAGIC, 16) != 0) { // Comparing "STiMage "
		NVBOutputError(QString("New page does not have recognizable RHK format. A shift must have been introduced due to incorect format implementation. Please, send the file %1 to Timofey").arg(file.fileName()));
		return NULL;
	}

	version = ((fileheader.header.version[8]-0x30)*10 + fileheader.header.version[9]-0x30)*10 + fileheader.header.version[10]-0x30;
	version_minor = ((fileheader.header.version[12]-0x30)*10 + fileheader.header.version[13]-0x30)*10 + fileheader.header.version[14]-0x30;

	if (version != 5) {
		NVBOutputError("Only RHK v4 files are supported by this plugin");
		return NULL;
		}

	unicode = fileheader.header.version[16]-0x30; // FIXME unicode is never used

	NVBFile * f = new NVBFile(info);
	if (!f) return 0;

	for (quint32 i=0;i<fileheader.page_index.page_count;i++) {

		RHKPageIndex * pi = fileheader.page_index.page_index_array + i;
		switch (pi->page_data_type) {
			case 0:
				f->addSource(new RHK4TopoPage(pi,file));
				break;
			case 1:
				f->addSource(new RHK4SpecPage(pi,file,subtractBias));
				break;
			default:
				NVBOutputError(QString("%1 : data type unsupported").arg(getPageTypeString(pi->page_data_type)));
				break;
			}

		}

	destroyRHKHeader(fileheader);

	return f;
}

NVBFileInfo * RHK4FileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw()
{
  const char MAGIC[] = {  0x53, 0x00,
  0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
	0x67, 0x00, 0x65, 0x00, 0x20, 0x00 }; // STiMage

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

//  QString name;
	NVB::PageType type;
//  QSize size;
  QMap<QString,NVBVariant> comments;
	RHKFile fileheader = getRHKHeader(file);
  QStringList strings;
	int version, version_minor, unicode;

	if (memcmp(fileheader.header.version, MAGIC, 16) != 0) { // Comparing "STiMage "
		NVBOutputError(QString("New page does not have recognizable RHK format. A shift must have been introduced due to incorect format implementation. Please, send the file %1 to Timofey").arg(file.fileName()));
		return NULL;
	}

	NVBFileInfo * fi = 0;
	fi = new NVBFileInfo(info);
	if (!fi) {
		NVBOutputError("NVBFileInfo creation failed");
		return 0;
		}

	version = ((fileheader.header.version[8]-0x30)*10 + fileheader.header.version[9]-0x30)*10 + fileheader.header.version[10]-0x30;
	version_minor = ((fileheader.header.version[12]-0x30)*10 + fileheader.header.version[13]-0x30)*10 + fileheader.header.version[14]-0x30;

	if (version != 5) {
		NVBOutputError("Only RHK v4 files are supported by this plugin");
		delete fi;
		return NULL;
		}

	unicode = fileheader.header.version[16]-0x30; // FIXME unicode is never used

	for (quint32 i=0;i<fileheader.page_index.page_count;i++) {

		RHKPageIndex * pi = fileheader.page_index.page_index_array + i;
		if (pi->page_data_type > 1) { // I have no idea what to do with this data
			NVBOutputError(QString("%1 : data type unsupported").arg(getPageTypeString(pi->page_data_type)));
			continue;
			}
		if (!pi->object_list) { // Object list didn't load
			NVBOutputError("Empty object list");
			continue;
			}

		comments.clear();

		RHKPageHeader header;

		for (quint32 j=0; j<pi->object_list_count;j++)
			switch(pi->object_list[j].id) {
				case  3: // Page header
					file.seek(pi->object_list[j].offset);
					file.read((char*)&header,qMin((qint64)(pi->object_list[j].data_size),(qint64)sizeof(header)));
					header.object_list = loadObjectList(file,header.object_list_count);
					if (!header.object_list) header.object_list_count = 0;
					break;
				case  4: // Page data
					NVBOutputPMsg("Skipping data");
//					file.seek(pi->object_list[j].offset);
//					file.read((char*)&header,pi->object_list[j].data_size);
					break;
				case 14: // Thumbmail
					NVBOutputPMsg("Skipping thumbnail");
					break;
				case 16: // Thumbnail header
					NVBOutputPMsg("Skipping thumbnail header");
					break;
				default:
					NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page index object list").arg(getObjectTypeString(pi->object_list[j].id)));
					break;
				}

		if (!header.object_list_count) {
			NVBOutputError("Page header not found");
			continue;
			}
		if (!header.object_list) {
			NVBOutputError("Page header object list empty");
			continue;
			}

		for (quint32 j=0; j<header.object_list_count;j++)
			switch(header.object_list[j].id) {
				case  5: // Image drift header
				case  6: // Image drift data
				case  7: // Spec drift header
				case  8: // Spec drift data
				case  9: // Color info
				case 11: // Tip track header
				case 12: // Tip track data
				case 17: // Tip track data
					NVBOutputPMsg(QString("Skipping %1").arg(getObjectTypeString(header.object_list[j].id)));
					break;
				case 10: // String data
					file.seek(header.object_list[j].offset);
					strings = loadRHKStrings(file,header.string_count);
					break;
				default:
					NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page header object list").arg(getObjectTypeString(header.object_list[j].id)));
					break;
				}

		comments.insert("System note",strings.at(1));
		comments.insert("Session comment",strings.at(2));
		comments.insert("User comment",strings.at(3));
		comments.insert("Original path",strings.at(4));
		comments.insert("Aquisition date",strings.at(5));
		comments.insert("Aquisition time",strings.at(6));
		comments.insert("X axis label",strings.at(10));
		comments.insert("Y axis label",strings.at(11));
		comments.insert("Status channel",strings.at(12));
//		comments.insert("Completed line count",strings.at(13));
//		comments.insert("Oversampling count",strings.at(14));
//		comments.insert("Sliced voltage",strings.at(15));
//		comments.insert("PLLPro Status",strings.at(16));

		for (int i = 17; i<strings.size(); i++) {
			comments.insert(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
			}

		comments.insert("Source type",getSourceTypeString(pi->page_source_type));
		comments.insert("Image type",getImageTypeString(header.image_type));
		comments.insert("Group ID",QString::number(header.group_ID));

		comments.insert("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
		comments.insert("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
		comments.insert("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));
		comments.insert("GUID",getGUIDString(pi->page_ID));

		switch (pi->page_data_type) {
			case 0 : { // Topography
				type = NVB::TopoPage;
				comments.insert("Page type",getPageTypeString(header.page_type));
				comments.insert("Scan direction",getDirectionString(header.scan));
				break;
				}
			case 1 : { // Spectroscopy
				type = NVB::SpecPage;
				comments.insert("Line type",getLineTypeString(header.line_type));
				break;
				}
			}

		fi->pages.append(NVBPageInfo(strings.at(0),type,QSize(header.x_size,header.y_size),comments));
		}

	destroyRHKHeader(fileheader);

  return fi;
}

RHK4TopoPage::RHK4TopoPage(RHKPageIndex * index, QFile & file):NVB3DPage()
{
	RHKPageHeader header;
	RHKColorInfo cInfo;
	qint32 * dataRHK = 0;

	if (!index->object_list) {
		NVBOutputError("Object list empty");
		throw;
		}
	
	for (quint32 j=0; j < index->object_list_count;j++)
		switch(index->object_list[j].id) {
			case  3: // Page header
				file.seek(index->object_list[j].offset);
				file.read((char*)&header,qMin((qint64)(index->object_list[j].data_size),(qint64)sizeof(header)));
				header.object_list = RHK4FileGenerator::loadObjectList(file,header.object_list_count);
				if (!header.object_list) header.object_list_count = 0;
				break;
			case  4: // Page data
					file.seek(index->object_list[j].offset);
					dataRHK = (qint32*)malloc(index->object_list[j].data_size);
					if (file.read((char*)dataRHK,index->object_list[j].data_size) < index->object_list[j].data_size) {
						NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
						free(dataRHK);
						}
				break;
			case 14: // Thumbmail
				NVBOutputPMsg("Skipping thumbnail");
				break;
			case 16: // Thumbnail header
				NVBOutputPMsg("Skipping thumbnail header");
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page index object list").arg(RHK4FileGenerator::getObjectTypeString(index->object_list[j].id)));
				break;
			}

	if (!header.object_list_count) {
		NVBOutputError("Page header not found");
		throw;
		}
	if (!header.object_list) {
		NVBOutputError("Page header object list empty");
		throw;
		}

	for (quint32 j=0; j<header.object_list_count;j++)
		switch(header.object_list[j].id) {
			case  5: // Image drift header
			case  6: // Image drift data
			case  7: // Spec drift header
			case  8: // Spec drift data
			case 11: // Tip track header
			case 12: // Tip track data
			case 17: // Tip track data
				NVBOutputPMsg(QString("Skipping %1").arg(RHK4FileGenerator::getObjectTypeString(header.object_list[j].id)));
				break;
			case  9: // Color info // FIXME there's a list there - we are only taking the first color scheme
				file.seek(header.object_list[j].offset);
				file.read((char*)&cInfo,header.object_list[j].data_size);
				break;
			case 10: // String data
				file.seek(header.object_list[j].offset);
				strings = RHK4FileGenerator::loadRHKStrings(file,header.string_count);
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page header object list").arg(RHK4FileGenerator::getObjectTypeString(header.object_list[j].id)));
				break;
			}

	pagename = strings.at(0).trimmed();

	xd = NVBDimension(strings.at(7));
	yd = NVBDimension(strings.at(8));
	zd = NVBDimension(strings.at(9));

	setComment("System note",strings.at(1));
	setComment("Session comment",strings.at(2));
	setComment("User comment",strings.at(3));
	setComment("Original path",strings.at(4));
	setComment("Aquisition date",strings.at(5));
	setComment("Aquisition time",strings.at(6));
	setComment("X axis label",strings.at(10));
	setComment("Y axis label",strings.at(11));
	setComment("Status channel",strings.at(12));
//		setComment("Completed line count",strings.at(13));
//		setComment("Oversampling count",strings.at(14));
//		setComment("Sliced voltage",strings.at(15));
//		setComment("PLLPro Status",strings.at(16));

	for (int i = 17; i<strings.size(); i++) {
		setComment(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
		}

	setComment("Source type",RHK4FileGenerator::getSourceTypeString(index->page_source_type));
	setComment("Image type",RHK4FileGenerator::getImageTypeString(header.image_type));

	setComment("Scan direction",RHK4FileGenerator::getDirectionString(header.scan));
	setComment("Group ID",QString::number(header.group_ID));

	setComment("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
	setComment("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
	setComment("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));
	setComment("GUID",RHK4FileGenerator::getGUIDString(index->page_ID));


  setComment("Page type",RHK4FileGenerator::getPageTypeString(header.page_type));

  _resolution = QSize(header.x_size,header.y_size);
  _position = QRectF(0,0,fabs(header.x_scale*header.x_size),fabs(header.y_scale*header.y_size));
  _position.moveCenter(QPointF(header.x_offset, -header.y_offset));

	if (dataRHK) {
		double * tdata = (double*)calloc(sizeof(double),header.x_size*header.y_size);

		scaler<qint32,double> intscaler(header.z_offset,header.z_scale);
		scaleMem<qint32,double>(tdata,intscaler,dataRHK,header.x_size*header.y_size);
		free(dataRHK);

	/*    int fx = header.x_scale > 0 ? 1 : -1;
		int fy = header.y_scale > 0 ? -1 : 1; // RHK y axis is inverted with respect to Qt's y axis*/
		data = (double*)calloc(sizeof(double),header.x_size*header.y_size);
		// RHK y axis is inverted with respect to Qt's y axis
		flipMem<double>(data, tdata, header.x_size, header.y_size, !(header.x_scale > 0), header.y_scale > 0 );
		free(tdata);

		getMinMax();

		if (data) {
			setColorModel(new NVBHSVWheelContColorModel(cInfo.start_h/360, cInfo.end_h/360, cInfo.start_s, cInfo.end_s, cInfo.start_b, cInfo.end_b,zMin,zMax));
			}
		}
}

RHK4SpecPage::RHK4SpecPage(RHKPageIndex * index, QFile & file, bool subtractBias):NVBSpecPage()
{
	RHKPageHeader header;
	RHKObject * data_obj = 0;
	RHKObject * xdata_obj = 0;

	if (!index->object_list) {
		NVBOutputError("Object list empty");
		throw;
		}

	for (quint32 j=0; j < index->object_list_count;j++)
		switch(index->object_list[j].id) {
			case  3: // Page header
				file.seek(index->object_list[j].offset);
				file.read((char*)&header,qMin((qint64)(index->object_list[j].data_size),(qint64)sizeof(header)));
				header.object_list = RHK4FileGenerator::loadObjectList(file,header.object_list_count);
				if (!header.object_list) header.object_list_count = 0;
				break;
			case  4: // Page data
				data_obj = index->object_list + j;
//					file.seek(index->object_list[j].offset);
//					dataRHK = (qint64*)malloc(index->object_list[j].data_size);
//					if (file.read((char*)dataRHK,index->object_list[j].data_size) < index->object_list[j].data_size) {
//						NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
//						free(dataRHK);
//						}
				break;
			case 14: // Thumbmail
				NVBOutputPMsg("Skipping thumbnail");
				break;
			case 16: // Thumbnail header
				NVBOutputPMsg("Skipping thumbnail header");
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page index object list").arg(RHK4FileGenerator::getObjectTypeString(index->object_list[j].id)));
				break;
			}

	if (!header.object_list_count) {
		NVBOutputError("Page header not found");
		throw;
		}
	if (!header.object_list) {
		NVBOutputError("Page header object list empty");
		throw;
		}

	for (quint32 j=0; j<header.object_list_count;j++)
		switch(header.object_list[j].id) {
			case  5: // Image drift header
			case  6: // Image drift data
			case  7: // Spec drift header
			case  9: // Color info
			case 11: // Tip track header
			case 12: // Tip track data
			case 17: // Tip track data
				NVBOutputPMsg(QString("Skipping %1").arg(RHK4FileGenerator::getObjectTypeString(header.object_list[j].id)));
				break;
			case  8: // Spec drift data
				xdata_obj = header.object_list + j;
				break;
			case 10: // String data
				file.seek(header.object_list[j].offset);
				strings = RHK4FileGenerator::loadRHKStrings(file,header.string_count);
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page header object list").arg(RHK4FileGenerator::getObjectTypeString(header.object_list[j].id)));
				break;
			}

	pagename = strings.at(0).trimmed();

	xd = NVBDimension("m");
	yd = NVBDimension("m");
	td = NVBDimension(strings.at(7));
	zd = NVBDimension(strings.at(9));

	setComment("System note",strings.at(1));
	setComment("Session comment",strings.at(2));
	setComment("User comment",strings.at(3));
	setComment("Original path",strings.at(4));
	setComment("Aquisition date",strings.at(5));
	setComment("Aquisition time",strings.at(6));
	setComment("X axis label",strings.at(10));
//	setComment("Y axis label",strings.at(11));
	setComment("Status channel",strings.at(12));
//		setComment("Completed line count",strings.at(13));
//		setComment("Oversampling count",strings.at(14));
//		setComment("Sliced voltage",strings.at(15));
//		setComment("PLLPro Status",strings.at(16));

	for (int i = 17; i<strings.size(); i++) {
		setComment(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
		}

	setComment("Source type",RHK4FileGenerator::getSourceTypeString(index->page_source_type));
	setComment("Image type",RHK4FileGenerator::getImageTypeString(header.image_type));

//	setComment("Scan direction",RHK4FileGenerator::getDirectionString(header.scan));
	setComment("Group ID",QString::number(header.group_ID));

	setComment("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
	setComment("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
	setComment("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));
	setComment("GUID",RHK4FileGenerator::getGUIDString(index->page_ID));


	setComment("Page type",RHK4FileGenerator::getPageTypeString(header.page_type));
  setComment("Line type",RHK4FileGenerator::getLineTypeString(header.line_type));

	if (!data_obj) {
		NVBOutputError("Data not found");
		return;
		}

  _datasize = QSize(header.x_size,header.y_size);

  xs = (double*)calloc(sizeof(double),header.x_size);
  if (header.x_scale > 0)
		for (int i = 0; i<header.x_size; i++)
			xs[i] = header.x_offset+i*header.x_scale;
  else
		for (int i = 0; i<header.x_size; i++)
			xs[i] = header.x_offset+(header.x_size-1-i)*header.x_scale;

	if (subtractBias)
		for (int i = 0; i<header.x_size; i++)
			xs[i]+=header.bias;

  ys = (double*)calloc(sizeof(double),header.x_size*header.y_size);
	file.seek(data_obj->offset);
  switch (header.line_type) {
    case 19: // Gdatalog
    case 21: // Gechem
    case 13: // Givnorm
    case  5: // Gfft_power # CORRECT (this is the actual value in the files)
    case  6: // Gfft_power # INCORRECT (this is the value listed in the manual)
    case 11: // Gautocorr
    case 18: // Goutspec
      {
		//
		//					dataRHK = (qint64*)malloc(index->object_list[j].data_size);
		//					if (file.read((char*)dataRHK,index->object_list[j].data_size) < index->object_list[j].data_size) {
		//						NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
		//						free(dataRHK);
		//						}

			float * tdata = (float*)malloc(header.page_data_size);
      if (file.read((char*)tdata,header.page_data_size) < header.page_data_size) {
				NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
        }
      else {
        scaler<float,double> floatscaler(0,1); //### Suppose there's no scaling
        scaleMem<float,double>(ys,floatscaler,tdata,header.x_size*header.y_size);
        }
      free(tdata);
      break;
      }
    default : {
			qint32 * tdata = (qint32*)malloc(header.page_data_size);
      if (file.read((char*)tdata,header.page_data_size) < header.page_data_size) {
				NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
        }
      else {
				scaler<qint32,double> intscaler(header.z_offset,header.z_scale);
				scaleMem<qint32,double>(ys,intscaler,tdata,header.x_size*header.y_size);
        }
      free(tdata);
      break;
      }
    }
  if (header.x_scale < 0) {
    double * tmp = (double*)calloc(sizeof(double),header.x_size*header.y_size);
    flipMem<double>(tmp,ys,header.x_size,header.y_size,true,false);
    free(ys);
    ys = tmp;
    }

	if ( header.page_type != 7 && header.page_type != 31 && xdata_obj) {

		file.seek(xdata_obj->offset);

		RHKSpecInfo sInfo;

		for(int i=0; i<header.y_size; i++) {
			file.read((char*)&sInfo,sizeof(RHKSpecInfo));
			_positions.append(QPointF(sInfo.x_coordinate,-sInfo.y_coordinate));
			_data.append(new QwtCPointerData(xs,ys+i*header.x_size,header.x_size));
      }
		}

  setColorModel(new NVBRandomDiscrColorModel(header.y_size));

}

RHK4SpecPage::~ RHK4SpecPage()
{
  while (!_data.isEmpty()) {
    delete _data.takeFirst();
    }
  if (xs) free(xs);
  if (ys) free(ys);
}

QString RHK4FileGenerator::getGUIDString(RHK_GUID id) {
	return QString("%1-%2-%3-%4").arg(id.Data1,0,16).arg(id.Data2,0,16).arg(id.Data3,0,16).arg(id.Data4,0,16);
}

QString RHK4FileGenerator::getLineTypeString(qint32 type) {
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

QString RHK4FileGenerator::getSourceTypeString(qint32 type) {
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

QString RHK4FileGenerator::getDirectionString(qint32 type) {
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

QString RHK4FileGenerator::getImageTypeString(qint32 type) {
  switch(type) {
    case 0 : return "Normal image";
    case 1 : return "Autocorrelation image";
    default : {
			NVBOutputError(QString("Unknown image type %1").arg(type));
      return QString();
      }
    }
}

QString RHK4FileGenerator::getPageDataTypeString(qint32 type) {
	switch(type) {
		case 0 : return "Image data";
		case 1 : return "Line/Spectral data";
		case 2 : return "XY data";
		case 3 : return "Annotated line/spectral data";
		case 4 : return "Text data";
		case 5 : return "Annotated text data";
		case 6 : return "Sequential data";
		default : {
			NVBOutputError(QString("Unknown page data type %1").arg(type));
			return QString();
			}
		}
}

QString RHK4FileGenerator::getPageTypeString(qint32 type) {
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

QString RHK4FileGenerator::getObjectTypeString(qint32 type) {
	switch(type) {
		case  0 : return "Undefined";
		case  1 : return "Page Index Header";
		case  2 : return "Page Index Array";
		case  3 : return "Page Header";
		case  4 : return "Page Data";
		case  5 : return "Image Drift Header";
		case  6 : return "Image Drift";
		case  7 : return "Spec Drift Header";
		case  8 : return "Spec Drift Data";
		case  9 : return "Color Info";
		case 10 : return "String data";
		case 11 : return "Tip Track Header";
		case 12 : return "Tip Track Data";
		case 13 : return "PRM";
		case 14 : return "Thumbnail";
		case 15 : return "PRM Header";
		case 16 : return "Thumbnail Header";
		case 17 : return "API Info";
		default : {
			NVBOutputError(QString("Invalid object type %1 found").arg(type));
			return QString();
			}
		}

}

RHKFile RHK4FileGenerator::getRHKHeader(QFile & file)
{
	RHKFile fileheader;

	// Get main header

	file.read((char*)&fileheader.header_size,2); // read header size
	if (fileheader.header_size > sizeof(RHKFileHeader)) {
		NVBOutputError("Unreasonably large header");
		return fileheader;
	}

	file.read((char*)&fileheader.header,fileheader.header_size);
	if (fileheader.header_size < (int)sizeof(RHKFileHeader))
		memset(((char*)&fileheader.header)+fileheader.header_size,0,sizeof(RHKFileHeader) - fileheader.header_size);

	if (fileheader.header.object_field_size != sizeof(RHKObject))
		NVBOutputError("Weird object size. This will crash"); // FIXME

	fileheader.header.object_list = loadObjectList(file,fileheader.header.object_list_count);
	if (!fileheader.header.object_list) fileheader.header.object_list_count = 0;
	
	for(quint32 i=0;i<fileheader.header.object_list_count;i++) {
		switch(fileheader.header.object_list[i].id) {
			case 1: // Page index header
				if (fileheader.header.object_list[i].data_size > sizeof(RHKPageIndexHeader)) {
					NVBOutputError("Size of page index header too large. Skipping");
					memset(&fileheader.page_index,0,sizeof(RHKPageIndexHeader));
					}
				else {
					file.seek(fileheader.header.object_list[i].offset);
					file.read((char*)&fileheader.page_index,qMin((qint64)(fileheader.header.object_list[i].data_size),(qint64)sizeof(fileheader.page_index)));
					fileheader.page_index.object_list = loadObjectList(file,fileheader.page_index.object_list_count);
					if (!fileheader.page_index.object_list) fileheader.page_index.object_list_count = 0;
					}
				break;
			case 13: // PRM data
				NVBOutputPMsg("Skipping PRM Data");
				break;
			case 15: // PRM header
				NVBOutputPMsg("Skipping PRM Header");
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type %1 in file header object list").arg(fileheader.header.object_list[i].id));
				break;
		}
	}

	if (!fileheader.page_index.page_count) return fileheader;

	const quint32 sizeof_RHKPageIndex = 32; // FIXME somehow

	for(quint32 i=0;i<fileheader.page_index.object_list_count;i++) {
		switch(fileheader.page_index.object_list[i].id) {
			case 2: // Page index array
				file.seek(fileheader.page_index.object_list[i].offset);
				fileheader.page_index.page_index_array = (RHKPageIndex*)malloc(sizeof(RHKPageIndex)*fileheader.page_index.page_count);
				memset((char*)fileheader.page_index.page_index_array,0,sizeof(RHKPageIndex)*fileheader.page_index.page_count);
				for (quint32 j=0; j<fileheader.page_index.page_count; j++) {
					quint64 success = file.read((char*)(fileheader.page_index.page_index_array+j),sizeof_RHKPageIndex);
					if (success < sizeof_RHKPageIndex) {
						NVBOutputError(QString("Error while reading next page index : %1").arg(file.errorString()));
						return fileheader;
						}
					fileheader.page_index.page_index_array[j].object_list = loadObjectList(file,fileheader.page_index.page_index_array[j].object_list_count);
					if (!fileheader.page_index.page_index_array[j].object_list) fileheader.page_index.page_index_array[j].object_list_count = 0;
					}
				break;
			default:
				NVBOutputPMsg(QString("Skipping object of unexpected type '%1' in page index header object list").arg(getObjectTypeString(fileheader.page_index.object_list[i].id)));
				break;
		}
	}

	return fileheader;
}

void RHK4FileGenerator::destroyRHKHeader(RHKFile fileheader) {
	if (fileheader.header.object_list) {
		free(fileheader.header.object_list);
		fileheader.header.object_list = 0;
		}
	if (fileheader.page_index.object_list) {
		free(fileheader.page_index.object_list);
		fileheader.page_index.object_list = 0;
		}
	if (fileheader.page_index.page_index_array) {
		for (quint32 j=0; j<fileheader.page_index.page_count; j++)
			if (fileheader.page_index.page_index_array[j].object_list)
				free(fileheader.page_index.page_index_array[j].object_list);
		free(fileheader.page_index.page_index_array);
		fileheader.page_index.page_index_array = 0;
		}
}

RHKObject * RHK4FileGenerator::loadObjectList(QFile &file, quint32 object_count) {
	quint64 list_size = object_count*sizeof(RHKObject);
	RHKObject * list = (RHKObject*)malloc(list_size);
	quint64 success = file.read((char*)list,list_size);
	if (success < list_size) {
		free(list);
		NVBOutputError(QString("Error while reading object list : %1").arg(file.errorString()));
		return 0;
		}
	else
		return list;
}

QString RHK4FileGenerator::loadRHKString(QFile & file) {
	QString r;

	quint32 * s;
	qint16 slen;
	file.read((char*)&slen,2);
	s = (quint32*)calloc(4,slen+1);
	for (int j = 0; j < slen; j++) file.read((char*)(s+j),2);
	r = QString::fromUcs4(s,slen);
	free(s);

	return r;

}

QStringList RHK4FileGenerator::loadRHKStrings(QFile & file, qint16 nstrings)
{
  QStringList r;
  quint32 * s;
  qint16 slen;
  for(int i = 0; i<nstrings; i++) {
    file.read((char*)&slen,2);
    s = (quint32*)calloc(4,slen+1);
    for (int j = 0; j < slen; j++) file.read((char*)(s+j),2);
    r << QString::fromUcs4(s,slen);
    free(s);
    }
  return r;
}




Q_EXPORT_PLUGIN2(rhk4, RHK4FileGenerator)
