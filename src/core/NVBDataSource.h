//
// Copyright 2006 - 2013 Timofey <typograph@elec.ru>
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

#ifndef NVBDATASOURCE_H
#define NVBDATASOURCE_H

class NVBDataColorInstance;
class NVBFile;
class NVBDataSet;
class NVBAxisMap;
class NVBColorMap;
class NVBDataSource;
class NVBAxisPhysMap;

#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QString>
#include "NVBAxis.h"
#include "NVBUnits.h"
#include "NVBVariant.h"
//#include "NVBAxisSelector.h"

typedef QMap<QString, NVBVariant> NVBDataComments;

class NVBDataSet : public QObject {
		Q_OBJECT
		friend class NVBConstructableDataSource;
		friend class NVBSelectorDataInstance;
	public:
		/// Initial type of the data
		enum Type {
		  Undefined = 0, // The dataset is displayed in default (2D) view by default
		  Topography,    // The dataset is displayed in 2D view by default
		  Spectroscopy   // The dataset is displayed in Graph view by default
		};

	protected:
		/// Parent data source (to be able to implement functions)
		NVBDataSource * p;
		/// Data name
		QString n;
		/// Value array
		double * d;
		/// Value dimension
		NVBUnits dim;
		/// Axis indexes
		QVector<axisindex_t> as;
		/// Colors
		NVBColorMap * clr;
		/// Hint for displaying the dataset
		/*!
		 * This paremeter influences the way this dataset is displayed by default.
		 * All datasets are functionally equivalent, independently of their type.
		 */
		Type t;

		NVBDataComments comments;

	protected:
		mutable QVector<axissize_t> asizes;
		/// minimum and maximum
		mutable double zmin, zmax;
		void getMinMax() const;

	public:
		NVBDataSet(NVBDataSource * parent,
		           QString name,
		           double * data,
		           NVBUnits dimension,
		           QVector<axisindex_t> axes,
		           Type type = Undefined,
		           NVBColorMap * colormap = 0);

		// Dataset owns its data, and will free the memory
		virtual ~NVBDataSet();

		inline QString name() const { return n; }
		inline NVBUnits dimension() const { return dim; }
		inline virtual const double * data() const { return d; }
		QVector<axissize_t> sizes() const;
		axissize_t sizeAt(axisindex_t i) const;
		const NVBAxis & axisAt(axisindex_t i) const ;
		inline axisindex_t nAxes() const { return as.count(); }

		inline axisindex_t parentIndex(axisindex_t i) const { return as.at(i); }
		inline const QVector<axisindex_t> & parentIndexes() const { return as; }
		inline axisindex_t indexAtParent(axisindex_t i) const { return as.indexOf(i); }

		template< template<class T> class CTo, template<class T> class CFrom >
		CTo<axisindex_t> mapFromSource(CFrom<axisindex_t> indexes) const {
			CTo< axisindex_t > r;
			foreach(axisindex_t i, indexes)
			r.append(indexAtParent(i));
			return r;
			}

		inline QList<axisindex_t> mapFromSource(QList<axisindex_t> indexes) const
			{ return mapFromSource<QList, QList>(indexes); }
		inline QVector<axisindex_t> mapFromSource(QVector<axisindex_t> indexes) const
			{ return mapFromSource<QVector, QVector>(indexes); }

		NVBAxisMapping mapFromSource(const NVBAxisMap * map) const;
		NVBAxisMapping mapFromSource(const NVBAxisMapping & mapping) const;

		template< template<class T> class CTo, template<class T> class CFrom >
		CTo<axisindex_t> mapToSource(CFrom<axisindex_t> indexes) const {
			CTo< axisindex_t > r;
			foreach(axisindex_t i, indexes)
			r.append(parentIndex(i));
			return r;
			}

		inline QList<axisindex_t> mapToSource(QList<axisindex_t> indexes) const
			{ return mapToSource<QList, QList>(indexes); }
		inline QVector<axisindex_t> mapToSource(QVector<axisindex_t> indexes) const
			{ return mapToSource<QVector, QVector>(indexes); }

		const NVBColorMap * colorMap() const ;
		inline void setColorMap(NVBColorMap * m) { clr = m; }
		NVBDataColorInstance* colorInstance() const;

		inline NVBDataSource * dataSource() const { return p; }

		inline Type type() const { return t; }

		double min() const;
		double max() const;

		//! \returns all comments belonging to the dataset
		inline NVBDataComments getAllComments() const { return comments; }
		//! \returns comment from this dataset, or if \a recursive, also from the parent datasource
		NVBVariant getComment(const QString & key, bool recursive = true) const;

//    inline QVector<NVBDataMap*> maps() const { return ms; }
//    void addMapping(NVBDataMap* map) { ms.append(map); }

	signals:
		/// The signal is emitted whenever the data has changed. This includes cases when axis sets change
		void dataChanged();

		/// Size and/or number of axes has changed. This signal is emitted whenever the parent datasource emits its signal
		void dataReformed();

		/// When a datasource is replacing another, it makes all its datasets emit this signal.
		/// It is also emitted when the dataset is replaced (the pointer to the dataset becomes invalid)
		void overwritten(NVBDataSet * newDataSet);

	public slots:
		/// Some of the dataset data is cached. This slot clears all cached values.
		void invalidateCaches();
		/// Signal users of the dataset that the other one takes its place
		void overwrite(NVBDataSet * newDataSet) { emit overwritten(newDataSet); }
	};

Q_DECLARE_METATYPE(NVBDataSet*)
// Q_DECLARE_METATYPE(const NVBDataSet*)

#ifdef NVB_DEBUG_DSUSE

/// Mark the datasource \a source as used, to prevent clean-up
#define useDataSource(source) useAndLogDataSource(NVB_CURRENT_FUNCTION,(source));
/// Unmark the datasource, to enable automatic clean-up
#define releaseDataSource(source) releaseAndLogDataSource(NVB_CURRENT_FUNCTION,(source));

/// Mark the dataset \a set as used, to prevent clean-up
#define useDataSet(set) useAndLogDataSet(NVB_CURRENT_FUNCTION,(set));
/// Unmark the dataset, to enable automatic clean-up
#define releaseDataSet(set) releaseAndLogDataSet(NVB_CURRENT_FUNCTION,(set));

/// Mark the datasource \a source as used, to prevent clean-up
void useAndLogDataSource(QString fn, const NVBDataSource* source);
/// Unmark the datasource, to enable automatic clean-up
void releaseAndLogDataSource(QString fn, const NVBDataSource* source);

/// Mark the dataset \a set as used, to prevent clean-up
void useAndLogDataSet(QString fn, const NVBDataSet* set);
/// Unmark the dataset, to enable automatic clean-up
void releaseAndLogDataSet(QString fn, const NVBDataSet* set);

#else

/// Mark the datasource \a source as used, to prevent clean-up
void useDataSource(const NVBDataSource* source);
/// Unmark the datasource, to enable automatic clean-up
void releaseDataSource(const NVBDataSource* source);

/// Mark the dataset \a set as used, to prevent clean-up
void useDataSet(const NVBDataSet* set);
/// Unmark the dataset, to enable automatic clean-up
void releaseDataSet(const NVBDataSet* set);

#endif

typedef int NVBAxesProps; // FIXME This is here to make compilation times faster

class NVBDataSource : public QObject {

		friend class NVBFile;

		Q_OBJECT

	private:
		/// Number of references to this source
		mutable unsigned int refCount;
#ifdef NVB_DEBUG_DSUSE
		friend void useAndLogDataSource(QString fn, const NVBDataSource* source);
		friend void releaseAndLogDataSource(QString fn, const NVBDataSource* source);
#else
		friend void useDataSource(const NVBDataSource* source);
		friend void releaseDataSource(const NVBDataSource* source);
#endif
		NVBDataSource( const NVBDataSource& );

	protected:
		NVBAxesProps outputAxesProps;
		NVBDataComments comments;

	public:

		NVBDataSource();
		virtual ~NVBDataSource();

		virtual const NVBAxis & axis(axisindex_t i) const { return axes().at(i);}
		NVBAxis axisByName(QString name) const ;
		virtual axisindex_t axisIndexByName(QString name) const;
		virtual const QList< NVBAxis > & axes() const = 0;
		virtual axisindex_t nAxes() const { return (axisindex_t)axes().count(); }

		virtual const QList< NVBDataSet * > & dataSets() const = 0;

		virtual const NVBColorMap * defaultColorMap() const;

		NVBAxisMapping getMapping(const NVBAxisMap * map) const;

		/// \returns the comment for the given \a key
		virtual NVBVariant getComment(const QString& key, bool recursive = true) const;
		/// \returns all the available comments in a QMap
		virtual NVBDataComments getAllComments() const;
		/// \returns a unified list of all dataset comments for key
		virtual NVBVariant collectComments(const QString & key) const;


		/// Control widget for NVBWidgetStack
		virtual QWidget * controlWidget() const = 0;
		/// For wrappers
		virtual NVBDataSource * parent() const = 0;
		/// Original file
		virtual NVBFile * origin() const { return parent() ? parent()->origin() : 0; }

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
		virtual inline NVBAxesProps inputRequirements() const { return outputRequirements(); }

	public slots:
		/// To be used by "on-top" data sources. Emits \a objectPushed()
		virtual inline void override( NVBDataSource * newpt) { emit objectPushed(newpt, this); }

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
		void objectPopped(const NVBDataSource * newobj, const NVBDataSource * oldobj);

		/**
			* @fn objectPushed
			*
			* \brief This data source is wrapped with a filter
			*
			* This object ( referenced by \a oldobj ) is wrapped with a filter.
			* The filter is given by \a newobj.
			*/
		void objectPushed(const NVBDataSource * newobj, const NVBDataSource * oldobj);

	};

Q_DECLARE_METATYPE(NVBDataSource*)

class NVBConstructableDataSource : public NVBDataSource {
		Q_OBJECT
	protected:
		/// Axis sizes
		QList< NVBAxis > axs;
		/// Axis maps
		QList< NVBAxisMapping > amaps;
		/// Datasets
		QList< NVBDataSet * > dsets;
		/// Color maps
		QList< NVBColorMap * > cmaps;
		/// Parent file
		NVBFile * o;

		// Should never be constructed without source (or pass 0)
		NVBConstructableDataSource();
	public:
		NVBConstructableDataSource(NVBFile * orig);
		virtual ~NVBConstructableDataSource();

		virtual inline const NVBAxis & axis(axisindex_t i) const { return axs.at(i); }
		virtual NVBAxis & addAxis(QString name, axissize_t length);
		virtual void addAxisMap(NVBAxisMap * map, QVector<axisindex_t> axes = QVector<axisindex_t>());
		virtual NVBDataSet * addDataSet(QString name, double* data, NVBUnits dimension, NVBDataComments comments = NVBDataComments(), QVector< axisindex_t > axes = QVector<axisindex_t>(), NVBDataSet::Type type = NVBDataSet::Undefined, NVBColorMap* map = 0);
		virtual NVBDataSet * addDataSet(QString name, double* data, NVBUnits dimension, NVBDataSet::Type type) {
			return addDataSet(name, data, dimension, NVBDataComments(), QVector<axisindex_t>(), type, 0);
			}
		virtual const QList< NVBAxis > & axes() const { return axs; }
		virtual const QList< NVBDataSet * > & dataSets() const { return dsets; }

		virtual inline void addComment(QString key, NVBVariant value) { comments.insert(key, value); }
//		/// \returns the comment for the given \a key
//		virtual NVBVariant getComment(const QString& key, bool recursive = true) const ;
//		/// \returns all the available comments in a QMap
//		virtual inline const NVBDataComments& getAllComments() const { return cmnts; }

		void filterAddComments(NVBDataComments& newComments);

		virtual QWidget * controlWidget() const { return 0; }
		virtual NVBDataSource * parent() const { return 0; }

		/// Original file
		virtual NVBFile * origin() const { return o;}

		/// Make this datasource an orphan
		void detach() { o = 0; }

	};

#endif
