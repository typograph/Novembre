#ifndef NVBDATASETICONS_H
#define NVBDATASETICONS_H

#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
#include "NVBDataGlobals.h"
#include "NVBAxisSelector.h"

class NVBDataSet;
class NVBColorInstance;

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set);

class NVB2DIconEngine : public QObject, public QIconEngineV2 {
	Q_OBJECT
	protected:
		const NVBDataSet * dset;
		NVBColorInstance * ci;
		QList<QPixmap> cache;
		NVBAxisSelector selector;
		NVBSelectorInstance si;
	public :
		
		NVB2DIconEngine(const NVBDataSet* dataset);
		virtual ~NVB2DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);

	protected slots:
		void redrawCache();
		void setSource(const NVBDataSet * dataset = 0);
};


class NVB1DIconEngine : public QObject, public QIconEngineV2 {
	Q_OBJECT
	private:
		const NVBDataSet* dset;
		QMap<QSize,QPixmap> cache;

		NVBAxisSelector selector;
		NVBSelectorInstance instance;
		
		QPixmap drawCacheAt(QSize size);
	public:
		NVB1DIconEngine(const NVBDataSet* dataset);
		~NVB1DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
	protected slots:
		void setSource(const NVBDataSet * dataset = 0);
		void redrawCache() { cache.clear(); }
};


#endif
