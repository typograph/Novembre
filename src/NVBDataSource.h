#ifndef NVBDATASOURCE_H
#define NVBDATASOURCE_H

#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QString>
#include "NVBDataGlobals.h"
#include "NVBDimension.h"
#include "NVBVariant.h"
#include "NVBAxesSelector.h"

class NVBAxis;
class NVBDataSet;
class NVBAxisMap;
class NVBColorMap;
class NVBDataSource;

typedef QMap<QString,NVBVariant> NVBDataComments;

class NVBAxis {
	friend class NVBConstructableDataSource;
  private:
    /// Parent data source (to be able to implement functions)
    NVBDataSource * p;
    /// Axis name
    QString n;
    /// Axis length
    axissize_t l;
    /// Relevant mappings
    QVector< NVBAxisMap * > ms;

	  void addMapping( NVBAxisMap * map ) { ms.append(map); }

	public:
    NVBAxis(NVBDataSource * parent, QString name, axissize_t length) : p(parent), n(name), l(length) {;}
    NVBAxis(const NVBAxis & other) : p(other.p), n(other.n), l(other.l), ms(other.ms) {;}
    // NVBAxis doesn't own anything, neither the parent, nor the maps.
    ~NVBAxis() {;}

    inline QString name() const { return n; }
    inline axissize_t length() const { return l; }
    inline QVector<NVBAxisMap*> maps() const { return ms; }

};

class NVBDataSet : public QObject {
	Q_OBJECT
	friend class NVBConstructableDataSource;
	public:
		/// Initial type of the data
		/*!
			* This influences the way this dataset is displayed by default
			*/
		enum Type {
			Undefined = 0 ,
			Topography ,
			Spectroscopy
			};

	private:
    /// Parent data source (to be able to implement functions)
    NVBDataSource * p;
    /// Data name
    QString n;
    /// Value array
    double * d;
		/// Value dimension
		NVBDimension dim;
		/// Axis indexes
		QVector<axisindex_t> as;
		/// Colors
		NVBColorMap * clr;
//    /// Relevant mappings
//    QVector<NVBDataMap*> ms;
		Type t;

	protected:
		mutable QVector<axissize_t> asizes;
		
	public:
		NVBDataSet(NVBDataSource * parent,
							 QString name,
							 double * data,
							 NVBDimension dimension,
							 QVector<axisindex_t> axes,
							 Type type = Undefined,
							 NVBColorMap * colormap = 0);
							 
    // Dataset owns its data, and will free the memory
    ~NVBDataSet();
		
    inline QString name() const { return n; }
		inline NVBDimension dimension() const { return dim; }
		inline const double * data() const { return d; }
    QVector<axissize_t> sizes() const;
    axissize_t sizeAt(int i) const;
    inline axisindex_t nAxes() const { return as.count(); }

		NVBColorMap * colorMap() const ;

		inline NVBDataSource * dataSource() const { return p; }

		inline Type type() const { return t; }
			
		double min() const;
		double max() const;

    inline NVBDataComments comments();

//    inline QVector<NVBDataMap*> maps() const { return ms; }
//    void addMapping(NVBDataMap* map) { ms.append(map); }

	signals:
// TODO : think whether reallocation of data should change anything.
//	A good rule would be not to keep the pointer obtained from here
		void dataChanged();

};

/// Increases the reference count for the source \a source
void useDataSource(NVBDataSource* source);
/// Decreases the reference count for the source \a source
void releaseDataSource(NVBDataSource* source);

class NVBDataSource : public QObject {
	Q_OBJECT
	private:
		/// Number of references to this source. Note, that creating an object uses it automatically
		unsigned int refCount;
		friend void useDataSource(NVBDataSource* source);
		friend void releaseDataSource(NVBDataSource* source);

	protected:
		NVBAxesProps outputAxesProps;
		
  public:

    NVBDataSource();
    virtual ~NVBDataSource();

		virtual const NVBAxis & axis(axisindex_t i) const = 0;
		virtual const QList< NVBAxis > & axes() const = 0;
		virtual axisindex_t nAxes() const { return (axisindex_t)axes().count(); }

		virtual const QList< NVBDataSet * > & dataSets() const = 0;

		virtual const NVBColorMap * defaultColorMap() const ;
		
		/// \returns the comment for the given \a key
		virtual NVBVariant getComment(const QString& key) const = 0;
		/// \returns all the available comments in a QMap
		virtual const NVBDataComments& getAllComments() const = 0;


		/// Control widget for NVBWidgetStack
		virtual QWidget * controlWidget() const = 0;
		/// For wrappers
		virtual NVBDataSource * parent() const = 0;

		/**
			* To have safe changing of plugin parameters, a datasource has to have
			* a mechanism for preventing unsuitable combinations of axes.
			*
			* This is done via input/output requirements, that specify the number
			* and/or type of axes the datasource should have.
			*/
		
		/// Set minimal axis number / dimensions etc.
		virtual void setOutputRequirements(NVBAxesProps axesprops);
		virtual inline NVBAxesProps outputRequirements() const { return outputAxesProps; }
		virtual inline NVBAxesProps inputRequirements() const { return outputRequirements(); };
		
	public slots:
		/// To be used by "on-top" data sources. Emits \a objectPushed()
		virtual inline void override( NVBDataSource * newpt) { emit objectPushed(newpt,this); }
	signals:
		/// Some change will occur to sizes of axes.
		void axesAboutToBeResized();
		/// Data shape is not the same anymore. Pointers become invalid.
		void axesResized();
		/// Data is going to be violently changed (e.g. axes deleted). Forget everything
		void dataAboutToBeReformed();
		/// Data was changed completely. Reload everything
		void dataReformed();

		/**
			* @fn objectPopped
			*
			* \brief This data source is removed from the source stack
			*
			* This object ( referenced by \a oldobj ) is removed from the source stack.
			* Below it, is \a newobj. \a newobj can be 0, meaning chain self-destruction.
			* Because of that, this signal has always to be connected with Qt::QueuedConnection.
			*/
		void objectPopped( NVBDataSource * newobj, NVBDataSource * oldobj);

		/**
			* @fn objectPushed
			*
			* \brief This data source is wrapped with a filter
			*
			* This object ( referenced by \a oldobj ) is wrapped with a filter.
			* The filter is given by \a newobj.
			*/
		void objectPushed( NVBDataSource * newobj, NVBDataSource * oldobj);

};

class NVBConstructableDataSource : public NVBDataSource {
	protected:
		/// Comments for the source
		NVBDataComments comments;
		/// Axis sizes
		QList< NVBAxis > axs;
		/// Axis maps
		QList< NVBAxisMap * > amaps;
		/// Datasets
		QList< NVBDataSet * > dsets;
		/// Color maps
		QList< NVBColorMap * > cmaps;
		
	public:
		NVBConstructableDataSource();
		virtual ~NVBConstructableDataSource();

		virtual inline const NVBAxis & axis(axisindex_t i) const { return axs.at(i); }
    virtual NVBAxis & addAxis(QString name, axissize_t length);
		virtual void addAxisMap(NVBAxisMap * map, axisindex_t axis = -1);
		virtual void addDataSet(QString name, double * data, NVBDimension dimension, QVector<axisindex_t> axes = QVector<axisindex_t>());
		virtual const QList< NVBAxis > & axes() const { return axs; }
		virtual const QList< NVBDataSet * > & dataSets() const { return dsets; }

		virtual inline void addComment(QString key, QString value) { comments.insert(key,value); }
		/// \returns the comment for the given \a key
		virtual inline NVBVariant getComment(const QString& key) const { return comments.value(key); }
		/// \returns all the available comments in a QMap
		virtual inline const NVBDataComments& getAllComments() const { return comments; }
		inline void setComments(NVBDataComments _cms) { comments = _cms; }
		
		virtual QWidget * controlWidget() const { return 0; }
		virtual NVBDataSource * parent() const { return 0; }

};


#endif
