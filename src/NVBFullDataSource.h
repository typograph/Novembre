#ifndef NVBFULLDATASOURCE_H
#define NVBFULLDATASOURCE_H

#include "NVBDataSource.h"

class NVBFullDataSource : public NVBDataSource {
  private:
    /// Axis sizes
    QVector< NVBAxis > axes;
    /// Datasets
    QList< NVBDataSet > sets;
    /// Axis maps
    QList< NVBAxisMap * > amaps;
    /// Data maps
    QList< NVBDataMap * > dmaps;
  public:
    NVBFullDataSource() {;}

    NVBAxis axis(int i) const { return axes.at(i); }
};

#endif
