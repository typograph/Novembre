#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QString>
#include "NVBDimension.h"
#include "NVBVariant.h"

class NVBAxisMap;
class NVBColorMap;

/// Increases the reference count for the source \a source
void useDataSource(NVBDataSource* source);
/// Decreases the reference count for the source \a source
void releaseDataSource(NVBDataSource* source);

typedef QMap<QString,NVBVariant> NVBDataComments;

class NVBDataSource : public QObject, public QList< NVBDataSet > {
	Q_OBJECT
	private:
		/// Number of references to this source. Note, that creating an object uses it automatically
		unsigned int refCount;
		friend void useDataSource(NVBDataSource* source);
		friend void releaseDataSource(NVBDataSource* source);
	protected:
		/// Comments for the source
		NVBDataComments comments;
		/// Axis sizes
		QVector< NVBAxis > axes;
//		/// Datasets
//		 sets;
		/// Axis maps
		QList< NVBAxisMap * > amaps;
		/// Color maps
		QList< NVBColorMap * > cmaps;
		/// Comments
		NVBDataComments comments;
  public:

		NVBDataSource();
		virtual ~NVBDataSource();

		virtual NVBAxis axis(int i) const { return axes.at(i); }
    virtual void addAxis(QString name, quint64 length) {
			if (sets.count() != 0) {
				NVBOutputError("Trying to add axes to a non-empty datasource");
				return;
				}
			axes.append(NVBAxis(this,name,length));
			}
		virtual void addAxisMap(NVBAxisMap * map, int axis = -1) { // Try the same with NVBAxisSelector
			if (axis == -1)
				axis = amaps.count();
			amaps.append(map);
			axes[axis].addMap(map);
			}
		virtual void addDataSet(QString name, double * data, NVBDimension dimension, QVector<quint8> axes = QVector<quint8>)  {
			if (axes.count() == 0)
				for(int i=0; i<axes.count(); i++)
					axes << i;
			append(NVBDataSet(this,name,data,dimension,axes));
			}

		/// \returns the comment for the given \a key
		virtual inline NVBVariant getComment(const QString& key) const { return comments.value(key); }
		/// \returns all the available comments in a QMap
		virtual inline const NVBDataComments& getAllComments() const { return comments; }
		inline void setComments(NVBDataComments _cms) { comments = _cms; }

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
		/// Data was changed completely. Reload averything
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

class NVBAxis {
  private:
    /// Parent data source (to be able to implement functions)
    NVBDataSource * p;
    /// Axis name
    QString n;
    /// Axis length
    quint64 l;
    /// Relevant mappings
    QVector<NVBAxisMap*> ms;
  public:
    NVBAxis(NVBDataSource * parent, QString name, quint64 length):p(parent),n(name),l(length) {;}
    // NVBAxis doesn't own anything, neither the parent, nor the maps.
    ~NVBAxis() {;}
    
    inline QString name() const { return n; }
    inline quint64 length() const { return l; }
    inline QVector<NVBAxisMap*> maps() const { return ms; }
    
    void addMapping(NVBAxisMap* map) { ms.append(map); }
};

class NVBDataSet : public QObject {
	Q_OBJECT
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
		QVector<quint8> as;
		/// Colors
		NVBColorMap * clr;
//    /// Relevant mappings
//    QVector<NVBDataMap*> ms;
  public:
		NVBDataSet(NVBDataSource * parent, QString name, double * data, NVBDimension dimension, QVector<quint8> axes)
			:	QObject()
			,	p(parent)
			,	n(name)
			,	d(data)
			,	dim(dimension)
			,	as(axes)
			{;}

    // Dataset owns its data, and will free the memory
    ~NVBDataSet() {
      free(d);
      }
     
    inline QString name() const { return n; }
		inline NVBDimension dimension() const { return dim; }
		inline dimension() const { return dim; }
		inline const double * data() const { return d; }
    QVector<quint64> sizes() const {
      QVector<quint64> r;
      foreach (quint16 i, as)
				r << p->axis(i).length();
      return r;
      }
    inline quint64 sizeAt(int i) const { return p->axis(as.at(i)).length(); }
		inline NVBDataComments comments() { return p->getAllComments(); }

//    inline QVector<NVBDataMap*> maps() const { return ms; }
//    void addMapping(NVBDataMap* map) { ms.append(map); }

	signals:
		void dataAboutToBeReallocated();
		void dataReallocated();
		void dataChanged();

};

