#include "textCfSTM.h"
#include "NVBFileInfo.h"

bool NVBTextSTMFileGenerator::canLoadFile(QString filename)
{
  if (filename.at(filename.length()-1) != 'h') return false;
  try {
    TConFile tmp(filename);
    }
  catch(...) {
    return false;
    }
  // TODO Internal format checking
  return true;
}

NVBFileStruct * NVBTextSTMFileGenerator::loadFile(QString filename)
{
  TConFile * confile;
  try {
    confile = new TConFile(filename);
    }
  catch (int e) {
    NVBOutputError("NVBTextSTMFileGenerator::loadFile","Confile creation failed with error %d",e);
    return NULL;
    }

  TConSect * opt = confile->getSect("General");
  if (!opt) throw nvberr_invalid_format;
  int num_pages = opt->getIntValue("NumPages");

  NVBFileStruct * s = new NVBFileStruct(filename);
  if (!s) throw nvberr_not_enough_memory;

  char sname[20];

  for (int i=1;i<=num_pages;i++) {
    sprintf(sname,"Page%d",i);
    if (!(opt = confile->getSect(sname))) throw nvberr_invalid_format;
    char * ptype = opt->getValue("PageType");
    if (ptype[0] == 'T' || ptype[0] == 't')
      s->pages.append(new TtextSTMTopoPage(opt));
    else if (ptype[0] == 'S' || ptype[0] == 's')
      s->pages.append(new TtextSTMSpecPage(opt));
    else throw nvberr_invalid_format;
    }

  delete confile;

  return s;
}

NVBFileInfo * NVBTextSTMFileGenerator::loadFileInfo(QString filename)
{
  TConFile * confile;
  try {
    confile = new TConFile(filename);
    }
  catch (int e) {
    NVBOutputError("NVBTextSTMFileGenerator::loadFileInfo","Confile creation failed with error %d",e);
    return NULL;
    }

  TConSect * opt = confile->getSect("General");
  if (!opt) throw nvberr_invalid_format;
  int num_pages = opt->getIntValue("NumPages");

  NVBFileInfo * i = new NVBFileInfo(filename);
  if (!i) throw nvberr_not_enough_memory;

  char sname[20];

  for (int j=1;j<=num_pages;j++) {
    sprintf(sname,"Page%d",j);
    if (!(opt = confile->getSect(sname))) throw nvberr_invalid_format;
    char * c = opt->getValue("PageType");
    QString ptype = c;
    free(c);
    if (ptype[0] == 'T' || ptype[0] == 't') {
      char * c = opt->getValue("PageName");
      i->pages.append(NVBPageInfo(c, NVB::TopoPage, QSize(opt->getIntValue("PageNX"),opt->getIntValue("PageNY")), QMap<QString,NVBVariant>()));
      free(c);
      }
    else if (ptype[0] == 'S' || ptype[0] == 's') {
      char * c = opt->getValue("PageName");
      i->pages.append(NVBPageInfo(c, NVB::SpecPage, QSize(opt->getIntValue("PageNX"),opt->getIntValue("PageNC")), QMap<QString,NVBVariant>()));
      free(c);
      }
    else throw nvberr_invalid_format;
    }

  delete confile;

  return i;
}

TtextSTMTopoPage::TtextSTMTopoPage(TConSect * sect):NVB3DPage()
{
  if (!(sect->hasPair("PageName")))
    throw nvberr_invalid_format;

  pagename = sect->getValue("PageName");

  TLispInt * evaluator = new TLispInt(sect->getValue("PageFormula"));
  setComment("Formula",QString(sect->getValue("PageFormula")));

  xd = NVBDimension(sect->getValue("PageDX"));
  yd = NVBDimension(sect->getValue("PageDY"));
  zd = NVBDimension(sect->getValue("PageDZ"));

  int nx = sect->getIntValue("PageNX");
  int ny = sect->getIntValue("PageNY");

  _resolution = QSize(nx,ny);

  double x0 = sect->getDoubleValue("PageX0");
  double lx = sect->getDoubleValue("PageLX");
  double y0 = sect->getDoubleValue("PageY0");
  double ly = sect->getDoubleValue("PageLY");

  _position = QRectF(x0,y0,lx,ly);

  double * pdata = (double*)calloc(nx*sizeof(double),ny);

  for (int i=0; i<ny; i++)
    for (int j=0; j<nx; j++)
      pdata[i*nx+j] = evaluator->evaluate(x0+j*lx/nx,y0+i*ly/ny);

  setData(pdata);

  delete evaluator;

  setComment("Bias",NVBPhysValue(sect->getValue("PageBias")+QString(" V")));
  setComment("Setpoint",NVBPhysValue(sect->getValue("PageSetpoint")+QString(" A")));

  char * ptype = sect->getValue("PageColorType");
  switch (ptype[0]) {
    case ('R') :
    case ('r') : // Ramp
      {
        colors = new NVBGrayRampContColorModel(sect->getDoubleValue("PageColorC0"),sect->getDoubleValue("PageColorC1"),sect->getDoubleValue("PageColorZ0"),sect->getDoubleValue("PageColorZ1"));
        break;
      }
    case ('C') :
    case ('c') : // Const
      {
        colors = new NVBConstantContColorModel(sect->getIntValue("PageColor"));
        break;
      }
    default :
      {
        break;
      }
    }
}

TtextSTMSpecPage::TtextSTMSpecPage(TConSect * sect):NVBSpecPage()
{
  if (!(sect->hasPair("PageName")))
    throw nvberr_invalid_format;

  pagename = sect->getValue("PageName");

  TLispInt * evaluator = new TLispInt(sect->getValue("PageFormula"));
  setComment("Formula",QString(sect->getValue("PageFormula")));

  // TODO Put PageDX and PageDY in all the testSTM files
  xd = NVBDimension();
  yd = NVBDimension();
  td = NVBDimension(sect->getValue("PageDX"));
  zd = NVBDimension(sect->getValue("PageDZ"));

  int nx = sect->getIntValue("PageNX");
  int ny = sect->getIntValue("PageNC");

  _datasize = QSize(nx,ny);

  xdata = (double*)calloc(sizeof(double),nx);
  if (!xdata) throw nvberr_not_enough_memory;
  ydata = (double*)calloc(nx*sizeof(double),ny);
  if (!ydata) { free(xdata); throw nvberr_not_enough_memory; }

  QList<QwtData*> pdata;

  double x0 = sect->getDoubleValue("PageX0");
  double lx = sect->getDoubleValue("PageLX");

  for (int j=0; j<nx; j++)
    xdata[j] = x0+j*lx/nx;

  for (int i=0; i<ny; i++) {
    for (int j=0; j<nx; j++)
      ydata[i*nx+j] = evaluator->evaluate(x0+j*lx/nx,i);
    pdata.append(new QwtCPointerData(xdata,ydata+i*nx,nx));
    }

  setData(pdata);

  delete evaluator;

  char * ptype = sect->getValue("PagePosType");
  switch (ptype[0]) {
    case ('G') :
    case ('g') : // Grid
      {
        double px0 = sect->getDoubleValue("PagePosX0");
        double py0 = sect->getDoubleValue("PagePosX0");
        int pnx = sect->getIntValue("PagePosNX");
        int pny = sect->getIntValue("PagePosNY");
        double plx = sect->getDoubleValue("PagePosLX");
        double ply = sect->getDoubleValue("PagePosLY");
        for (int i=0; i<pny; i++) {
          for (int j=0; j<pnx; j++) {
          _positions.append(QPointF(px0 + j*plx/(pnx-1), py0 + i*ply/(pny-1)));
          }}
        break;
      }
    case ('S') :
    case ('s') : // Set
      {
        for (int i=0; i<ny; i++)
          _positions.append(QPointF(sect->getDoubleValue(QString("PagePosX%1").arg(i).toLatin1().data()),sect->getDoubleValue(QString("PagePosY%1").arg(i).toLatin1().data())));
        break;
      }
    default :
      {
        break;
      }
    }

  ptype = sect->getValue("PageColorType");
  switch (ptype[0]) {
/*    case ('R') :
    case ('r') : // Ramp
      {
        colors = new NVBRGBRampDiscrColorModel(0x010101*(char)(0xFF*sect->getDoubleValue("PageColorC0")),
        0x010101*(char)(0xFF*sect->getDoubleValue("PageColorC1")));
        break;
      }*/
    case ('C') :
    case ('c') : // Const
      {
        colors = new NVBConstDiscrColorModel(0x010101*(char)(0xFF*sect->getDoubleValue("PageColor")));
        break;
      }
    default :
      {
        throw nvberr_unexpected_value;
      }
    }  
}

TtextSTMSpecPage::~ TtextSTMSpecPage()
{
  if (xdata) free(xdata);
  if (ydata) free(ydata);
}

void TtextSTMTopoPage::commit() {
}

void TtextSTMSpecPage::commit() {
}

Q_EXPORT_PLUGIN2(textSTM, NVBTextSTMFileGenerator)
