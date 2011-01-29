#ifndef NVBDATATRANSFORM_H
#define NVBDATATRANSFORM_H

#include "NVBDataCore.h"
#include "NVBDataSource.h"

class NVBFile;
/**
	* \class NVBDataTransform
	*
	* Transforms datasets using \fn transfromNArray(), but is a bit higher order.
	* Subclass in your filters.
	*/
class NVBDataTransform {
	protected:
		QList<NVBAxis> as;
		QVector<axissize_t> resultSize;
		virtual double singleValueTransform(double a) const = 0;
	public:
		NVBDataTransform(QList<NVBAxis> axes):as(axes) {;}

		/// Returns the number of axes in the slice (
		axisindex_t reqAxes();

		/// Tranforms a dataset using this transform
		NVBDataSet * transformDataSet(const NVBDataSet * data, QVector<axisindex_t> sliceaxes, QVector<axisindex_t> targetaxes = QVector<axisindex_t>());

		/// The transform function for \a transformNArray
		virtual void operator() (const double * data, axisindex_t n, const axissize_t * sizes, axisindex_t m, const axisindex_t * slice, double * target) const;
};

/// Transform data using a transform function
//template <typename Transform>
double * transformNArray(const double * data, axisindex_t n, const axissize_t * sizes,
																							axisindex_t m, const axisindex_t * sliceaxes, const axisindex_t * targetaxes,
																							axisindex_t p, const axissize_t * newsizes,
																							const NVBDataTransform & transform );

class NVBTransformedDataSource : public NVBDataSource {
	Q_OBJECT
	private:
		const NVBDataSource * parent_ds;

	public:
		/// Constructs a new datasource using an external datatransform. Assumes ownership of \a transform
		NVBTransformedDataSource(const NVBDataSource * parent);

#if 0
		/// For use with NVBFileWindow - pass data on active axes and datasets
		NVBTransformedDataSource(const NVBDataSource * parent, NVBDataTransform & transform, NVBActiveView view);
#endif

		virtual NVBAxis axis(axisindex_t i) const = 0;
		virtual axisindex_t nAxes() const = 0;

		/// \returns the comment for the given \a key
		virtual inline NVBVariant getComment(const QString& key) const { return parent_ds->getComment(key); }
		/// \returns all the available comments in a QMap
		virtual inline const NVBDataComments& getAllComments() const { return parent_ds->getAllComments(); }

//		virtual QWidget * controlWidget() const = 0;
		virtual NVBDataSource * parent() const { return parent_ds; }

	protected slots:
		virtual void recalculate() = 0;

};

/** ********
	* For transfroms that keep the existing axes, like FFT, smooth or plane subtract
	*
	*/

class NVBCDimTDataSource : public NVBTransformedDataSource {
	Q_OBJECT
	private:
		const NVBDataSource * parent_ds;

	public:
		/// Constructs a new datasource using an external datatransform. Assumes ownership of \a transform
		NVBTransformedDataSource(const NVBDataSource * parent);

		virtual NVBAxis axis(axisindex_t i) const { return parent_ds->axis(i); }
		virtual axisindex_t nAxes() const { return  parent_ds->nAxes(); }

	protected slots:
		virtual void recalculate();

};

#endif