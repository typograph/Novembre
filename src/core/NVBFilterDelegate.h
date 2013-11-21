//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef NVBFILTERDELEGATE_H
#define NVBFILTERDELEGATE_H

#include "NVBLogger.h"
#include "NVBDataSource.h"
#include "NVBGeneralDelegate.h"

/// The filter forwards comments of the page \a source . The macros has to be used in class declaration. You have to reconnect signals yourself.
#define NVB_FORWARD_COMMENTS(source) \
  virtual inline NVBVariant getComment(const QString& key) const { return (source)->getComment(key); } \
  virtual inline const QMap<QString,NVBVariant>& getAllComments() const { return (source)->getAllComments(); } \
 
#define NVB_FORWARD_TOPODIM(source) \
  virtual inline NVBDimension xDim() const { return (source)->xDim();} \
  virtual inline NVBDimension yDim() const { return (source)->yDim();} \
  virtual inline NVBDimension zDim() const { return (source)->zDim();} \
 
#define NVB_FORWARD_SPECDIM(source) \
  NVB_FORWARD_TOPODIM(source); \
  virtual inline NVBDimension tDim() const { return (source)->tDim();}

/// The filter forwards data of the topography page \a source . The macros has to be used in class declaration.  You have to reconnect signals yourself.
#define NVB_FORWARD_TOPODATA(source) \
  virtual inline double getZMin() const {return (source)->getZMin();} \
  virtual inline double getZMax() const {return (source)->getZMax();} \
  NVB_FORWARD_TOPODIM(source); \
  virtual inline QSize resolution() const { return (source)->resolution(); } \
  virtual inline QRectF position() const { return (source)->position(); } \
  virtual inline double rotation() const { return (source)->rotation(); } \
  virtual inline const double * getData() const { return (source)->getData(); } \
  virtual inline double getData(int x, int y) const { return (source)->getData(x,y); } \
 
/// The filter forwards data of the spectroscopy page \a source . The macros has to be used in class declaration.  You have to reconnect signals yourself.
#define NVB_FORWARD_SPECDATA(source) \
  virtual inline double getZMin() const {return (source)->getZMin();} \
  virtual inline double getZMax() const {return (source)->getZMax();} \
  NVB_FORWARD_SPECDIM(source); \
  virtual QSize datasize() const { return (source)->datasize(); } \
  virtual QList<QPointF> positions() const { return (source)->positions(); } \
  virtual QList<QwtData*> getData() const { return (source)->getData(); } \
 
/// The filter forwards all data signals (aboutToBeChanged(), adjusted() and Changed()).
#define NVB_FORWARD_DATASIGNALS(source) \
    connect(source,SIGNAL(dataAboutToBeChanged()),SIGNAL(dataAboutToBeChanged())); \
    connect(source,SIGNAL(dataChanged()),SIGNAL(dataChanged())); \
    connect(source,SIGNAL(dataAdjusted()),SIGNAL(dataAdjusted()));

/// The filter forwards coloring of the spectroscopy page \a source . The macros has to be used in class declaration. You have to reconnect signals yourself.
#define NVB_FORWARD_SPECCOLORS(source) \
  virtual const NVBDiscrColorModel * getColorModel()  const  { return (source)->getColorModel(); } \
 
/// The filter forwards coloring of the topography page \a source . The macros has to be used in class declaration.  You have to reconnect signals yourself.
#define NVB_FORWARD_TOPOCOLORS(source) \
  virtual inline const NVBContColorModel * getColorModel() const  { return (source)->getColorModel(); } \
 
/// The filter forwards all color signals (aboutToBeChanged(), adjusted() and Changed()).
#define NVB_FORWARD_COLORSIGNALS(source) \
    connect(source,SIGNAL(colorsAboutToBeChanged()),SIGNAL(colorsAboutToBeChanged())); \
    connect(source,SIGNAL(colorsChanged()),SIGNAL(colorsChanged())); \
    connect(source,SIGNAL(colorsAdjusted()),SIGNAL(colorsAdjusted()));

class NVB3DDataSource;
class NVBSpecDataSource;

class NVBDataSourceLink {

	protected:
		NVBDataSource * provider;

	public:
		NVBDataSourceLink(NVBDataSource* source): provider(source) {
			if (provider) useDataSource(provider);
			}
		virtual ~NVBDataSourceLink() {
			if (provider) releaseDataSource(provider);
			}

		void substSource(NVBDataSource* source) {
			if (provider != source) {
				if (provider) releaseDataSource(provider);

				provider = source;

				if (provider) useDataSource(provider);
				}
			}

	};


/**
 * \class NVB3DFilterDelegate
 * Do not be confused: this class operates on any page and provides a topographical
 * one. It is a morthed datasource. The default implementation returns the underlying page
 */

class NVB3DFilterDelegate :  public NVB3DDataSource, public NVBDataSourceLink {
		Q_OBJECT
	public:
		NVB3DFilterDelegate(NVBDataSource* source): NVB3DDataSource(), NVBDataSourceLink(source) {
//    connect(provider,SIGNAL(commentsChanged()),SIGNAL(commentsChanged()));
			owner = source->owner;
//    followSource(); // has to be called from subclasses

			}
		virtual ~NVB3DFilterDelegate() { if (provider) provider->disconnect(this); }

		virtual inline QString name() const { return provider->name(); }
//	virtual inline const NVBFile * owner() const { return provider->owner();}
//  virtual void setFileName(QString newfile) { provider->setFileName(newfile); }

		NVB_FORWARD_COMMENTS(provider);

	public slots:
		virtual void setSource(NVBDataSource* source) {

			if (provider) provider->disconnect(this);

			if (!source) {
				emit objectPopped(source, this); // going away
				return;
				}

			substSource(source);

			connect(provider, SIGNAL(commentsChanged()), SIGNAL(commentsChanged()));

			connectSignals();

			followSource();

			}

	protected :
		virtual void connectSignals() {;}
		void followSource() {
			provider->override(this);

			connect(provider, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
			connect(provider, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);
			}

	};

/**
  * \class NVB3DHardlinkDelegate
  * This is a copy class. It acts like a permanent link to the original page.
  * It does not react to objectPushed()
  */

class NVB3DHardlinkDelegate : public NVB3DDataSource, public NVBDataSourceLink  {
		Q_OBJECT
	protected:
		NVB3DDataSource * page;
	public:
		NVB3DHardlinkDelegate(NVB3DDataSource * source): NVB3DDataSource(), NVBDataSourceLink(source), page(source) {
			NVB_FORWARD_DATASIGNALS(source);
			NVB_FORWARD_COLORSIGNALS(source);
			owner = source->owner;
			connect(provider, SIGNAL(commentsChanged()), SIGNAL(commentsChanged()));
			connect(source, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource *)), this, SLOT(reparentBranch(NVBDataSource *, NVBDataSource *)), Qt::QueuedConnection);
			}
		virtual ~NVB3DHardlinkDelegate() {;}

		NVB_FORWARD_TOPOCOLORS(page);
		NVB_FORWARD_TOPODATA(page);
		NVB_FORWARD_COMMENTS(provider);

		virtual inline QString name() const { return provider->name(); }
//	virtual inline const NVBFile * owner() const { return provider->owner();}
//  virtual void setFileName(QString newfile) { provider->setFileName(newfile); }

	protected slots:
		void reparentBranch(NVBDataSource * , NVBDataSource *) {
			//### There are two possibilities at that point. If the branching point object is removed by user, we could either keep it for the branches or destroy the branch. It is more logical and safer to destroy it.
			emit objectPopped(this, 0); // self-destruct branch
			}

	};

/**
 * \class NVBSpecFilterDelegate
 * Do not be confused: this class operates on any page and provides a spectroscopical
 * one. It is a morthed datasource. Note, that this class doesn't provide anything
 * useful.
 */

class NVBSpecFilterDelegate : public NVBSpecDataSource, public NVBDataSourceLink {
		Q_OBJECT
	public:
		NVBSpecFilterDelegate(NVBDataSource* source): NVBSpecDataSource(), NVBDataSourceLink(source) {
			owner = source->owner;
			}
		virtual ~NVBSpecFilterDelegate() { if (provider) provider->disconnect(this); }

		virtual inline QString name() const { return provider->name(); }
//	virtual inline const NVBFile * owner() const { return provider->owner();}
//  virtual void setFileName(QString newfile) { provider->setFileName(newfile); }

		NVB_FORWARD_COMMENTS(provider);

	public slots:
		virtual void setSource(NVBDataSource* source) {

			if (provider) provider->disconnect(this);

			if (!source) {
				emit objectPopped(source, this); // going away
				return;
				}

			substSource(source);

			connectSignals();

			followSource();

			}

	protected :
		virtual void connectSignals() {
			connect(provider, SIGNAL(commentsChanged()), SIGNAL(commentsChanged()));
			}
		void followSource() {
			provider->override(this);

			connect(provider, SIGNAL(objectPushed(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)));
			connect(provider, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource*)), SLOT(setSource(NVBDataSource*)), Qt::QueuedConnection);
			}

	};

/**
  * \class NVBSpecHardlinkDelegate
  * This is a copy class. It acts like a permanent link to the original page.
  * It does not react to objectPushed()
  */

class NVBSpecHardlinkDelegate : public NVBSpecDataSource, public NVBDataSourceLink {
		Q_OBJECT
	protected:
		NVBSpecDataSource * page;
	public:
		NVBSpecHardlinkDelegate(NVBSpecDataSource * source): NVBSpecDataSource(), NVBDataSourceLink(source), page(source) {
			NVB_FORWARD_DATASIGNALS(source);
			NVB_FORWARD_COLORSIGNALS(source);
			owner = source->owner;
			connect(provider, SIGNAL(commentsChanged()), SIGNAL(commentsChanged()));
			connect(source, SIGNAL(objectPopped(NVBDataSource *, NVBDataSource *)), this, SLOT(reparentBranch(NVBDataSource *, NVBDataSource *)), Qt::QueuedConnection);
			}
		virtual ~NVBSpecHardlinkDelegate() {;}

		NVB_FORWARD_SPECCOLORS(page);
		NVB_FORWARD_SPECDATA(page);
		NVB_FORWARD_COMMENTS(provider);

		virtual inline QString name() const { return provider->name(); }
//	virtual inline const NVBFile * owner() const { return provider->owner();}
//  virtual void setFileName(QString newfile) { provider->setFileName(newfile); }

	protected slots:
		void reparentBranch(NVBDataSource * , NVBDataSource *) {
			//### There are two possibilities at that point. If the branching point object is removed by user, we could either keep it for the branches or destroy the branch. It is more logical and safer to destroy it.
			emit objectPopped(this, 0); // self-destruct branch
			}
	};

// NVBDataSource * hardlinkDataSource( NVBDataSource * );
/// Create a stable link to a \a source, that will not be affected by masking the source through filters
NVBDataSource * hardlinkDataSource(NVBDataSource * source);
#endif
