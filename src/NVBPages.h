//
// C++ Interface: NVBPages
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBPAGES_H
#define NVBPAGES_H

#include "NVBLogger.h"
#include "NVBDataSource.h"

using namespace NVBColoring;
using namespace NVBErrorCodes;

/**
\class NVB3DPage
Implementation of NVB3DDataSource for real data
*/
class NVB3DPage : public NVB3DDataSource
{
Q_OBJECT
protected:
  NVBContColorModel* colors;
  double * data;
  /// Fills in \a zMin and \a zMax
  virtual void getMinMax();
  QSize _resolution;
  QRectF _position;
  NVBDimension xd,yd,zd;

  /// The name of the source
  QString pagename;
  /// The name of the source file
  QString file;

public:
  NVB3DPage():NVB3DDataSource(),colors(0),data(0) { getMinMax(); }
  virtual ~NVB3DPage();

  bool setComment(const QString & key, const NVBVariant & value) {
    comments.insert(key,value);
    emit commentsChanged();
    return true;
    }

  /// \returns the name of the source
  virtual inline QString name() const { return pagename;}
  /// \returns the name of the file the data originates from
  virtual inline QString fileName() const { return file;}
  /// Overrides filename
  virtual inline void setFileName(QString newfile) { file = newfile; }

  virtual inline NVBDimension xDim() const { return xd; }
  virtual inline NVBDimension yDim() const { return yd; }
  virtual inline NVBDimension zDim() const { return zd; }

  virtual inline QSize resolution() const { return _resolution; }
  virtual inline QRectF position() const { return _position; }

  virtual inline const NVBContColorModel * getColorModel() const { return colors; }
  virtual inline bool canSetColorModel() const { return true; }
  /// \returns \b true if the color model was successfully set to \a colorModel .
  /// If \a colorModel is 0, nothing is changed.
  virtual bool setColorModel(NVBContColorModel * colorModel);

  virtual inline const double * getData() const { return data; }
  virtual inline double getData(int x, int y) const { return data[x+y*_resolution.width()]; }
  virtual inline bool canSetData() const { return true; }
  /// \returns \b true if data was successfully set to \a newdata .
  /// If \a newdata is 0, nothing is changed.
  virtual bool setData(double * newdata);

};

/**
\class NVBSpecPage
Implementation of NVBPage for spectroscopy pages
*/
class NVBSpecPage : public NVBSpecDataSource
{
Q_OBJECT
protected:
  /// Fills in \a zMin and \a zMax
  virtual void getMinMax();
  QList<QPointF> _positions;
  NVBDiscrColorModel * colors;
  QList<QwtData*> _data;
  QSize _datasize;
  NVBDimension xd,yd,zd,td;

  /// The name of the source
  QString pagename;
  /// The name of the source file
  QString file;

public:
  NVBSpecPage():NVBSpecDataSource(),colors(0) { getMinMax(); }
  virtual ~NVBSpecPage();

  bool setComment(const QString & key, const NVBVariant & value) {
    comments.insert(key,value);
    emit commentsChanged();
    return true;
    }

  /// \returns the name of the source
  virtual inline QString name() const { return pagename;}
  /// \returns the name of the file the data originates from
  virtual inline QString fileName() const { return file;}
  /// Overrides filename
  virtual inline void setFileName(QString newfile) { file = newfile; }

  virtual inline NVBDimension xDim() const { return xd; }
  virtual inline NVBDimension yDim() const { return yd; }
  virtual inline NVBDimension zDim() const { return zd; }
  virtual inline NVBDimension tDim() const { return td; }

  virtual inline QSize datasize() const {return _datasize;}

  virtual inline const NVBDiscrColorModel * getColorModel() const { return colors; }
  virtual inline bool canSetColorModel() const { return true; }
  /// \returns \b true if the color model was successfully set to \a colorModel . If \a colorModel is 0, nothing is changed.
  virtual bool setColorModel( NVBDiscrColorModel * colorModel );

  virtual inline QList<QPointF> positions() const { return _positions; }

  virtual inline QList<QwtData*> getData() const {return _data;}
  virtual inline bool canSetData() const { return true; }
  /// \returns \b true if data was successfully set to \a data . If \a data is empty, nothing is changed.
  virtual bool setData( QList<QwtData*> data );
  /// \returns \b true if data for curve \a y was successfully set to \a data . If \a data is 0, nothing is changed.
  virtual bool setData( int y, QwtData * data );

};

/**
  \struct NVBFileStruct\
  A base structure for a file. It exist in order not to encumber file plugins with NVBFile
*/
struct NVBFileStruct {
  /// Constructs an empty file
  NVBFileStruct() {;}
  /// Constructs an empty file with name \a f
  NVBFileStruct(QString f):filename(f) {;}
  ~NVBFileStruct() {;}
  /// The name of the file
  QString filename;
  /// The list of pages
  QList<NVBDataSource*> pages;
};

#endif
