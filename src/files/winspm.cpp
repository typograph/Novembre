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
#include "winspm.h"
#include "../core/dimension.h"
#include "../core/NVBContColoring.h"
#include "../core/NVBDiscrColoring.h"
#include <QDir>
#include "../core/NVBFileInfo.h"
#include "../core/NVBFile.h"

QStringList WinSPMFileGenerator::availableInfoFields() const {
	return QStringList() \
	       << "Bias" \
	       << "Setpoint" \
	       ;
	}

NVBAssociatedFilesInfo WinSPMFileGenerator::associatedFiles(QString filename) const {
	QStringList filenames(filename);

	QFileInfo info(filename);
	QDir dir;
	dir.setPath(info.absolutePath());
	QFileInfoList spcs = dir.entryInfoList(QStringList(QString("%1*.spc").arg(info.baseName())), QDir::Files, QDir::Name);

	foreach(QFileInfo spc, spcs)
	filenames << spc.filePath();

	return NVBAssociatedFilesInfo(info.baseName(), filenames, this);
	}

/*
bool WinSPMFileGenerator::canLoadFile(QString filename)
{
// 	return true;

	if (filename.right(3).toLower() != "tif") return false;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	const char MAGIC_SIZE = sizeof(TIFF_HEADER);

	char test[MAGIC_SIZE];

	if (file.read(test,MAGIC_SIZE) != MAGIC_SIZE)
		return false;
	file.close();
	return (memcmp(test,TIFF_HEADER,MAGIC_SIZE) == 0);
}
*/

NVBFile * WinSPMFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
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

	if (!f) return 0;

// -------- read TIFF header

	struct {
		qint16 endianness;
		qint16 answer;
		quint32 dir_start;
		quint16 dir_size;
		} tiff_header;

	file.read((char*)&tiff_header, 10); // Byte alignment issue

	if (file.size() < tiff_header.dir_start + tiff_header.dir_size) {
		NVBOutputError("File is too small");
		return 0;
		}

// --------- read WinSPM header

	TWinSPM::Header header;

	if (!getWinSPMHeader(header, file)) {
		delete f;
		return 0;
		}

	NVBOutputVPMsg(QString("File version %1").arg(header.winspm_version));

// --------- read first directory

	file.seek(tiff_header.dir_start);

	qint16 num_tags;
	TIFF_tag tag, color_map_tag ;// FIXME unused , make_tag, model_tag, hostpc_tag;
	quint32 strip_offset = 0; // FIXME unused, strip_rows, strip_bytes;

	file.read((char*)&num_tags, 2);

	for (int i = 0; i < num_tags; i++) {
		file.read((char*)&tag, 12);

		switch (tag.tag_type) {
			case TT_IMAGE_WIDTH : {
				if (tag.data_type != TIFF_DATA_TYPE::FOUR_BYTES ||
				    tag.data_length != 1 || tag.data != header.xres) {
					NVBOutputError(QString("Image width tag inconsistent with format %1 %2").arg(tag.data).arg(header.xres));
					return 0;
					}

				break;
				}

			case TT_IMAGE_HEIGHT : {
				if (tag.data_type != TIFF_DATA_TYPE::FOUR_BYTES ||
				    tag.data_length != 1 || tag.data != header.yres) {
					NVBOutputError(QString("Image height tag inconsistent with format %1 %2").arg(tag.data).arg(header.yres));
					return 0;
					}

				break;
				}

			case TT_BITS_PER_SAMPLE : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES ||
				    tag.data_length != 1) {
					// || (tag.data != 8 && header.compressed != 0) || (tag.data == 8 && header.compressed == 0)
					NVBOutputError("Bits per sample tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_COMPRESSION : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES ||
				    tag.data_length != 1) {
//         || (tag.data != 8 && header.compressed != 0) || (tag.data == 8 && header.compressed == 0)) {
					NVBOutputError("Compression tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_PHOTOMETRIC_INTERPRETATION : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES ||
				    tag.data_length != 1 || tag.data != 1) {
					NVBOutputError("Photometric interpretation tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_IMAGE_DESCRIPTION : {
				if (tag.data_type != TIFF_DATA_TYPE::ASCII) {
					NVBOutputError("Image description tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_MAKE : {
				if (tag.data_type != TIFF_DATA_TYPE::ASCII) {
					NVBOutputError("Make tag inconsistent with format");
					return 0;
					}

				// FIXME unused make_tag = tag;
				break;
				}

			case TT_MODEL : {
				if (tag.data_type != TIFF_DATA_TYPE::ASCII) {
					NVBOutputError("Model tag inconsistent with format");
					return 0;
					}

				// FIXME unused model_tag = tag;
				break;
				}

			case TT_STRIP_OFFSET : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES || tag.data_length != 1) {
					NVBOutputError("Strip offset tag inconsistent with format");
					return 0;
					}

				strip_offset = (quint16)(tag.data);
				break;
				}

			case TT_SAMPLES_PER_PIXEL : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES ||
				    tag.data_length != 1 || tag.data != 1) {
					NVBOutputError("Samples per pixel tag inconsistent with format");
					return 0;
					}

//        strip_offset = tag.data;
				break;
				}

			case TT_ROWS_PER_STRIP : {
				if (tag.data_type != TIFF_DATA_TYPE::FOUR_BYTES || tag.data_length != 1) {
					NVBOutputError("Strip offset tag inconsistent with format");
					return 0;
					}

				// FIXME unused strip_rows = tag.data;
				break;
				}

			case TT_STRIP_BYTE_COUNT : {
				if (tag.data_type != TIFF_DATA_TYPE::FOUR_BYTES || tag.data_length != 1) {
					NVBOutputError("Strip offset tag inconsistent with format");
					return 0;
					}

				// FIXME unused strip_bytes = tag.data;
				break;
				}

			case TT_X_RESOLUTION :
			case TT_Y_RESOLUTION : {
				if (tag.data_type != TIFF_DATA_TYPE::RATIONAL || tag.data_length != 1) {
					NVBOutputError("Resolution tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_RESOLUTION_UNIT : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES || tag.data_length != 1) {
					NVBOutputError("Resolution unit tag inconsistent with format");
					return 0;
					}

				break;
				}

			case TT_HOST_COMPUTER : {
				if (tag.data_type != TIFF_DATA_TYPE::ASCII) {
					NVBOutputError("Host computer tag inconsistent with format");
					return 0;
					}

				// FIXME unused hostpc_tag = tag;
				break;
				}

			case TT_COLOR_MAP : {
				if (tag.data_type != TIFF_DATA_TYPE::TWO_BYTES ||
				    tag.data_length != 0x300 || tag.data != 0x41BD4) {
					NVBOutputError("Color map tag inconsistent with format");
					return 0;
					}

				// FIXME unused color_map_tag = tag;
				break;
				}

			default :
				break;
			}
		}

	/*
	// --------- read data

	  // strip_bytes == header.yres;
	  // strip_rows == header.yres

	  if (header.cits_offset) {
	    file.seek(header.cits_offset);
	    // read CITS
	    }

	  if (header.fft_offset) {
	    file.seek(header.fft_offset);
	    // read FFT
	    }

	  if (header.montage_offset) {
	    file.seek(header.montage_offset);
	    // read Montage ??? ###
	    }
	*/

	f->addSource(new WinSPMTopoPage(file, header, strip_offset, color_map_tag.data));

	if (header.cits_offset) {
		file.seek(header.cits_offset);
		f->addSource(new WinSPMCITSPage(file, header));
//     s->pages.append(WinSPMCITSPage::slices(file,header));
		}

	if (header.sps_offset) {
		file.seek(header.sps_offset);

		for (int i = 0; i < header.n_of_sps; i++) {
			quint32 next_sps_offset = file.pos() + 0x17CA;
			TWinSPM::Header specheader;
			getWinSPMHeader(header, file);
			NVBOutputVPMsg(QString("Spectroscopy file version %1").arg(specheader.winspm_version));
			f->addSource(new WinSPMSpecPage(file, header, specheader, next_sps_offset));
			}
		}
	else {
		if (info.count() > 1) {

			WinSPMSpecPage * specpage = new WinSPMSpecPage(header);

			for (QStringList::const_iterator it = ++(info.begin()); it != info.end(); ++it) {

				QFile specfile(*it);

				if (!specfile.open(QIODevice::ReadOnly))
					return f;

				TWinSPM::Header specheader;
				getWinSPMHeader(specheader, specfile);
				NVBOutputVPMsg(QString("Spectroscopy file version %1").arg(specheader.winspm_version));

				specpage->addCurve(specfile, specheader, (quint32)0x17CA);

				specfile.close();
				}

			f->addSource(specpage);
			}
		}

	file.close();

	return f;
	}

NVBFileInfo * WinSPMFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() {
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

	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) {
		NVBOutputError("Out of memory");
		return 0;
		}

//  QString name;
//  NVB::PageType type;
//  QSize size;
	QMap<QString, NVBVariant> comments;

// -------- read TIFF header

	struct {
		qint16 endianess;
		qint16 answer;
		quint32 dir_start;
		quint16 dir_size;
		} tiff_header;

	file.read((char*)&tiff_header, sizeof(tiff_header));

	if (memcmp(&tiff_header, TIFF_HEADER, sizeof(TIFF_HEADER)) != 0)	{
		NVBOutputError("File format magic mismatch.");
		return 0;
		}

	if (file.size() < tiff_header.dir_start + tiff_header.dir_size) {
		NVBOutputError("File is too small");
		return 0;
		}

// --------- read WinSPM header

	TWinSPM::Header header;

	getWinSPMHeader(header, file);

	comments.insert("Bias", NVBPhysValue(QString("%1 V").arg(header.bias)));
	comments.insert("Setpoint", NVBPhysValue(QString("%1 nA").arg(header.reference_value)));

	fi->pages.append(NVBPageInfo("Topography", NVB::TopoPage, QSize(header.xres, header.yres), NVBPhysValue(header.xreal, NVBDimension("nm")), NVBPhysValue(header.yreal, NVBDimension("nm")), comments));

	if (header.sps_offset) {
		// FIXME: Spectroscopy format unknown at that point
		fi->pages.append(NVBPageInfo("Spectroscopy", NVB::SpecPage, QSize(header.xres, header.yres), NVBPhysValue(), NVBPhysValue(), comments));
		}
	else if (info.count() > 1) {
		fi->pages.append(NVBPageInfo("Spectroscopy", NVB::SpecPage, QSize(header.xres, info.count() - 1), NVBPhysValue(), NVBPhysValue(), comments));
		}

//	file.seek(data_offset);

//	QwtArray<double> xs(header.xres), ys(header.xres);
//	float * raw_data = (float*)malloc(4*header.xres);

//	file.read((char*)raw_data,header.xres*4);
//	for (int i = 0; i < header.xres; i++)
//		ys << raw_data[i];

//	file.read((char*)raw_data,header.xres*4);
//	for (int i = 0; i < header.xres; i++)
//		xs << raw_data[i];


	file.close();

	return fi;
	}


WinSPMTopoPage::WinSPMTopoPage(QFile & file, const TWinSPM::Header & _header, quint32 data_offset, quint32 color_offset): header(_header) {
	pagename = "Topography";

	xd = NVBDimension("nm");
	yd = NVBDimension("nm");
	zd = NVBDimension("nm");

	setComment("X axis label", QString("X"));
	setComment("Y axis label", QString("Y"));
	setComment("Z axis label", QString("Z"));

	_resolution = QSize(header.xres, header.yres);
	_position = QRectF(0, 0, header.xreal, header.yreal);
	_angle = -header.spm_param.rotation;

	setComment("Bias", NVBPhysValue(QString("%1 V").arg(header.bias)));
	setComment("Setpoint", NVBPhysValue(QString("%1 nA").arg(header.reference_value)));

	quint32 data_bytes;

	file.seek(data_offset);

	if (header.bpp == 16) {

		data_bytes = header.xres * header.yres * 2;
		quint16 * idata = (quint16*)malloc(data_bytes);
		data = (double*) malloc(data_bytes * 4); // databytes * 8 / 2

		file.read((char*)idata, data_bytes);

		scaler<quint16, double> intscaler(header.z0, (header.z255 - header.z0) / 65535);
		scaleMem<quint16, double>(data, intscaler, idata, header.xres * header.yres);

		free(idata);

		}
	else {

		data_bytes = header.xres * header.yres;
		quint8 * idata = (quint8*)malloc(data_bytes);
		data = (double*) malloc(data_bytes * 8); // databytes * 8

		file.read((char*)idata, data_bytes);

		scaler<quint8, double> intscaler(header.z0, (header.z255 - header.z0) / 255);
		scaleMem<quint8, double>(data, intscaler, idata, header.xres * header.yres);

		free(idata);

		file.seek(color_offset);
		quint16 colormap[256][3];
		file.read((char*)&colormap, 1536);

		}

	getMinMax();

	setColorModel(new NVBGrayRampContColorModel(0, 1, header.z0, header.z255));

	}

bool WinSPMFileGenerator::getWinSPMHeader(TWinSPM::Header & header, QFile & file) {
	file.read((char*) & (header.winspm_version), 2);
	file.read((char*)header.internal_filename_b4, 80);
	file.read((char*) & (header.xres), 2);
	file.read((char*) & (header.yres), 2);
	file.read((char*) & (header.xreal), 4);
	file.read((char*) & (header.yreal), 4);
	file.read((char*) & (header.z0), 4);
	file.read((char*) & (header.z255), 4);
	file.read((char*) & (header.adc_min), 2);
	file.read((char*) & (header.adc_max), 2);
	file.read((char*) & (header.initial_scan_scale), 4);
	file.read((char*)header.internal_filename, 40);
	file.read((char*)header.info, 200);
	file.read((char*)header.history, 50);
	file.read((char*) & (header.has_current_info), 2);
	file.read((char*) & (header.bias), 4);
	file.read((char*) & (header.reference_value), 4);
	file.read((char*) & (header.reserved1), 2);

	file.read((char*) & (header.measurement_date), 5);
	file.read((char*) & (header.save_date), 5);
	file.read((char*) & (header.measurement_time), 4);
	file.read((char*) & (header.save_time), 4);

	file.read((char*) & (header.lookup_table), 256);
	file.read((char*) & (header.fft_offset), 4);
	file.read((char*) & (header.transform_off), 2);
	file.read((char*) & (header.selected_region), 8);
	file.read((char*) & (header.compressed), 1);
	file.read((char*) & (header.bpp), 1);
	file.read((char*) & (header.cits_offset), 4);
	file.read((char*) & (header.backscan_tip_voltage), 4);
	file.read((char*) & (header.sts_point_x), 2);
	file.read((char*) & (header.sts_point_y), 2);
	file.read((char*)header.reserved2, 10);
	file.read((char*)header.reserved3, 10);
	file.read((char*) & (header.tip_speed_x), 4);
	file.read((char*) & (header.tip_speed_y), 4);
	file.read((char*) & (header.piezo_sensitivity), 42);
	file.read((char*) & (header.spm_param), 60);
	file.read((char*) & (header.montage_offset), 4);
	file.read((char*)header.image_location, 260);
	file.read((char*) & (header.spm_misc_param), 118);
	file.read((char*)header.red_lookup, 256);
	file.read((char*)header.green_lookup, 256);
	file.read((char*)header.blue_lookup, 256);
	file.read((char*) & (header.sub_revision_no), 4);
	file.read((char*) & (header.image_type), 4);
	file.read((char*) & (header.data_source), 4);
	file.read((char*) & (header.display_mode), 4);
	file.read((char*) & (header.measurement_start_time), 8);
	file.read((char*) & (header.measurement_end_time), 8);
	file.read((char*) & (header.profile_roughness), 254);
	file.read((char*) & (header.settings_3d), 446);
	file.read((char*) & (header.lut_brightness), 4);
	file.read((char*) & (header.lut_contrast), 4);
	file.read((char*) & (header.software_version), 2);
	file.read((char*) & (header.software_subversion), 4);
	file.read((char*) & (header.extracted_region), 20);
	file.read((char*) & (header.lut_gamma), 4);
	file.read((char*) & (header.n_of_sps), 2);
	file.read((char*) & (header.sps_offset), 4);
	file.read((char*) & (header.line_trace_end_x), 2);
	file.read((char*) & (header.line_trace_end_y), 2);
	file.read((char*) & (header.forward), 2);
	file.read((char*) & (header.lift_signal), 2);
	file.read((char*) & (header.lia_settings), 132);
	file.read((char*) & (header.temp_param), 32);
	file.read((char*) & (header.converted_afm), 26);
	file.read((char*) & (header.special_measurement_param), 62);

	if (file.atEnd()) {
		NVBOutputError("File is too small to contain a header");
		return false;
		}

	return true;
	}


WinSPMSpecPage::WinSPMSpecPage(const TWinSPM::Header & topo_header): theader(topo_header) {

	pagename = "Spectroscopy";

	xd = NVBDimension("nm");
	yd = NVBDimension("nm");
	zd = NVBDimension("nA");
	td = NVBDimension("V");

	setComment("X axis label", QString("X"));
	setComment("Y axis label", QString("Y"));
	setComment("Z axis label", QString("Z"));

	_datasize = QSize(0, 0);

	colors = new NVBConstDiscrColorModel(Qt::black);

	}

WinSPMSpecPage::WinSPMSpecPage(QFile & file, const TWinSPM::Header & topo_header, const TWinSPM::Header & _header, quint32 data_offset): theader(topo_header) {

	Q_UNUSED(file);
	Q_UNUSED(_header);
	Q_UNUSED(data_offset);

	// FIXME: Spectroscopy format unknown at that point

	pagename = "Spectroscopy";

	xd = NVBDimension("nm");
	yd = NVBDimension("nm");
	zd = NVBDimension("nA");
	td = NVBDimension("V");

	setComment("X axis label", QString("X"));
	setComment("Y axis label", QString("Y"));
	setComment("Z axis label", QString("Z"));

	_datasize = QSize(0, 0);

	colors = new NVBConstDiscrColorModel(Qt::black);
	}

void WinSPMSpecPage::addCurve(QFile & file, const TWinSPM::Header & header, quint32 data_offset) {

	_positions << QPointF(header.sts_point_x * theader.xreal / theader.xres, header.sts_point_y * theader.yreal / theader.yres);

	file.seek(data_offset);

	QwtArray<double> xs(header.xres), ys(header.xres);
	float * raw_data = (float*)malloc(4 * header.xres);

	file.read((char*)raw_data, header.xres * 4);

	for (int i = 0; i < header.xres; i++)
		ys << raw_data[i];

	file.read((char*)raw_data, header.xres * 4);

	for (int i = 0; i < header.xres; i++)
		xs << raw_data[i];

	_data << new QwtArrayData(xs, ys);

	free(raw_data);

	_datasize.rwidth() = header.xres;
	_datasize.rheight() += 1;

	getMinMax();

//	file.read(,);

	}

WinSPMCITSPage::WinSPMCITSPage(QFile & file, const TWinSPM::Header & _header): header(_header) {
	pagename = "Spectroscopy";

	xd = NVBDimension("nm");
	yd = NVBDimension("nm");
	zd = NVBDimension("nA");
	td = NVBDimension("V");

	setComment("X axis label", QString("X"));
	setComment("Y axis label", QString("Y"));
	setComment("Z axis label", QString("Z"));

	file.seek(0x9900); // It's just there -- I don't know why
	getCITSHeader(cits_header, file);

	_datasize = QSize(cits_header.n_of_cits, 128 * 128);

	setComment("Bias", NVBPhysValue(QString("%1 V").arg(header.bias)));
	setComment("Setpoint", NVBPhysValue(QString("%1 nA").arg(header.reference_value)));

	colors = new NVBRandomDiscrColorModel(128 * 128);

	QwtArray<double> xs, ys[128 * 128];

	quint16 * raw_data = (quint16*)malloc(128 * 128 * 2);

	quint32 cio = 0x9A86;//file.pos();

	for (int i = 0; i < cits_header.n_of_cits; i++) {

		file.seek(cio + i * 165);

		TWinSPM::CITS_ImageHeader img_header;
		getCITSImageHeader(img_header, file);

		xs << img_header.voltage;

		file.seek(cio + cits_header.n_of_cits * 165 + i * 128 * 128 * 2);
		file.read((char*)raw_data, 128 * 128 * 2);

		scaler<quint16, double> data_scaler(img_header.current0, (img_header.current64 - img_header.current0) / 65536);

		for (int j = 0; j < 128 * 128; j++)
			ys[j] << data_scaler.scale(raw_data[j]);

		}

	free(raw_data);

	for (int i = 0; i < 128; i++)
		for (int j = 0; j < 128; j++) {
			_positions << QPointF(j * header.xreal / header.xres, i * header.yreal / header.yres);
			_data << new QwtArrayData(xs, ys[i * 128 + j]);
			}

	getMinMax();
	}

void WinSPMCITSPage::getCITSHeader(TWinSPM::CITS_Header & header, QFile & file) {
	file.read((char*) & (header.winspm_version), 2);
	file.read((char*) & (header.n_of_cits), 2); // Number of CITS images
	file.read((char*) & (header.cits_width), 2); // 128
	file.read((char*) & (header.cits_height), 2); // 128
	file.read((char*) & (header.n_of_current), 2); // Number of current images
	file.read((char*) & (header.check_flag), 2); // only first 3 bits have meaning
	file.read((char*) & (header.n_display_cits), 2); // Number of cits images to display
	file.read((char*) & (header.disp_mode), 1);
	file.read((char*) & (header.conductance0), 8); // Conductance at level 0
	file.read((char*) & (header.conductance255), 8); // Conductance at level 255
	file.read((char*) & (header.disp_sel), 1); // Display selection
	file.read((char*) & (header.n_cits), 2); // Number of cits image
	file.seek(file.pos() + 100);
	}

void WinSPMCITSPage::getCITSImageHeader(TWinSPM::CITS_ImageHeader & header, QFile & file) {
	file.read((char*) & (header.adc_min), 2);
	file.read((char*) & (header.adc_max), 2);
	file.read((char*) & (header.adc_source), 1); // unused
	file.read((char*) & (header.adc_offset), 2);
	file.read((char*) & (header.adc_gain), 2);
	file.read((char*) & (header.head_amp_gain), 4);
	file.read((char*) & (header.voltage), 4); // Image voltage
	file.read((char*) & (header.current0), 4); // Current at image data=0
	file.read((char*) & (header.current64), 4); // Current at image data = 64 ???
	file.read((char*)(header.title), 40); // CITS Image title
	file.read((char*) & (header.source_type), 4);
	file.seek(file.pos() + 96);
	}

WinSPMCITSSlicePage::WinSPMCITSSlicePage(const TWinSPM::Header & _header, const TWinSPM::CITS_ImageHeader & cits_header, double * _data): NVB3DPage(), header(_header) {
	data = _data;

	pagename = QString("CITS: %1 V").arg(cits_header.voltage);

	xd = NVBDimension("nm");
	yd = NVBDimension("nm");
	zd = NVBDimension("nA");

	setComment("X axis label", QString("X"));
	setComment("Y axis label", QString("Y"));
	setComment("Z axis label", QString("I"));

	_resolution = QSize(128, 128);
	_position = QRectF(0, 0, header.xreal, header.yreal);

	setComment("Bias", NVBPhysValue(QString("%1 V").arg(header.bias)));
	setComment("Setpoint", NVBPhysValue(QString("%1 nA").arg(header.reference_value)));

	getMinMax();

//	setColorModel(new NVBGrayRampContColorModel(0,1,cits_header.current0,cits_header.current64));
	setColorModel(new NVBGrayRampContColorModel(0, 1, zMin, zMax));
	}

QList< NVBDataSource * > WinSPMCITSPage::slices(QFile & file, const TWinSPM::Header & header) {
	QList< NVBDataSource * > pages;

	file.seek(0x9900); // It's just there -- I don't know why
	TWinSPM::CITS_Header tcits_header;
	getCITSHeader(tcits_header, file);

	quint16 * raw_data = (quint16*)malloc(128 * 128 * 2);

	quint32 cio = 0x9A86;//file.pos();

	for (int i = 0; i < tcits_header.n_of_cits; i++) {

		file.seek(cio + i * 165);

		TWinSPM::CITS_ImageHeader img_header;
		getCITSImageHeader(img_header, file);
//		xs << img_header.voltage;

		file.seek(cio + tcits_header.n_of_cits * 165 + i * 128 * 128 * 2);
		file.read((char*)raw_data, 128 * 128 * 2);

		scaler<quint16, double> data_scaler(img_header.current0, (img_header.current64 - img_header.current0) / 65536);

		double * scaled_data = (double*)malloc(128 * 128 * sizeof(double));

		scaleMem<quint16, double>(scaled_data, data_scaler, raw_data, 128 * 128);

		pages << new WinSPMCITSSlicePage(header, img_header, scaled_data);

		}

	free(raw_data);

	return pages;

	}

Q_EXPORT_PLUGIN2(winspm, WinSPMFileGenerator)