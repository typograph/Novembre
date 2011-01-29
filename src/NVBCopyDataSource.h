#ifndef NVBCOPYDATASOURCE_H
#define NVBCOPYDATASOURCE_H
#include "NVBDataSource.h"

class NVBCopyDataSource : public NVBDataSource {

	private:
		NVBDataSource * source;
		bool marker_copy;
	private slots:
		void reemitPushed(NVBDataSource* news, NVBDataSource* olds);
		void reemitPopped(NVBDataSource* news, NVBDataSource* olds);
		void setParent(NVBDataSource * s);

	public:
		enum CopyType { TrueCopy, MarkerCopy };
		
		NVBCopyDataSource(NVBDataSource * p, CopyType c = MarkerCopy);
		~NVBCopyDataSource();
		
		virtual NVBAxis axis(axisindex_t i) const { return parent()->axis(i); }
		virtual axisindex_t nAxes() const { return parent()->nAxes(); }
		
		virtual const QList< NVBAxis > & axes() const { return parent()->axes(); }
		virtual const QList< NVBDataSet * > & dataSets() const { return parent()->dataSets(); }

		/// \returns the comment for the given \a key
		virtual inline NVBVariant getComment(const QString& key) const { return parent()->getComment(key); }
		/// \returns all the available comments in a QMap
		virtual inline const NVBDataComments& getAllComments() const { return parent()->getAllComments(); }

		/// Control widget for NVBWidgetStack
		virtual QWidget * controlWidget() const { return 0; }
		/// For wrappers
		virtual inline NVBDataSource * parent() const { return source; }
}

#endif
