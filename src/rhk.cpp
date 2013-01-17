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
#include "rhk.h"
#include "NVBLogger.h"
#include <QApplication>
#include "NVBSettings.h"

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

// ----------- New code

/*
bool RHKFileGenerator::canLoadFile(QString filename)
{
  if (filename.right(3).toLower() != "sm3") return false;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  if (!file.seek(2))
    return false;

  const char MAGIC[] = {
  0x53, 0x00, 0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
  0x67, 0x00, 0x65, 0x00, 0x20, 0x00};

  const char MAGIC_SIZE = sizeof(MAGIC);

  char test[MAGIC_SIZE];

  if (file.read(test,MAGIC_SIZE) != MAGIC_SIZE)
    return false;
  file.close();
  return (memcmp(test,MAGIC,MAGIC_SIZE) == 0);  
}
*/

QStringList RHKFileGenerator::availableInfoFields() const {
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

RHKFileGenerator::RHKFileGenerator()
	: NVBFileGenerator()
{
	subtractBias = qApp->property("NVBSettings").value<QSettings*>()->value("Plugins/RHK-SUBTRACT-BIAS",false).toBool();
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

	if (file.atEnd()) {
		NVBOutputError("Empty file");
		return 0;
		}

	NVBFile * f = new NVBFile(info);
	if (!f) return 0;

	while(!file.atEnd()) {
		NVBDataSource * page = loadNextPage(file);
		if (!page) break;
		f->addSource(page);
		}

	if (f->rowCount() == 0) {
		delete f;
		return 0;
		}

	return f;
}

NVBFileInfo * RHKFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw()
{
  const char MAGIC[] = {  0x53, 0x00,
  0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
  0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00,
  0x30, 0x00, 0x34, 0x00, 0x2E, 0x00, 0x30, 0x00,
  0x30, 0x00, 0x32, 0x00, 0x20, 0x00, 0x31, 0x00,
	0x00, 0x00}; // STiMage 004.002 1

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

//  QString name;
  NVB::PageType type;
//  QSize size;
  QMap<QString,NVBVariant> comments;
  TRHKHeader header;
  QStringList strings;
	int version, version_minor, unicode;

  while(!file.atEnd()) {

    comments.clear();

	file.peek((char*)&header,44);
	if (memcmp(header.version, MAGIC, 16) != 0) {
			NVBOutputError(QString("New page does not have recognizable RHK format. A shift must have been introduced due to incorect format implementation. Please, send the file %1 to Timofey").arg(file.fileName()));
      delete fi;
      return NULL;
    }

	header = getRHKHeader(file);
	if (!RHKHeaderIsSane(header,file.fileName())) {
		if (fi->pages.count() > 0)
			return fi;
		delete fi;
		return 0;
		}

	version = ((header.version[8]-0x30)*10 + header.version[9]-0x30)*10 + header.version[10]-0x30;
	version_minor = ((header.version[12]-0x30)*10 + header.version[13]-0x30)*10 + header.version[14]-0x30;

	if (version != 4) {
		NVBOutputError("Only RHK v4 files are supported by this plugin");
		delete fi;
		return NULL;
		}

	if (version_minor == 1) {
		header.colorinfo_count = 1;
		header.grid_ysize = 0;
		header.grid_xsize = 0;
		}

		unicode = header.version[16]-0x30; // FIXME unicode is never used

    strings = loadRHKStrings(file,header.string_count);
		if (strings.isEmpty()) {
			if (fi->pages.count() > 0)
				return fi;
			delete fi;
			return 0;
			}

    comments.insert("System note",strings.at(1));
    comments.insert("Session comment",strings.at(2));
    comments.insert("User comment",strings.at(3));
    comments.insert("Original path",strings.at(4));
    comments.insert("Aquisition date",strings.at(5));
    comments.insert("Aquisition time",strings.at(6));
    comments.insert("X axis label",strings.at(10));
    comments.insert("Y axis label",strings.at(11));
  
    if (strings.size() >= 13) comments.insert("Status channel",strings.at(12));
    for (int i = 13; i<strings.size(); i++) {
      comments.insert(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
      }
  
    comments.insert("Source type",getSourceTypeString(header.source_type));
    comments.insert("Image type",getImageTypeString(header.image_type));
    comments.insert("Group ID",QString::number(header.group_ID));
  
    comments.insert("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
    comments.insert("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
    comments.insert("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));
    comments.insert("GUID",getGUIDString(header.page_ID));
  
    file.seek(file.pos() + header.page_data_size);

		NVBPhysValue xspan, yspan;

    switch (header.type) {
      case 0 : { // Topography
        type = NVB::TopoPage;
        comments.insert("Page type",getPageTypeString(header.page_type));
        comments.insert("Scan direction",getDirectionString(header.scan));
				xspan = NVBPhysValue(fabs(header.x_scale*header.x_size),strings.at(7));
				yspan = NVBPhysValue(fabs(header.y_scale*header.y_size),strings.at(8));
        quint16 cs;
        file.peek((char*)&cs,2);
        file.seek(file.pos() + header.colorinfo_count*(cs+2));
        break;
        }
      case 1 : { // Spectroscopy
        type = NVB::SpecPage;
        comments.insert("Line type",getLineTypeString(header.line_type));
				xspan = NVBPhysValue(fabs(header.x_scale*header.x_size),strings.at(7));
/* // Silently ignore this -- all RHK spec pages I encountered have colorinfo_count == 1
        if (header.colorinfo_count != 0) 
					NVBOutputError(QString("Coloring specified for a spectroscopy page. The file %1 might be corrupted. If not, please, send a copy of %1 to Timofey").arg(file.fileName()));
*/
        if (header.page_type != 7 && header.page_type != 31 && header.line_type != 21 && header.line_type != 4)
          file.seek(file.pos() + 2*sizeof(float)*header.y_size);
        break;
        }
      case 3 : {
				NVBOutputError(QString("Annotated spectroscopy page found. No information on such a page exists. Please send the file %1 to the developer").arg(file.fileName()));
        type = NVB::SpecPage;
        file.seek(file.pos() + 2*sizeof(float)*header.y_size);
        }
      case 2 :
      default : {
				NVBOutputError(QString("Non-existing (%1) page found. Your file might be corrupted. If not, please send the file %2 to the developer").arg(header.type).arg(file.fileName()));
        type = NVB::InvalidPage;
        }
      }
		fi->pages.append(NVBPageInfo(strings.at(0),type,QSize(header.x_size,header.y_size),xspan,yspan,comments));
    }

  return fi;
}

NVBDataSource * RHKFileGenerator::loadNextPage(QFile & file) const
{
  const char MAGIC[] = {  0x53, 0x00,
  0x54, 0x00, 0x69, 0x00, 0x4D, 0x00, 0x61, 0x00,
  0x67, 0x00, 0x65, 0x00, 0x20, 0x00, 0x30, 0x00,
  0x30, 0x00, 0x34, 0x00, 0x2E, 0x00, 0x30, 0x00,
  0x30, 0x00, 0x32, 0x00, 0x20, 0x00, 0x31, 0x00,
  0x00, 0x00}; // STImage 004.002 1

  TRHKHeader h;
  file.peek((char*)&h,44);
  if (memcmp(h.version, MAGIC, 28) != 0) {
		NVBOutputError(QString("New page does not have recognizable RHK format. File %1 is probably corrupted").arg(file.fileName()));
		return 0;
    }
  switch (h.type) {
    case 0 : {
			NVBOutputVPMsg("Topography page found");
			RHKTopoPage * tp = new RHKTopoPage(file);
			if (!tp || !tp->getData()) {
				if (tp) delete tp;
				file.seek(file.size()-1);
				return 0;
				}
			return tp;
      }
    case 1 : {
			NVBOutputVPMsg("Spectroscopy page found");
			RHKSpecPage * sp = new RHKSpecPage(file,subtractBias);
			if (!sp || !sp->getData().count()) {
				if (sp) delete sp;
				file.seek(file.size()-1);
				return 0;
				}
			return sp;
			}
    case 3 : {
			NVBOutputError(QString("Annotated spectroscopy page found. No information on such a page exists. Please send the file %1 to the developer").arg(file.fileName()));

			TRHKHeader header = RHKFileGenerator::getRHKHeader(file);
			if (!RHKHeaderIsSane(header,file.fileName())) { // Stop loading
				file.seek(file.size()-1);
				return 0;
				}
			QStringList strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
			if (strings.isEmpty()) {
				file.seek(file.size()-1);
				return 0;
				}
			file.seek(file.pos() + header.page_data_size);
			file.seek(file.pos() + 2*sizeof(float)*header.y_size);
			return 0;
      }
    case 2 :
    default : {
			NVBOutputError(QString("Non-existing (%1) page found. File %2 is probably corrupted").arg(h.type).arg(file.fileName()));
			file.seek(file.size()-1);
			return 0;
      }
    }
}

RHKTopoPage::RHKTopoPage(QFile & file):NVB3DPage()
{
  header = RHKFileGenerator::getRHKHeader(file);
	if (!RHKFileGenerator::RHKHeaderIsSane(header,file.fileName()))
		return;

  int version = header.version[14]-0x30; // 0,1,..9
  if (version == 1) {
    header.colorinfo_count = 1;
    header.grid_ysize = 0;
    header.grid_xsize = 0;
    }

  setComment("Page type",RHKFileGenerator::getPageTypeString(header.page_type));

  strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
	if (strings.isEmpty())
		return;

  pagename = strings.at(0).trimmed();
  setComment("System note",strings.at(1));
  setComment("Session comment",strings.at(2));
  setComment("User comment",strings.at(3));
  setComment("Original path",strings.at(4));
  setComment("Aquisition date",strings.at(5));
  setComment("Aquisition time",strings.at(6));

  xd = NVBDimension(strings.at(7));
  yd = NVBDimension(strings.at(8));
  zd = NVBDimension(strings.at(9));

  setComment("X axis label",strings.at(10));
  setComment("Y axis label",strings.at(11));

  if (strings.size() >= 13) setComment("Status channel",strings.at(12));
  for (int i = 13; i<strings.size(); i++) {
    setComment(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
    }

  _resolution = QSize(header.x_size,header.y_size);
  _position = QRectF(0,0,fabs(header.x_scale*header.x_size),fabs(header.y_scale*header.y_size));
  _position.moveCenter(QPointF(header.x_offset, -header.y_offset));
  angle = -header.angle;

  setComment("Source type",RHKFileGenerator::getSourceTypeString(header.source_type));
  setComment("Image type",RHKFileGenerator::getImageTypeString(header.image_type));

  setComment("Scan direction",RHKFileGenerator::getDirectionString(header.scan));
  setComment("Group ID",QString::number(header.group_ID));

  setComment("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
  setComment("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
  setComment("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));

  setComment("GUID",RHKFileGenerator::getGUIDString(header.page_ID));

  qint32 * dataRHK = (qint32*)malloc(header.page_data_size);
  if (file.read((char*)dataRHK,header.page_data_size) < header.page_data_size) {
		NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
    free(dataRHK);
		return;
		}
  else {
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
    }

	/*
48 00
	00 00 00 40  88 BA 77 3F  C8 D2 A7 3E // Start HSV
	00 00 30 42  72 C0 02 3F  00 00 80 3F //   End HSV

	01 00 00 00  00 01 00 00 // Direction #Entries
	00 00 00 00  8C 2E BA 3E // %start %end

	00 00 80 3F  00 00 80 3F // Gamma  Alpha
	00 00 00 00  00 00 80 3F // X Start/Stop
	00 00 00 00  00 00 80 3F // Y Start/Stop

	00 00 00 00  01 00 00 00 // Mode   Invert

48 00
	00 00 00 40  88 BA 77 3F  C8 D2 A7 3E
	00 00 6C 42  2F C0 66 3F  00 00 80 3F

	01 00 00 00  00 01 00 00
	8C 2E BA 3E  0D EC 1D 3F

	00 00 80 3F  00 00 80 3F
	00 00 00 00  00 00 80 3F
	00 00 00 00  00 00 80 3F

	00 00 00 00  00 00 00 00

48 00
	00 00 4D 43  12 69 4F 3F  C8 D2 A7 3E
	00 80 91 43  99 2D 99 3E  00 00 80 3F

	01 00 00 00  00 01 00 00
	0D EC 1D 3F  D8 3B 66 3F

	B4 D4 CB 3E  00 00 80 3F
	00 00 00 00  00 00 80 3F
	00 00 00 00  00 00 80 3F

	01 00 00 00  00 00 00 00

48 00
	00 00 00 40  88 BA 77 3F  C8 D2 A7 3E
	00 00 6C 42  2F C0 66 3F  00 00 80 3F

	01 00 00 00  00 01 00 00
	D8 3B 66 3F  00 00 80 3F

	00 00 80 3F  CD CC CC 3D
	00 00 00 00  00 00 80 3F
	00 00 00 00  00 00 80 3F

	02 00 00 00  00 00 00 00

	 *
	 */

    TRHKColorInfo cInfo;

		if (file.read((char*)&(cInfo.parameter_count),2) != 2) { // read header size
			NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
			return;
			}
		if (cInfo.parameter_count+2 <= (int)sizeof(TRHKColorInfo)) {
			if (file.read((char*)&(cInfo.start_h),cInfo.parameter_count) != cInfo.parameter_count) {
				NVBOutputError(QString("File ended abruptly. File %1 probably corrupted.").arg(file.fileName()));
				return;
				}
			}
		else {
			NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
			if (file.read((char*)&(cInfo.start_h),(int)sizeof(TRHKColorInfo)) != (int)sizeof(TRHKColorInfo)) {
				NVBOutputError(QString("File ended abruptly. File %1 probably corrupted.").arg(file.fileName()));
				return;
				}
			file.seek(file.pos() + cInfo.parameter_count+2 - sizeof(TRHKColorInfo));
			}

    if (data) {
      setColorModel(new NVBHSVWheelContColorModel(cInfo.start_h/360, cInfo.end_h/360, cInfo.start_s, cInfo.end_s, cInfo.start_b, cInfo.end_b,zMin,zMax));
      }

	if (header.colorinfo_count > 1) {
		NVBOutputError(QString("Multiple coloring not implemented, the coloring in %1 will be approximate").arg(file.fileName()));
		for (int i = 1; i < header.colorinfo_count; i++) {
      qint16 len;
			if (file.read((char*)&len,2) != 2 || len < 2) {
				NVBOutputError(QString("Colorinfo doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
				return; // read past end offile
				}
			file.seek(file.pos() + len);
      }
    }
}

RHKSpecPage::RHKSpecPage(QFile & file, bool subtractBias):NVBSpecPage()
{
  
  header = RHKFileGenerator::getRHKHeader(file);
	if (!RHKFileGenerator::RHKHeaderIsSane(header,file.fileName()))
		return;

  int version = header.version[14]-0x30; // 0,1,..9
  if (version == 1) {
    header.colorinfo_count = 1;
    header.grid_ysize = 0;
    header.grid_xsize = 0;
    }

//  if (!file) file = file;

  setComment("Line type",RHKFileGenerator::getLineTypeString(header.line_type));

  strings = RHKFileGenerator::loadRHKStrings(file,header.string_count);
	if (strings.isEmpty())
		return;

  pagename = strings.at(0).trimmed();
  setComment("System note",strings.at(1));
  setComment("Session comment",strings.at(2));
  setComment("User comment",strings.at(3));
  setComment("Original path",strings.at(4));
  setComment("Aquisition date",strings.at(5));
  setComment("Aquisition time",strings.at(6));

  // TODO Find out how to get this dimension
  xd = NVBDimension("m");
  yd = NVBDimension("m");
  td = NVBDimension(strings.at(7));
  zd = NVBDimension(strings.at(9));

  setComment("X axis label",strings.at(10));
//  setComment("Y axis label",strings.at(11));

  if (strings.size() >= 13) setComment("Status channel",strings.at(12));
  for (int i = 13; i<strings.size(); i++) {
    setComment(QString("Unexpected comment #%1").arg(i-12),strings.at(i));
    }

  _datasize = QSize(header.x_size,header.y_size);

  setComment("Source type",RHKFileGenerator::getSourceTypeString(header.source_type));
  setComment("Image type",RHKFileGenerator::getImageTypeString(header.image_type));

//  setComment("Scan direction",RHKFileGenerator::getDirectionString(header.scan));
  setComment("Group ID",QString::number(header.group_ID));

  setComment("Time per point",NVBPhysValue(QString("%1 s").arg(header.period)));
  setComment("Bias",NVBPhysValue(QString("%1 V").arg(header.bias)));
  setComment("Setpoint",NVBPhysValue(QString("%1 A").arg(header.current)));

  setComment("GUID",RHKFileGenerator::getGUIDString(header.page_ID));

  xs = (double*)calloc(sizeof(double),header.x_size);
  if (header.x_scale > 0)
		for (int i = 0; i<header.x_size; i++)
			xs[i] = header.x_offset+i*header.x_scale;
  else
		for (int i = 0; i<header.x_size; i++)
			xs[i] = header.x_offset+(header.x_size-1-i)*header.x_scale;

	if (subtractBias)
		for (int i = 0; i<header.x_size; i++)
			xs[i] += header.bias;

  ys = (double*)calloc(sizeof(double),header.x_size*header.y_size);
  switch (header.line_type) {
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
				return;
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
				free(tdata);
				return;
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

  float * posdata = (float*)calloc(sizeof(float),2*header.y_size);
  float * xposdata = posdata;
  float * yposdata = posdata + header.y_size;
	if ( header.page_type != 7 && header.line_type != 4 && header.line_type != 21 && header.page_type != 31  &&  file.read((char*)posdata,2*sizeof(float)*header.y_size) < (qint64)(2*sizeof(float)*header.y_size)) {
		NVBOutputError(QString("File %1 ended before the page could be fully read").arg(file.fileName()));
    }
    for (int i = 0; i<header.y_size; i++) {
      _positions.append(QPointF(xposdata[i],-yposdata[i]));
      _data.append(new QwtCPointerData(xs,ys+i*header.x_size,header.x_size));
      }

  free(posdata);

  setColorModel(new NVBRandomDiscrColorModel(header.y_size));

	getMinMax();


/* // Silently ignore this -- all RHK spec pages I encountered have colorinfo_count == 1
  if (header.colorinfo_count != 0) 
		NVBOutputError(QString("Coloring specified for a spectroscopy page. The file %1 might be corrupted. If not, please, send a copy of %1 to Timofey").arg(file.fileName()));
*/

}

RHKSpecPage::~ RHKSpecPage()
{
  while (!_data.isEmpty()) {
    delete _data.takeFirst();
    }
  if (xs) free(xs);
  if (ys) free(ys);
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
	memset((char*)&header,0,sizeof(TRHKHeader));
	if (file.read((char*)&header.parameter_size,2) != 2) { // read header size
		NVBOutputFileError(&file);
		return header;
		}

	if ( header.parameter_size < 0 || header.parameter_size > file.size()) {
		NVBOutputError(QString("Page header doesn't conform to spec. File %1 probably corrupted.").arg(file.fileName()));
		return header;
		}

	if (header.parameter_size+2 > (int)sizeof(TRHKHeader)) {
		NVBOutputError("Header too big. Trunkated.");
		if (file.read((char*)header.version,sizeof(TRHKHeader)-2) != sizeof(TRHKHeader)-2) {
			NVBOutputFileError(&file);
			header.parameter_size = 0;
			return header;
			}
		file.seek(file.pos() + header.parameter_size+2-sizeof(TRHKHeader));
		}
	else {
		if (file.read((char*)header.version,header.parameter_size) != header.parameter_size) {
			NVBOutputFileError(&file);
			header.parameter_size = 0;
			return header;
			}
		memset(((char*)header.version)+header.parameter_size,0,sizeof(TRHKHeader) - header.parameter_size - 2);
		}
  return header;
}

bool RHKFileGenerator::RHKHeaderIsSane(const TRHKHeader & header, QString filename) {
	if (! header.parameter_size) {
		// getRHKHeader returns an error in this case
		return false;
		}

	// Number of strings is positive and at least 10 (at most 18, as of last specification)
	if (header.string_count < 10 || header.string_count > 30) {
		NVBOutputError(QString("Page header has unexpected number of strings. File %1 is probably corrupted.").arg(filename));
		return false;
		}

	// Reasonable size
	if (header.x_size < 0 || header.y_size < 0 || header.page_data_size < (quint32)header.x_size || header.page_data_size < (quint32)header.y_size || header.page_data_size != abs(4*header.x_size*header.y_size)) {
		NVBOutputError(QString("Page has unexpected dimentions. File %1 is probably corrupted.").arg(filename));
		return false;
		}

	return true;
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
  return r;
}

Q_EXPORT_PLUGIN2(rhk, RHKFileGenerator)
