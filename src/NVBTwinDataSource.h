#ifndef NVBDATASOURCESPLITTER_H
#define NVBDATASOURCESPLITTER_H
#include "NVBCopyDataSource.h"

/**
 * When an intermediate datasource is opened in a new window,
 * it splits the datasource stack. A datasource cannot know
 * how many other sources are using its data, so it becomes difficult
 * to follow the I/O requirements.
 *
 * Behold NVBTwinDataSource...
 */


class NVBTwinDataSource : public NVBCopyDataSource {
	Q_OBJECT
	private:
		NVBTwinDataSource * other;
		
		NVBTwinDataSource(NVBDataSource * parent)
		:	NVBCopyDataSource(parent,NVBDataSource::MarkerCopy)
		, other(0)
			{
			}
			
		void setOther(NVBTwinDataSource * o) {
			other = o;
			connect(o,SIGNAL(destroyed(QObject)),this,SLOT(eliminate()));
			}
			
	public:
		static void createTwinSources(NVBTwinDataSource * one, NVBTwinDataSource * two, NVBDataSource * parent) {
			one = new NVBTwinDataSource(parent);
			two = new NVBTwinDataSource(parent);
			one->setOther(two);
			two->setOther(one);
			parent->override(one);
			}

		virtual inline NVBAxesProps inputRequirements() const { return outputRequirements() & other->outputRequirements(); };
	
	private slots:
		void eliminate() { emit objectPopped(parent(),this); }
			}
};

#endif
