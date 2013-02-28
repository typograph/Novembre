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
#ifndef RHK4_H
#define RHK4_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#define RHK_TOPOPAGE 0
#define RHK_SPECPAGE 1
#define RHK_UNKPAGE 2
#define RHK_ANNOTATEDSPECPAGE 3

#include <QtPlugin>
#include <QFile>
#include "dimension.h"
#include "NVBFileGenerator.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
// #include "NVBLogger.h"
#include "NVBPages.h"


struct MS_FILETIME {
	quint32 dwLowDateTime;
	quint32 dwHighDateTime;
	};

typedef struct {
	quint32 Data1;
	quint16 Data2;
	quint16 Data3;
	quint64 Data4;
	} RHK_GUID;

struct RHKObject {
	quint32 id;
	quint32 offset;
	quint32 data_size;
	RHKObject() { memset((char*)this, 0, sizeof(*this)); }
	};

struct RHKFileHeader {
	quint16 version[18];
	quint32 page_count;
	quint32 object_list_count;
	quint32 object_field_size;
	quint32 reserved[2];
	RHKObject * object_list; // as of rev4, 3 objects

	RHKFileHeader() { memset((char*)this, 0, sizeof(*this));}
	};

struct RHKPageHeader {
	quint16 field_size;
	quint16 string_count;

//	quint32 type;
	quint32 page_type;
	quint32 data_sub_source;
	quint32 line_type;

	qint32 x_coordinate;
	qint32 y_coordinate;
	qint32 x_size;
	qint32 y_size;

//  qint32 source_type;
	qint32 image_type;
	qint32 scan;
	qint32 group_ID;
	quint32 page_data_size;

	qint32 z_min;
	qint32 z_max;
	float x_scale;
	float y_scale;
	float z_scale;
	float xy_scale;
	float x_offset;
	float y_offset;
	float z_offset;
	float period;
	float bias;
	float current;
	float angle;

	qint32 colorinfo_count;
	qint32 grid_xsize;
	qint32 grid_ysize;

	quint32 object_list_count;
	quint8 wide_data_flag; // 32bits, from rev2
	quint8 reserved_flags[3];
	quint8 reserved[60];

	RHKObject * object_list;

	RHKPageHeader() { memset((char*)this, 0, sizeof(*this)); }
	};

struct RHKPageIndex {
	RHK_GUID page_ID;
	quint32 page_data_type;
	quint32 page_source_type;
	quint32 object_list_count;
	quint32 minor_version;
	RHKObject * object_list;
	RHKPageHeader page_header;

	RHKPageIndex() { memset((char*)this, 0, sizeof(*this));}
	};

struct RHKPageIndexHeader {
	quint32 page_count;
	quint32 object_list_count;
	quint32 reserved[2];
	RHKObject * object_list; // as of rev4, 1 objects
	RHKPageIndex * page_index_array;

	RHKPageIndexHeader() { memset((char*)this, 0, sizeof(*this));}
	};

struct RHKFile {
	quint16 header_size;
	RHKFileHeader header;
	RHKPageIndexHeader page_index;
	// RHKPRMHeader // FIXME ignored
	RHKFile() { memset((char*)this, 0, sizeof(*this)); }
	};

struct RHKSequentialDataHeader {
	qint32 data_type;
	qint32 data_length;
	qint32 param_count;

	quint32 object_list_count;
	quint32 data_info_size;
	quint32 data_info_string_count;
	RHKObject * object_list;

	RHKSequentialDataHeader() { memset((char*)this, 0, sizeof(*this));}
	};


struct RHKImageDriftHeader {
	MS_FILETIME start_time;
	qint32 drift_option;
	};

struct RHKImageDrift {
	float time;
	float x_step;
	float y_step;
	float x_step_total;
	float y_step_total;
	float x_rate;
	float y_rate;
	};

struct RHKSpecDriftHeader {
	MS_FILETIME start_time;
	qint32 drift_option;
	quint32 string_count;
	QString channel;
	};

struct RHKSpecInfo {
	float time;
	float x_coordinate;
	float y_coordinate;
	float x_step;
	float y_step;
	float x_step_total;
	float y_step_total;
	};

struct RHKTipTrackInfoHeader {
	MS_FILETIME start_time;
	float feature_height;
	float feature_width;
	float time_constant;
	float cycle_rate;
	float phase_lag;
	quint32 string_count;
	quint32 tip_track_info_count;
	QString channel;
	};

struct RHKTipTrackInfo {
	float time_total;
	float time;
	float x_step;
	float y_step;
	};


struct RHKDataInfo {
	float param_gain;
	// label, unit in standard rhk length-first format
	QString label;
	QString unit;
	};

/*TRHKCOLORTRANSFORM*/
struct  TRHKColorTransform {
	float gamma;
	float alpha;
	float x_start;
	float x_stop;
	float y_start;
	float y_stop;
	qint32 mapping_mode;

	/* In Visual C++4.2, the Standard C++ header files contained a typedef that equated bool with int.
	 * In Visual C++ 5.0 and later, bool is implemented as a built-in type with a size of 1 byte.
	 * That means that for Visual C++ 4.2, a call of sizeof(bool) yields 4,
	 * while in Visual C++ 5.0 and later, the same call yields 1.
	 * This can cause memory corruption problems if you have defined structure members of type bool
	 * in Visual C++ 4.2 and are mixing object files (OBJ) and/or DLLs built with the 4.2 and 5.0
	 * or later compilers.
	 */
	// FIXME So, how big is RHK's bool?
	bool invert;
	};

struct RHKColorInfo {
	qint16 parameter_count;
	qint16 reserved;
	float start_h;
	float start_s;
	float start_b;
	float end_h;
	float end_s;
	float end_b;
	qint32 color_direction;
	qint32 color_entries;
	float start_slidepos;
	float end_slidepos;
	TRHKColorTransform transform;
	} ;

struct RHKAPIInfo { // from rev4
	float voltage_hi;
	float voltage_lo;
	float gain;
	float offset;
	qint32 ramp_mode;
	qint32 ramp_type;
	qint32 step;
	qint32 image_count;
	qint32 output_dac;
	qint32 mux;
	qint32 bias_mode;
	quint32 string_count;
	QString unit;
	};

struct RHKPRMHeader {
	quint32 compression_flag;
	quint64 orig_data_size;
	quint64 compressed_size;
	};

struct RHKThumbnailHeader {
	quint32 width;
	quint32 height;
	quint32 data_format;
	};

// using namespace NVBErrorCodes;

class NVBDataSource;

class RHK4FileGenerator: public QObject, public NVBFileGenerator {
		Q_OBJECT
		Q_INTERFACES(NVBFileGenerator);

	private:
		bool subtractBias;

		static RHKObject * loadObjectList(QFile & file, quint32 object_count);
		static QString loadRHKString(QFile & file);
		static QStringList loadRHKStrings(QFile & file, qint16 nstrings);

		static RHKFile getRHKHeader(QFile & file);
		static void destroyRHKHeader(RHKFile header);
		static QString getPageTypeString(qint32 type);
		static QString getPageDataTypeString(qint32 type);
		static QString getGUIDString(RHK_GUID id);
		static QString getLineTypeString(qint32 type);
		static QString getSourceTypeString(qint32 type);
		static QString getDirectionString(qint32 type);
		static QString getImageTypeString(qint32 type);
		static QString getObjectTypeString(qint32 type);

		friend class RHK4TopoPage;
		friend class RHK4SpecPage;

	public:
		RHK4FileGenerator();
		virtual ~RHK4FileGenerator() {;}

		virtual inline QString moduleName() const { return QString("RHK XPMPro2 files");}
		virtual inline QString moduleDesc() const { return QString("RHK Technology STM file format. Works for SM4 files"); }

		virtual inline QStringList extFilters() const {
			static QStringList exts = QStringList() << "*.SM4" ;
			return exts;
			}

//  virtual bool canLoadFile(QString filename);
		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

		virtual QStringList availableInfoFields() const;

		//- Using super's method, since RHK only uses one file per project.
		// virtual inline NVBAssociatedFilesInfo associatedFiles(QString filename) const;
	};

class RHK4TopoPage : public NVB3DPage {
		Q_OBJECT
	private:
		RHKPageHeader header;
		QStringList strings;
	public:
		RHK4TopoPage(RHKPageIndex * index, QFile & file);
		virtual ~RHK4TopoPage() {;}
	public slots:
		virtual void commit() {;}
	};

class RHK4SpecPage : public NVBSpecPage {
		Q_OBJECT
	protected:
		RHKPageHeader header;
		QStringList strings;
		double * ys;
		double * xs;
	public:
		RHK4SpecPage(RHKPageIndex * index, QFile & file, bool subtractBias = false);
		virtual ~RHK4SpecPage();
	public slots:
		virtual void commit() {;}
	};


#endif
