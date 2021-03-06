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

#ifndef RHK_H
#define RHK_H

#include <QtCore/QFile>
#include "NVBUnits.h"
#include "NVBFileGenerator.h"
// #include "NVBLogger.h"

/*GUID*/
struct RHK_GUID {
	quint32 Data1;
	quint16 Data2;
	quint16 Data3;
	quint64 Data4;
	};

/*TRHKHEADER*/
struct TRHKHeader {
	qint16 parameter_size;
	quint16 version[18];
	qint16 string_count;
	qint32 type;
	qint32 page_type;
	qint32 data_sub_source;
	qint32 line_type;
	qint32 x_coordinate;
	qint32 y_coordinate;
	qint32 x_size;
	qint32 y_size;
	qint32 source_type;
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
	RHK_GUID page_ID;
	qint32 colorinfo_count;
	qint32 grid_xsize;
	qint32 grid_ysize;
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
	quint32 invert; // bool according to spec - 4 bytes long
	};

/*TCOLORINFO*/
struct TRHKColorInfo {
	qint16 parameter_count;
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
	};

// using namespace NVBErrorCodes;

class NVBDataSource;

class RHKFileGenerator: public QObject, public NVBFileGenerator {
		Q_OBJECT
// 		Q_INTERFACES(NVBFileGenerator)

	private:

		bool subtractBias;

		static TRHKHeader getRHKHeader(QFile & file);
		static QStringList loadRHKStrings(QFile & file, qint16 nstrings);
		static QString getPageTypeString(qint32 type);
		static QString getGUIDString(RHK_GUID id);
		static QString getLineTypeString(qint32 type);
		static QString getSourceTypeString(qint32 type);
		static QString getDirectionString(qint32 type);
		static QString getImageTypeString(qint32 type);

		static bool RHKHeaderIsSane(const TRHKHeader &, QString);

		static bool loadNextPage(QFile & file, NVBFile * sources, bool subtract_bias );
		static bool loadTopoPage(QFile & file, NVBFile * sources );
		static bool loadSpecPage(QFile & file, NVBFile * sources, bool subtract_bias );
		static void CommentsFromString(NVBDataComments & comments, const QStringList & strings);
		static void CommentsFromHeader(NVBDataComments & comments, const TRHKHeader & header);

		static void detectGrid(const TRHKHeader& header, const float* xposdata, const float* yposdata, int& np, int& nx, int& ny);

		static QList<QPointF> pointsFromXY(int length, float * x, float * y);

	protected slots:
		void loadSettings();

	public:
		RHKFileGenerator( NVBSettings settings );
		virtual ~RHKFileGenerator() {;}

		virtual inline QString moduleName() const { return QString("RHK XPMPro files");}
		virtual inline QString moduleDesc() const { return QString("RHK Technology STM file format. Works for SM3 files"); }

#ifndef FILEGENERATOR_NO_GUI
		virtual NVBSettingsWidget* configurationPage() const;
#endif

		virtual inline QStringList extFilters() const {
			static QStringList exts = QStringList() << "*.SM3" ; // FIXME << "*.SM2" << "*.SM4";
			return exts;
			}

		virtual NVBFile * loadFile(const NVBAssociatedFilesInfo & info) const throw();
		virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

		virtual QStringList availableInfoFields() const;

		//- Using super's method, since RHK only uses one file per measurement.
		// virtual inline NVBAssociatedFilesInfo associatedFiles(QString filename) const;

	};

#endif
