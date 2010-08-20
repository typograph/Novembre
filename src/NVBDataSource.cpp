#include "NVBDataSource.h"

/**
	* \class NVBDataSource
	*
	*
	*/

NVBDataSource::NVBDataSource() {
}

virtual NVBDataSource::~NVBDataSource() {
	foreach(NVBAxisMap * am, amaps) delete am;
	foreach(NVBColorMap * cm, cmaps) delete cm;
}
