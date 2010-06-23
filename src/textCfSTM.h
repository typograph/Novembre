#ifndef TEXTCFSTM_H
#define TEXTCFSTM_H

#include "confile.h"
#include "lispInt.h"
#include "NVBLogger.h"
#include "dimension.h"

#include <QtPlugin>
#include "NVBFileGenerator.h"
#include "NVBContColoring.h"
#include "NVBDiscrColoring.h"
#include "NVBPages.h"


using namespace NVBErrorCodes;


class NVBTextSTMFileGenerator: public QObject, public NVBFileGenerator {
Q_OBJECT
Q_INTERFACES(NVBFileGenerator);

public:
  NVBTextSTMFileGenerator():NVBFileGenerator() {;}
  virtual ~NVBTextSTMFileGenerator() {;}

  virtual inline QString moduleName() { return QString("TextSTM");}
  virtual inline QString moduleDesc() { return QString("Confile module. Simple formula-based paged STM file. The file format is not intended for general usage. It is only for testing. Can be used to visualize f(x,y).");}

  virtual inline QString nameFilter() { return QString("Text STM Files (*.sch)"); } // double semicolon as separator
  virtual inline QString extFilter() { return QString("*.sch"); }; // single semicolon as separator

  virtual bool canLoadFile(QString filename);
	virtual NVBFileStruct * loadFile(const NVBAssociatedFilesInfo & info) const throw();
	virtual NVBFileInfo * loadFileInfo(const NVBAssociatedFilesInfo & info) const throw();

};

class TtextSTMTopoPage : public NVB3DPage {
Q_OBJECT
public:
  TtextSTMTopoPage(TConSect * sect);
  virtual ~TtextSTMTopoPage() {;}
public slots:
  virtual void commit();
};

class TtextSTMSpecPage : public NVBSpecPage {
Q_OBJECT
protected:
  double * ydata;
  double * xdata;
public:
  TtextSTMSpecPage(TConSect * sect);
  virtual ~TtextSTMSpecPage();
public slots:
  virtual void commit();
};

#endif

