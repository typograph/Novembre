//
// C++ Interface: NVBFileWrappers
//
// Description: 
//
//
// Author: Timofey <timoty@pi-balashov>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NVBSOURCE_H
#define NVBSOURCE_H

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QMetaType>
#include <qwt_data.h>

#include "dimension.h"
#include "NVBVariant.h"
#include "NVBContColorModel.h"
#include "NVBDiscrColorModel.h"
#include "NVBDimension.h"
#include "NVBLogger.h"

class NVBFile;

/// \todo The roles defined here must be moved to models
/// Role for the type of the page
#define PageTypeRole (Qt::UserRole+1)
/// \b OBSOLETE: Role for the data of the page
#define PageDataRole (Qt::UserRole+2)
/// Role for the positions of the page
#define PagePositionRole (Qt::UserRole+3)
/// Role for the size of the page data
#define PageDataSizeRole (Qt::UserRole+4)
/// Role for the page as a whole
#define PageRole (Qt::UserRole+5)
/// Role for the page visualizer
#define PageVizItemRole (Qt::UserRole+200)
//#define PageAsDelegateRole (Qt::UserRole+205)

namespace NVB {

/// \enum NVB::PageType
/// Defines types of data pages in the program
enum PageType {
	/// This page type is returned by default from a NVBDataSource and indicates an invalid page. Such a page cannot be displayed.
	InvalidPage = -1,
	/// This page type is returned by default from a NVB3DDataSource and indicates a topographic page (x-y-z).
	TopoPage = 0,
	/// This page type is returned by default from a NVBSpecDataSource and indicates a spectroscopic page (t-z & x-y).
	SpecPage = 1,
	/// This page type exists for the sake of NVBPageToolbar and suchlike. The type of this page is unknown, i.e. it could be anything, \b except invalid. The page cannot be displayed
	UndefinedPage = 117,
	/// Page types up to this one are reserved by Novembre for future extensions
	/// Plugins may define new page types from \c ReservedPage+1
	ReservedPage = 128//,
//  /// Page types up to this one are reserved for Novembre plugins
//  /// Plugins may define new page types from \c UserPage+1
//  UserPage = 256
	};

}

Q_DECLARE_METATYPE(NVB::PageType);

using namespace NVBColoring;
// using namespace NVBErrorCodes;

/*!
* A set of data, i.e. 2D height array (topography) or a set of 1D arrays (spectroscopy).
* A datasource can displayed, deleted, morphed, etc.
*/
class NVBDataSource : public QObject
{
Q_OBJECT
private:
	/// Number of references to this source. Note, that creating an object uses it automatically
	unsigned int refCount;
	friend void useDataSource(NVBDataSource* source);
	friend void releaseDataSource(NVBDataSource* source);
	NVBDataSource( const NVBDataSource& );
protected:
	/// Minimal and maximal values for the z axis
	double zMin, zMax;
	/// Comments for the source
	QMap<QString,NVBVariant> comments;
public:
	/// Creates a used source
	NVBDataSource():QObject(),refCount(1),owner(0) {;}
	/// Destructor checks for stale references and prints a warning. Such a warning means something might be really wrong in the plugins used
	virtual ~NVBDataSource() {
		if (refCount)
			NVBOutputError("refCount !=0 -- data source not properly disposed of");
		}

	/// Use it for whatever you want
	void* tag;
	/// \returns the name of the source
	virtual QString name() const = 0;
	/// \returns the file the data originates from
//	virtual const NVBFile * owner() const { return file; }
	///// Here, it's not possible to just return the filename
	///// since NVBAssociatedFileInfo is kind of big...
	NVBFile * owner;



	/// \returns the type of the page
	virtual inline NVB::PageType type() const {return NVB::InvalidPage;}

	/// \returns The minimum on the Z axis
	virtual inline double getZMin() const {return zMin;}
	/// \returns the maximum on the Z axis
	virtual inline double getZMax() const {return zMax;}

	/// The X axis is the axis in real space, normally uses units of length
	/// \returns the dimension of the X axis
	virtual NVBDimension xDim() const = 0;
	/// The Y axis is the axis in real space, normally uses units of length
	/// \returns the dimension of the Y axis
	virtual NVBDimension yDim() const = 0;
	/// The Z axis is the axis in measurement space, the data axis. The units are arbitrary.
	/// \returns the dimension of the X axis
	virtual NVBDimension zDim() const = 0;

	/// \returns the comment for the given \a key
	virtual inline NVBVariant getComment(const QString& key) const { return comments.value(key); }
	/// \returns all the available comments in a QMap
	virtual inline const QMap<QString,NVBVariant>& getAllComments() const { return comments; }

//  virtual NVBDataSource* parent() {return NULL;}

public slots:
	/// \internal UNUSED. Could be used in the future to save pages
	virtual void commit() {;}
	/// emits \a objectPushed()
	virtual inline void override( NVBDataSource * newpt) { emit objectPushed(newpt,this); }

signals:
	/// Whatever data pointer was gotten out of this source will become invalid
	void dataAboutToBeChanged();
	/// Data obtained from this source has changed internally.
	/// This does not include things like resolution.
	void dataAdjusted();
	/// All data returned earlier might have changed. This signal 'includes' dataAdjusted() and colorsChanged()
	void dataChanged();
	/// Some comments were changed or added
	void commentsChanged();
	/// The reference to whatever color model obtained earlier will become invalid
	void colorsAboutToBeChanged();
	/// In subclasses this signal should usually originate from the adjusted() signal of the color model
	void colorsAdjusted();
	/// The reference to whatever color model obtained earlier is now invalid
	void colorsChanged();
	/// This object ( referenced by \a oldobj ) is removed from the source stack. Below it, is \a newobj. \a newobj can be 0, meaning chain self-destruction. Because of that, this signal has always to be connected with Qt::QueuedConnection.
	void objectPopped( NVBDataSource * newobj, NVBDataSource * oldobj);
	/// This object ( referenced by \a oldobj ) is wrapped with a filter. The filter is given by \a newobj
	void objectPushed( NVBDataSource * newobj, NVBDataSource * oldobj);
};

/// Increases the reference count for the source \a source
void useDataSource(NVBDataSource* source);
/// Decreases the reference count for the source \a source
void releaseDataSource(NVBDataSource* source);

/**
\class NVB3DDataSource
Extension of NVBDataSource for topography pages
*/
class NVB3DDataSource : public NVBDataSource
{
Q_OBJECT
public:
	/// constructs an empty topography source
	NVB3DDataSource():NVBDataSource() {;}
	virtual ~NVB3DDataSource() {;}

	/// \returns a topography page
	virtual inline NVB::PageType type() const {return NVB::TopoPage;}

	/// \returns the size of the data array.
	virtual QSize resolution() const =0;
	/// \returns the position of the page in real-world coordinates with dimensions given by xDim and yDim
	virtual QRectF position() const =0;
	
	/// \returns the angle of rotation around the center of \a position()
	virtual double rotation() const =0;

	/// \returns a pointer to the color model for this page.
	/// Note, that the pointer will become invalid after the \a colorsAboutToBeChanged() signal.
	virtual const NVBContColorModel * getColorModel() const =0;

	/// \returns a pointer to page data, in form of an array resolution().width()*resoltion().heigth() of doubles
	virtual const double * getData() const = 0;
	/// \returns the Z value at (\a x ,\a y ).
	virtual double getData(int x, int y) const = 0;

};

/**
\class NVBSpecDataSource
Extension of NVBDataSource for spectroscopy pages
*/
class NVBSpecDataSource : public NVBDataSource
{
 Q_OBJECT
public:
	NVBSpecDataSource():NVBDataSource() {;}
	virtual ~NVBSpecDataSource() {;}

	/// \returns a spectroscopy page
	virtual inline NVB::PageType type() const {return NVB::SpecPage;}

	/// \returns the dimension of the measurement axis (\e x on the graph)
	/// \todo Think about multi-dimensional spectroscopy
	virtual NVBDimension tDim() const = 0;

	/// \returns the list of colors, used by the page. The colors are in the same order, as curves
	virtual inline QList<QColor> colors() const { return getColorModel()->colorize(0,datasize().height()-1); }
	/// \returns a pointer to the color model for this page.
	/// Note, that the pointer will become invalid after the \a colorsAboutToBeChanged() signal.
	virtual const NVBDiscrColorModel * getColorModel() const =0;

	/// \returns a list of position if=n real space where curves were taken
	virtual QList<QPointF> positions() const =0;

	/// \returns a list of pointers to the curves of the page in form of \c QwtData .
	virtual QList<QwtData*> getData() const =0;

	/// \returns number of points per curve as QSize::width() and number of curves in page as QSize::height()
	virtual QSize datasize() const =0;
	/// \returns the size of data on the \e t-z plane
	virtual QRectF boundingRect() const;
	/// \returns the area where curves were taken on the \e x-y plane
	virtual QRectF occupiedArea() const;
};

Q_DECLARE_METATYPE(NVBDataSource*);

#endif
