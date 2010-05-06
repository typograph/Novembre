/***************************************************************************
 *   Copyright (C) 2006 by Timofey Balashov   *
 *   Timofey.Balashov@pi.uka.de   *
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
 ***************************************************************************/

#ifndef WINSPM_H
#define WINSPM_H

//#include <stdlib.h>
//#include <stdio.h>

//#include "mychar.h"

#include <QtPlugin>
#include <QFile>
#include "NVBFileGenerator.h"
#include "NVBPages.h"

using namespace NVBErrorCodes;

const char TIFF_HEADER[] = {
  0x49, 0x49, 0x2A, 0x00, 0xD4, 0x17, 0x00, 0x00
  };

namespace TIFF_DATA_TYPE {

enum {
  BYTE = 1,
  ASCII = 2,
  TWO_BYTES = 3,
  FOUR_BYTES = 4,
  RATIONAL = 5 // 8 bytes. First 4 -- numerator, next 4 -- denominator
};

}

struct TIFF_tag {
  qint16 tag_type;
  qint16 data_type;
  qint32 data_length;
  qint32 data; // Data or pointer
};

enum TIFF_TAGS {
  TT_NO_TAG = 0,
  TT_IMAGE_WIDTH = 256,
  TT_IMAGE_HEIGHT = 257,
  TT_BITS_PER_SAMPLE = 258,
  TT_COMPRESSION = 259,
//  TT_ = 260  
//  TT_ = 261
  TT_PHOTOMETRIC_INTERPRETATION = 262,
//  TT_ = 263  
//  TT_ = 264
//  TT_ = 265  
//  TT_ = 266
//  TT_ = 267  
//  TT_ = 268
//  TT_ = 269
  TT_IMAGE_DESCRIPTION = 270,
  TT_MAKE = 271,
  TT_MODEL = 272,
  TT_STRIP_OFFSET = 273,
//  TT_ = 274
//  TT_ = 275
//  TT_ = 276
  TT_SAMPLES_PER_PIXEL = 277,
  TT_ROWS_PER_STRIP = 278,
  TT_STRIP_BYTE_COUNT = 279,
//  TT_ = 280
//  TT_ = 281
  TT_X_RESOLUTION = 282,
  TT_Y_RESOLUTION = 283,
  
  TT_RESOLUTION_UNIT = 296,
  
//  TT_SOFTWARE = 305,  
//  TT_DATETIME = 306,

//  TT_ARTIST = 315, 
  TT_HOST_COMPUTER = 316,
  
  TT_COLOR_MAP = 320
};

namespace TWinSPM {


struct Date {
  quint8 day; // 1 to 31
  quint8 month; // 1 to 12
  quint16 year; // 1980 to 2099
  quint8 weekday; // 0 (sunday) to 6 (saturday)
};

struct Time {
  quint8 hour; // 0 to 23
  quint8 minute; // 0 to 59
  quint8 second; // 0 to 59
  quint8 centisecond; // 0 to 99
};

struct FileTime {
  quint8 unknown[8];
};

struct ImageExType { // Strcture to recor range selected from the image
  qint16 x1;
  qint16 x2;
  qint16 y1;
  qint16 y2;
};

struct CITSImageExType { // Strcture to recor range selected from the image
  quint8 adc_source; // ADC source
  qint16 adc_offset; // ADC offset (mV)
  quint16 adc_gain;  // ADC gain
  quint16 head_amp_gain; // Head amp gain
};

union ExtraType {
  ImageExType image_selection; // Image selection range
  CITSImageExType cits; // CITS measurement parameters
};

enum ImageType {
    WinSPM_IMAGE     = 1,
    WinSPM_SPECTRUM  = 2,
    WinSPM_PROFILE   = 3,
    WinSPM_HISTOGRAM = 4,
    WinSPM_VCO       = 5,
    WinSPM_INVALID   = 6
};

enum DataSourceType{
    DATA_SOURCE_Z                 = 1,
    DATA_SOURCE_LOG_I             = 2,
    DATA_SOURCE_LIN_I             = 3,
    DATA_SOURCE_AUX1              = 4,
    DATA_SOURCE_AUX2              = 5,
    DATA_SOURCE_AUX3              = 6,
    DATA_SOURCE_EXT_VOLTAGE       = 7,
    DATA_SOURCE_FORCE             = 8,
    DATA_SOURCE_AFM               = 9,
    DATA_SOURCE_FRICTION          = 10,
    DATA_SOURCE_PHASE             = 11,
    DATA_SOURCE_MFM               = 12,
    DATA_SOURCE_ELASTICITY        = 13,
    DATA_SOURCE_VISCOSITY         = 14,
    DATA_SOURCE_FFM_FRICTION      = 15,
    DATA_SOURCE_SURFACE_V         = 16,
    DATA_SOURCE_PRESCAN           = 17,
    DATA_SOURCE_RMS               = 18,
    DATA_SOURCE_FMD               = 19,
    DATA_SOURCE_CAPACITANCE_FORCE = 20
};

enum DisplayModeType {
    DISPLAY_DEFAULT = 1,
    DISPLAY_BMP     = 2,
    DISPLAY_3D      = 3
};

enum ModeType {
    MODE_LINE_1024    = 1,
    MODE_TOPO_MIRROR  = 2,
    MODE_TOPO_512     = 3,
    MODE_TOPO_256     = 4,
    MODE_TOPO_128     = 5,
    MODE_LINE_512     = 6,
    MODE_LINE_256     = 7,
    MODE_LINE_128     = 8,
    MODE_TOPO_X2      = 9,
    MODE_TOPO_X4      = 10,
    MODE_CITS         = 11,
    MODE_I_V          = 12,
    MODE_S_V          = 13,
    MODE_I_S          = 14,
    MODE_F_C          = 15,
    MODE_FFC          = 16,
    MODE_MONTAGE_128  = 17,
    MODE_MONTAGE_256  = 18,
    MODE_LSTS         = 19,
    MODE_TOPO_SPS     = 20,
    MODE_VCO          = 21,
    MODE_TOPO_IMAGE   = 22,
    MODE_TOPO3_VE_AFM = 23,
    MODE_TOPO4_MFM    = 24,
    MODE_TOPO3_LM_FFM = 25,
    MODE_TOPO2_FKM    = 26,
    MODE_TOPO2_FFM    = 27,
    MODE_TOPO_1204    = 28,
    MODE_TOPO_2X512   = 29,
    MODE_TOPO2_SCFM   = 30,
    MODE_TOPO2_MFM_1  = 31,
    MODE_TOPO64       = 32,
    MODE_PHASE_SHIFT  = 40,
    MODE_MANIPULATION = 40,
    MODE_CS3D_SCAN    = 50,
    MODE_F_V          = 60,
    MODE_SOFTWARE_GEN = 70
};

enum AFMMode {
    AFM_MODE_CONTACT = 1 << 0,
    AFM_MODE_SLOPE   = 1 << 1,
    AFM_MODE_FM      = 1 << 2,
    AFM_MODE_FMS     = 1 << 3,
    AFM_MODE_PHASE   = 1 << 4
};

enum SPMMode {
    SPM_MODE_NORMAL   = 1 << 0,
    SPM_MODE_VE_AFM   = 1 << 1,
    SPM_MODE_LM_AFM   = 1 << 2,
    SPM_MODE_KFM      = 1 << 3,
    SPM_MODE_MFM      = 1 << 4,
    SPM_MODE_MFM_LINE = 1 << 5,
    SPM_MODE_P_LIFT   = 1 << 6,
    SPM_MODE_L_LIFT   = 1 << 7,
    SPM_MODE_SCFM     = 1 << 8
};

enum MeasurementSignal {
    MEASUREMENT_SIGNAL_TOPOGRAPHY     = 1 << 0,
    MEASUREMENT_SIGNAL_BIAS           = 1 << 1,
    MEASUREMENT_SIGNAL_LINEAR_CURRENT = 1 << 2,
    MEASUREMENT_SIGNAL_LOG_CURRENT    = 1 << 3,
    MEASUREMENT_SIGNAL_FORCE          = 1 << 4,
    MEASUREMENT_SIGNAL_FRICTION_FORCE = 1 << 5,
    MEASUREMENT_SIGNAL_SUM            = 1 << 6,
    MEASUREMENT_SIGNAL_RMS            = 1 << 7,
    MEASUREMENT_SIGNAL_FMD            = 1 << 8,
    MEASUREMENT_SIGNAL_PHASE          = 1 << 9,
    MEASUREMENT_SIGNAL_AUX1           = 1 << 10,
    MEASUREMENT_SIGNAL_AUX2           = 1 << 11,
    MEASUREMENT_SIGNAL_AFM_CONTACT    = 1 << 12,
    MEASUREMENT_SIGNAL_MOTOR_X_20     = 1 << 13,
    MEASUREMENT_SIGNAL_MOTOR_Y_20     = 1 << 14,
    MEASUREMENT_SIGNAL_MOTOR_Z_20     = 1 << 15,
    MEASUREMENT_SIGNAL_AMB_APB        = 1 << 16,
    MEASUREMENT_SIGNAL_AFM            = 1 << 17,
    MEASUREMENT_SIGNAL_PRESCAN        = 1 << 18,
    MEASUREMENT_SIGNAL_LATERAL_FORCE  = 1 << 19,
    MEASUREMENT_SIGNAL_CMD_CPD        = 1 << 20,
    MEASUREMENT_SIGNAL_NONE           = 1 << 21
};

struct CalibType {
  float dummy1[4];
  float proximity_z; // Prozimity coefficient Z term    
  float dummy2[4];
  float ref_voltage; // Reference volatage (V)
  qint8 stage; // Stage, whatever it is
};

struct SPMParam{
    float clock; // Clock (ms)
    float rotation; // Measurement rotation angle
    float feedback_filter; // Feedback filter (Hz)
    float present_filter; // Present filter (kHz)
    float head_amp_gain; // Head amp gain (V/nA)
    qint16 loop_gain; // Loop gain (1 to 4)
    float x_off; // X offset (nm)
    float y_off; // Y offset (nm)
    float z_gain;// Z gain
    float z_off; // Z offset (V)
    float o_gain;// O Gain
    float o_off; // O offset (V)
    float back_scan_bias; // Back scan bias (V)
    ModeType mode;    // Neasurement mode
    qint16 dummy;
};

enum ActiveDialog {
  ACTIVEDIALOG_ADVANCED = 1 << 0,
  ACTIVEDIALOG_STM = 1 << 1,
  ACTIVEDIALOG_AC_AFM = 1 << 2,
  ACTIVEDIALOG_CONTACT_AFM = 1 << 3,
  ACTIVEDIALOG_OPTION = 1 << 4,
  ACTIVEDIALOG_SKPM = 1 << 5,
  ACTIVEDIALOG_MFM = 1 << 6,
  ACTIVEDIALOG_VE_AFM = 1 << 7,
  ACTIVEDIALOG_LM_FFM = 1 << 8,
  ACTIVEDIALOG_MAINTENANCE_1 = 1 << 9,
  ACTIVEDIALOG_MAINTENANCE_2 = 1 << 10,
  ACTIVEDIALOG_NC_AFM = 1 << 11,
  ACTIVEDIALOG_Z_STAGE = 1 << 12
};

/*
enum VCOFMDynamicRangeType {
};
*/
#define VCOFMDynamicRangeType qint32

struct SPMParam1 {
    qint32 dummy[2];
    float dds_frequency; // DDS frequency
    qint16 dds_low_filter;  // DDS low filter
    qint16 dds_high_filter; // DDS high filter
    qint16 dds_center_filter; // DDS center filter
    qint16 dds_enable; // boolean
    qint16 scan_filter; // Scan filter
    AFMMode afm_mode;
    qint32 slope_gain;
    qint16 x_addition_signal;
    qint16 y_addition_signal;
    qint16 z_addition_signal;
    qint16 bias_addition_signal;
    ActiveDialog active_dialog;
    SPMMode spm_mode;
    MeasurementSignal measurement_signal;
    qint16 phase_vco_scan; // Phase during VCO scan
    ModeType sps_mode;
    double dds_amplitude;
    double dds_center_locked_freq;
    float dds_phase_shift;
    qint16 dds_high_gain;
    qint16 dds_phase_polarity;
    qint16 dds_pll_excitation;
    qint16 dds_external;
    qint32 dds_rms_filter;
    qint32 dds_pll_loop_gain;
    qint32 dds_beat_noise;
    VCOFMDynamicRangeType dds_dynamic_range;
    float cantilever_peak_freq;
    float cantilever_q_factor;
    char buffer[10]; // Buffer area
};

struct Point {
  qint16 x, y;
};

struct ColorRef { //###
  qint32 unknown;
};

struct Rect {  //###
  qint32 unknown[4];
};

enum DialogControlSttsType {
  ENABLE = 1,
  DISABLE = 2,
  INVISIBLE = 3
};

struct ProfileDef {
  Point line[5][2]; // Coordinates of the line
  float marker[3][2]; // Location of the marker
  qint16 dummy;
  qint16 line_ind[5]; // Line indication
  qint16 marker_ind[3]; // Marker indication
  qint16 act_line[5]; // Active line
  Point single[2]; // Coordinates of Single Profile to be recorded 
  Point multi[2]; // Coordinates of Multi Profile to be recorded 
  ColorRef colors[5]; // Color of each line
  qint16 rec_marker_ind[3]; //Indication of marker to be recorded
  bool rz_marker_indicated; // Rz marker indicated / not indicated
  bool broad_line[5]; // Broad line setting
  qint16 broad_line_width[5] ; // Broad line width
  Rect area; // Selected area for roughness analysis
  qint16 area_setting; // Setting of selected area for roughness analysis
  qint16 range_x; // amount of movement by cursor (X)
  qint16 range_y; // amount of movement by cursor (Y)
  Point extra[2]; // Coordinates of Extra Profile to be recorded 
  bool broad_line_single;   // Broad line setting (Single Profile)
  bool broad_line_extra;    // Broad line setting (Extra Profile)
  bool broad_line_multi[5]; // Broad line setting (Multi Profile)
  qint16 broad_line_width_single;   // Broad line width (Single Profile)
  qint16 broad_line_width_extra;    // Broad line width (Extra Profile)
  qint16 broad_line_width_multi[5]; // Broad line width (Multi Profile)
  qint16 profile_vertical; // Direction of extra profile (false -- horizontal)
  qint16 rz_onoff_state; // Application of "Rz on/off" menu. Enum DialogControlSttsType
  char buffer[49]; // buffer
};

enum ThreeDDisplayMode {
  THREEDDISPLAYMODE_INVALID = 0,
  THREEDDISPLAYMODE_ILLUMINATE = 1,
  THREEDDISPLAYMODE_SHADE = 2,
  THREEDDISPLAYMODE_GRID = 4,
  THREEDDISPLAYMODE_MERGE = 8,
  THREEDDISPLAYMODE_SHADE_ILLUMINATED = 0x10,
  THREEDDISPLAYMODE_ALPHA = 0x20,
};

struct ThreeDSettings {
  ThreeDDisplayMode display_mode;
  float x_angle; // X rotation angle
  float y_angle; // Y rotation angle
  float z_angle; // Z rotation angle
  float z_scale;
  float z_offset;
  float xyz_scale;
  float x_shift;
  float y_shift;
  float perspective;
  float mesh_size_x;
  float mesh_size_y;
  float dispersion; // Specular level
  float reflection; // Diffuse level
  float brightness; // Reflectivity
  qint16 surface_color[2][3]; // Surface display color (2 sheets)
  float light_azimuth[2];
  float light_elevation[2];
  float light_brightness[2];
  qint16 display_x_scale;
  qint16 display_y_scale;
  qint16 display_z_scale;
  qint16 display_side;
  qint16 display_base;
  qint16 display_light; // Display light source
  qint8 scale_font[60]; // Scale indication font ### Unknown format
  ColorRef scale_color;
  qint16 display_3d; // true
  float xy_ratio; // XY aspect ratio
  ColorRef bg_color; // Background color
  float scale_font_size; // Scale indication font size
  float title_font_size; // Title indication font size
  float alpha; // Alpha value
  qint16 titlepath_ind[3]; // Indication of title scan size path ### ???
  char buffer[246];
  
};

struct Extract {
  qint8 unknown[20]; // ###
}; 

enum LIAOutput {
  LIAOUTTYPE_ACOS_ASIN = 1,
  LIAOUTTYPE_AMPL_PHASE = 2
};

enum LIAFilter {
  LIAFILTERTYPE_FLAT = 1,
  LIAFILTERTYPE_LOWPASS = 2,
  LIAFILTERTYPE_HIGHPASS = 3
};

enum LIALineReject {
  LIALINEREJECT_OFF = 1,
  LIALINEREJECT_F = 2,
  LIALINEREJECT_2F = 3,
  LIALINEREJECT_F_2F = 4
};

enum LIARefSource {
  LIAREF_INT = 1,
  LIAREF_TTL = 2,
  LIAREF_EXT = 3
};

enum LIADynRes {
  LIADYN_NORMAL = 1,
  LIADYN_HIGH_RES = 2,
  LIADYN_HIGH_STABILITY = 3
};

struct LIAParam {
  qint16 model; // Lock-in amplifier model (5110/7265)
  qint16 lia_on; // Lock-in amplifier on/off
  qint32 lia_onoff_state; // Control indicator setting. Enum DialogControlSttsType  
  double sensitivity; // Input sensitivity
  double int_time; // Time constant
  double phase; // Reference phase
  double frequency; // Modulation signal frequency
  double modulation; // Modulation signal amplitude
  qint32 output_type; // Kind of output signal. enum LIAOutput
  qint32 filter_type; // Kind of filter. enum LIAFilter
  double filter_frequency; // Filter frequency
  qint32 linereject_type; // Kind of line reject. enum LIALineReject
  qint16 track_on; // Track on/off
  qint32 refsignal_type; // Kind of output signal. enum LIARefSource
  qint16 ref_frequency; // For model 7265 (1F--3F), for 5110 2F ON/OFF
  qint16 slope_12db_on; // Slope/12dB on/off
  qint16 expand_on; // Expand on/off
  qint32 dynres_type; // Dynamic resolution setting . enum LIADynRes
  qint16 offset_on; // Offset on/off
  double ch1_offset; // Offset for channel ch1 (%)
  double ch2_offset; // Offset for channel ch2 (%)
  double scan_start_frequency; // Modulation signal scan start frequency
  double scan_end_frequency;   // Modulation signal scan end frequency
  double scan_peak_frequency;  // Peak frequency detected in a modulation signal scan
  qint16 scan_time; // Modulation signal scan time
  qint32 scan_result_apply; // Modulation signal scan result apply ON/OFF
};

enum SampleHolder {
  HEATING = 0,
  COOLING = 1
};

enum TempUnit {
  CELSIUS = 0,
  FAHRENHEIT = 1
};

struct TempParam {
  qint16 tempcont_connected; // Temperature controller connection On/Off
  qint16 sample_holder; // Kind of sample holder. Enum SampleHolder
  qint16 temp_unit; // Temperature indication setting. Enum TempUnit
  qint16 tempcont_on; // Temperature control On/Off
  float p_gain;  // P gain of temperature control (PID control)
  qint32 i_gain; // I gain of temperature control (PID control)
  qint32 d_gain; // G gain of temperature control (PID control)
  float temp_offset; // Temperature offset
  float temp_present; // Present temperature
  float temp_set; // Set temperature
};

struct AFMFixedRef {
  qint16 conversion_on; // Conversion apply On/Off
  double reference_conversion; // Cnversion value of reference (nm)/(Hz)
  double osc_apmlitude; // Cantilever oscillation amplitude (nm)
  double reference_signal; // Reference signal value at Nc-AFM
};

struct LMCantileverParam {
  qint8 unknown[62]; // ###
};

struct CITS_Header {
  qint16 winspm_version;
  qint16 n_of_cits; // Number of CITS images
  qint16 cits_width; // 128
  qint16 cits_height; // 128
  qint16 n_of_current; // Number of current images
  qint16 check_flag; // only first 3 bits have meaning
  qint16 n_display_cits; // Number of cits images to display
  quint8 disp_mode;
  double conductance0; // Conductance at level 0
  double conductance255; // Conductance at level 255
  quint8 disp_sel; // Display selection
  qint16 n_cits; // Number of cits image
  qint8 buffer[100];
};

struct CITS_ImageHeader{
  qint16 adc_min;
  qint16 adc_max;
  quint8 adc_source; // unused
  qint16 adc_offset;
  quint16 adc_gain;
  qint32 head_amp_gain;
  float voltage; // Image voltage
  float current0; // Current at image data=0
  float current64; // Current at image data = 64 ???
  char title[40]; // CITS Image title
  MeasurementSignal source_type;
  char buffer[96];
};

struct Header{
    quint16 winspm_version; // 100 * version. Is 405 for 4.05
    char internal_filename_b4[80]; // Internal filename in ver. 4.00 or older
    quint16 xres; // Image width : 128, 256 or 512 according to v2.12
    quint16 yres; // Image height : 128, 256 or 512 according to v2.12
    float xreal;  // Scan width in nm
    float yreal;  // Scan height in nm
    float z0;     // Real data at pixel data 0 (nm or nA)
    float z255;   // Real data at pixel data 255 (nm or nA)
    qint16 adc_min; // A/D converter minimum value during scan
    qint16 adc_max; // A/D converter maximum value during scan
    float initial_scan_scale; // Scale value during initial scan
    char internal_filename[40]; // Image internal filename
    char info[5][40]; // Image information (EOL CR)
    uchar history[50]; // History of image processing ### How do we implement that?
    qint16 has_current_info; // With current/voltage info : 1, without it : 0
    float bias; // Bias voltage (V)
    float reference_value; // Reverence value (nA or V)
    qint16 reserved1; // Dummy data (0x0000)
    Date measurement_date; // Measured on that day
    Date save_date;        // Measured on that day
    Time measurement_time; // Measured at that time
    Time save_time;        // Measured at that time
    char lookup_table[256];       // Numbers from 0x00 to 0xFF
    quint32 fft_offset;     // Offset of FFT data (0 if no data)
    quint16 transform_off;  // Transform off ### What's that?
    ExtraType selected_region; // Region selected from image
    bool compressed; // Compressed / not compressed image
    qint8 bpp; // Bits per pixel (0 = 8 bits)
    quint32 cits_offset;  // Pointer to CITS data
    float backscan_tip_voltage; // Tip voltage during backscan (MAX_FLOAT if no setting)
    qint16 sts_point_x; // STS measurement point X (or -1,-1) in the image
    qint16 sts_point_y; // STS measurement point Y (or -1,-1) in the image
    uchar reserved2[10]; // (^010)
    uchar reserved3[10]; // (^010)
    float tip_speed_x; // Tip speed X (nm/s) during image measurement
    float tip_speed_y; // Tip speed Y (nm/s) during image measurement
    CalibType piezo_sensitivity; // Sensitivity setting for piezoelectric element
    SPMParam spm_param; // Detailed SPM parameters
    quint32 montage_offset; // Montage offset
    char image_location[260]; // Full path specification
    SPMParam1 spm_misc_param; // Miscellaneous SPM parameters
    uchar red_lookup[256]; // Red lookup table
    uchar green_lookup[256]; // Green lookup table
    uchar blue_lookup[256]; // Blue lookup table
    qint32 sub_revision_no; // Subrevision number
    ImageType image_type;
    DataSourceType data_source;
    DisplayModeType display_mode;
    FileTime measurement_start_time;
    FileTime measurement_end_time;
    ProfileDef profile_roughness;
    ThreeDSettings settings_3d; // 3D setting
    float lut_brightness; // LUT Brightness
    float lut_contrast;   // LUT Contrast
    qint16 software_version; // Software version used for measurement
    qint32 software_subversion; // Software sub-version used for measurement
    Extract extracted_region; // Region extracted from the image
    float lut_gamma; // LUT gamma
    qint16 n_of_sps; // Number of SPS data saved together
    quint32 sps_offset; // Pointer to SPS data
    qint16 line_trace_end_x; // End point X for line trace measurement
    qint16 line_trace_end_y; // End point Y for line trace measurement
    qint16 forward; // Forward/backward identification flag
    qint16 lift_signal; // Lift signal identification flag
    LIAParam lia_settings; // Lock-in amplifier settings
    TempParam temp_param; // Temperature controller setting parameters
    AFMFixedRef converted_afm; // COnverted values for AFM signal
    LMCantileverParam special_measurement_param; // Special mesurement parameter
    
    /* After all that there's a 3060 bytes buffer */
    
};

}

class WinSPMFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

private:
//   static NVBDataSource * loadNextPage(QFile * const file);
  static void getWinSPMHeader(TWinSPM::Header &header, QFile * const file);
//   static QStringList loadWinSPMStrings(QFile * const file, qint16 nstrings);
//   static QString getPageTypeString(qint32 type);
//   static QString getGUIDString(GUID id);
//   static QString getLineTypeString(qint32 type);
//   static QString getSourceTypeString(qint32 type);
//   static QString getDirectionString(qint32 type);
//   static QString getImageTypeString(qint32 type);

  friend class WinSPMTopoPage;
  friend class WinSPMSpecPage;

public:
  WinSPMFileGenerator():NVBFileGenerator() {;}
  virtual ~WinSPMFileGenerator() {;}

  virtual inline QString moduleName() { return QString("WinSPM files");}
  virtual inline QString moduleDesc() { return QString("WinSPM STM file format, as specified in manual for v.2.12"); }

  virtual inline QString nameFilter() { return QString("WinSPM files (*.tif)"); } // double semicolon as separator
  virtual inline QString extFilter() { return QString("*.tif;*.TIF;*.tiff;*.TIFF"); }; // single semicolon as separator

  virtual bool canLoadFile(QString filename);
  virtual NVBFileStruct * loadFile(QString filename);
  virtual NVBFileInfo * loadFileInfo(QString filename);

};

/*
class WinSPMFile : public NVBFile {
public:
  WinSPMFile(QString filename);
  virtual ~WinSPMFile() {;}
};
*/

class WinSPMTopoPage : public NVB3DPage {
Q_OBJECT
private:
  TWinSPM::Header header;
public:
  WinSPMTopoPage(QFile * const file, const TWinSPM::Header & _header, quint32 data_offset, quint32 color_offset = 0);
  virtual ~WinSPMTopoPage() {;}
public slots:
  virtual void commit() {;}
};

class WinSPMSpecPage : public NVBSpecPage {
Q_OBJECT
protected:
  TWinSPM::Header theader;
public:
  WinSPMSpecPage(const TWinSPM::Header & topo_header);
  WinSPMSpecPage(QFile * const file, const TWinSPM::Header & topo_header, const TWinSPM::Header & _header, quint32 data_offset);
  virtual ~WinSPMSpecPage() {;}
  
  void addCurve(QFile * const file, const TWinSPM::Header & _header, quint32 data_offset);
public slots:
  virtual void commit() {;}
};

class WinSPMCITSSlicePage;

class WinSPMCITSPage : public NVBSpecPage {
Q_OBJECT
protected:
  TWinSPM::Header header;
  TWinSPM::CITS_Header cits_header;
  
  static void getCITSHeader(TWinSPM::CITS_Header &header, QFile * const file);
  static void getCITSImageHeader(TWinSPM::CITS_ImageHeader &header, QFile * const file);
public:
  WinSPMCITSPage(QFile * const file, const TWinSPM::Header & _header);
  virtual ~WinSPMCITSPage() {;}
  
  static QList<NVBDataSource*> slices(QFile * const file, const TWinSPM::Header & _header);
public slots:
  virtual void commit() {;}
};

class WinSPMCITSSlicePage : public NVB3DPage {
Q_OBJECT
private:
  TWinSPM::Header header;
public:
  WinSPMCITSSlicePage(const TWinSPM::Header & _header, const TWinSPM::CITS_ImageHeader & cits_header, double * _data);
  virtual ~WinSPMCITSSlicePage() {;}
public slots:
  virtual void commit() {;}
};


#endif
