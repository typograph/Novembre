#ifndef NVBDATASETICONS_H
#define NVBDATASETICONS_H

#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
#include "NVBDataGlobals.h"

class NVBDataSet;

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set);

#if QT_VERSION >= 0x040300
class NVB2DIconEngine : public QObject, public QIconEngineV2 {
#else
class NVB2DIconEngine : public QObject, public QIconEngine {
#endif
	Q_OBJECT
	protected:
		const NVBDataSet * dset;
		NVBColorInstance * i;
		QList<QPixmap*> cache;

	public :
		
		NVB2DIconEngine(NVBDataSet* dataset);
		virtual ~NVB2DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);

	protected slots:
		void redrawCache();
		void setSource(NVBDataSet * dataset);
};


#if QT_VERSION >= 0x040300
class NVB1DIconEngine : public QObject, public QIconEngineV2 {
#else
class NVB1DIconEngine : public QObject, public QIconEngine {
#endif
	Q_OBJECT
	private:
		const NVBDataSet* dset;
		axisindex_t taxis;
	public:
		NVB1DIconEngine(const NVBDataSet* dataset);
		~NVB1DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
	protected slots:
		void setSource(NVBDataSet * dataset);
};


#endif
