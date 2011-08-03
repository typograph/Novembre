#ifndef NVBDATASETICONS_H
#define NVBDATASETICONS_H

#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
#include "NVBDataGlobals.h"
#include "NVBAxisSelector.h"

class NVBDataSet;
class NVBColorInstance;
class NVBAxisPointMap;
class NVBAxes2DGridMap;
class NVBFile;

/*
 * This function has to be moved to NVBToolsFactory
 */
QIcon createDatasetIcon(const NVBDataSet * set);
QList<QIcon> createDataIcons(const NVBFile * file);
QList<QIcon> createSpecOverlayIcons(const NVBFile * file);

class NVB2DIconEngine : public QObject, public QIconEngineV2 {
	Q_OBJECT
	protected:
		const NVBDataSet * dset;
		NVBColorInstance * ci;
		QList<QPixmap> cache;
		NVBAxisSelector selector;
		NVBSelectorDataInstance si;
	public :
		
		NVB2DIconEngine(const NVBDataSet* dataset);
		virtual ~NVB2DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);

		static QPixmap colorizeWithPlaneSubtraction(NVBDataSet * page);

	protected slots:
		virtual void redrawCache();
		void setSource(const NVBDataSet * dataset = 0);
};


class NVB1DIconEngine : public QObject, public QIconEngineV2 {
	Q_OBJECT
	protected:
		const NVBDataSet* dset;
		QMap<QSize,QPixmap> cache;

		NVBAxisSelector selector;
		NVBSelectorDataInstance instance;
		
		QPixmap drawCacheAt(QSize size);
	public:
		NVB1DIconEngine(const NVBDataSet* dataset);
		~NVB1DIconEngine();

		virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
	protected slots:
		void setSource(const NVBDataSet * dataset = 0);
		void redrawCache() { cache.clear(); }
};

class NVBMixTSIconEngine : public NVB2DIconEngine {
private:
protected:
	QList<QPointF> points;
public :
	NVBMixTSIconEngine (const NVBDataSet* topo, const NVBAxisPointMap * points, axissize_t npnts);
	NVBMixTSIconEngine (const NVBDataSet* topo, const NVBAxes2DGridMap * points, axissize_t nptsx, axissize_t nptsy);
	virtual ~NVBMixTSIconEngine() {;}

	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State);
};

#endif
