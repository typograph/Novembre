#include "textSTM.h"
#include <QtCore/QSettings>
#include "NVBDataSource.h"
#include "NVBAxisMaps.h"
#include <QtCore/QStack>
#include <QtCore/QMap>
#include <QtCore/QtAlgorithms>

enum FTokenType { Number, Variable, Function };
enum FTokenFunction { Add, Subtract, Multiply, Divide, Cosine, Sine };

struct FToken {
	FTokenType type;
	union {
		FTokenFunction fntype;
		double value;
		axisindex_t axis;
		};

	FToken(FTokenType t, FTokenFunction fn)
		: type(t)
		, fntype(fn)
		{;}
	FToken(FTokenType t, double v)
		: type(t)
		, value(v)
		{;}
	FToken(FTokenType t, axisindex_t a)
		: type(t)
		, axis(a)
		{;}
};

class STMCalculator {
private:
	static QStringList functions;
	QList<FToken> tokenizeFormula(QString formula);
	double valueAt(QVector<axissize_t> ixs, QList<FToken> tokens);
	QStringList axisNames;
	QVector<axissize_t> axisLengths;
	QVector<axisindex_t> usedAxes;
public:
	void addAxis(QString variable, axissize_t length);
	double * calculateData(QString formula);
	QVector<axisindex_t> sliceAxes();
};

QStringList STMCalculator::functions = QStringList("+") << "-" << "*" << "/" << "cos" << "sin";

void addAxis(QString variable, axissize_t length) {
	axisNames << variable;
	axisLenghts << length;
}

QList<FToken> STMCalculator::tokenizeFormula(QString formula) {
	QStringList tokens = formula.split(' ',QString::SkipEmptyParts);
	QList<FToken> result;
	usedAxes.clear();

	foreach(QString token, tokens){
		bool ok;
		double v = token.toDouble(&ok);
		if(ok) {
			result << FToken(Number,v);
			continue;
			}
		axisindex_t ai = (axisindex_t)axisNames.indexOf(token);
		if (ai >= 0) {
			result << FToken(Var, ai);
			if (!usedAxes.contains(ai))
				usedAxes << ai;
			continue;
			}
		int fn = functions.indexOf(token);
		if (fn >= 0) {
			result << FToken(Function,(FTokenFunction)fn);
			continue;
			}
		NVBOutputError(QString("Unrecognizable token %1").arg(token));
		}

	qSort(usedAxes);

	return result;
}

double STMCalculator::valueAt(QVector<axissize_t> ixs, QList<FToken> tokens) {

	QStack<double> calculation;

	foreach(FToken token, tokens) {
		switch(token.type) {
			case Number: {
				calculation.push(token.value);
				break;
				}
			case Variable: {
				calculation.push(ixs.at(token.axis));
				break;
				}
			case Function: {
				switch (token.fntype) {
					case Add : {
						calculation.push(calculation.pop()+calculation.pop());
						break;
						}
					case Subtract : {
						calculation.push(-calculation.pop()+calculation.pop());
						break;
						}
					case Multiply : {
						calculation.push(calculation.pop()*calculation.pop());
						break;
						}
					case Divide : {
						calculation.push(1/calculation.pop()*calculation.pop());
						break;
						}
					case Cosine : {
						calculation.push(cos(calculation.pop()));
						break;
						}
					case Sine : {
						calculation.push(sin(calculation.pop()));
						break;
						}
					default:
						NVBOutputError(QString("Unknown function type %1").arg(token.fntype));
					}
				}
			default:
				NVBOutputError(QString("Unknown token type %1").arg(token.type));
			}
		}

	if (calculation.count() != 1)
		NVBOutputError("Formula incomplete");

	return calculation.first();
}

double * STMCalculator::calculateData(QString formula) {

	QList<FToken> tokens = tokenizeFormula(formula);

	QVector<axissize_t> ixs(axes.count(),0);

	double * target = (double*)malloc(
			subprod(
					axisLengths.constData(),
					axisLengths.count(),
					usedAxes.constData()
					)
			*sizeof(double));

// Using the sliceNArray trick

	axissize_t indT = 0;

	QVector<axissize_t> at(usedAxes.count());
	at[0] = 1;
	for(int i = 1; i < usedAxes.count(); i++)
		at[i] = at.at(i-1)*axisLengths.at(sliceAxes.at(i));

	calculateData_cycle:
		target[indT] = valueAt(ixs,tokens);

		for(int i = 0; i < axes.count(); i++)
			if (ixs[i] == axisLengths.at(sliceAxes.at(i)) - 1) {
				ixs[i] = 0;
				indT -= at.at(i)*(axisLengths.at(sliceAxes.at(i)) - 1);
				}
			else {
				ixs[i] += 1;
				indT += at.at(i);
				goto calculateData_cycle;
				}

	return target;
}

virtual NVBFile * TestSTMFileGenerator::loadFile(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	STMCalculator calculator;
	QSettings stmfile = QSettings(info.first(),QSettings::IniFormat);

	int npages = stmfile.value("NPages",0).toInt();
	if (npages < 1) return 0;

	NVBFile * f = 0;

	try {
		f = new NVBFile(info);
		}
	catch (...) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	for (int i = 1; i <= npages; i+=1) {
		stmfile.beginGroup(QString("Page%1").arg(i));
		int nsets = stmfile.value("NSets",0).toInt();
		int naxes = stmfile.value("NAxes",0).toInt();
		if (naxes > 0 && nsets > 0) {
			NVBDataSource * page = new NVBDataSource();
			for (int axis = 1; axis <= naxes; axis+=1) {
				stmfile.beginGroup(QString("Axis%1").arg(axis));
				page->addAxis(stmfile.value("Name").toString(),stmfile.value("Length").toInt());
				calculator.addAxis(stmfile.value("Var").toString(),stmfile.value("Length").toInt());
				if (stmfile.contains("Dim"))
					page->addAxisMap(
							new NVBAxisPhysMap(
									stmfile.value("Origin",0).toDouble(),
									stmfile.value("Factor",1).toDouble(),
									stmfile.value("Dim").toString()
									)
							);
				stmfile.endGroup();
				}
			for (int set = 1; set <= nsets; set+=1) {
				stmfile.beginGroup(QString("Set%1").arg(set));
				page->addDataSet(
						stmfile.value("Name").toString(),
						calculator.calculateData(stmfile.value("Formula").toString()),
						NVBDimension(stmfile.value("Dim").toString()),
						calculator.sliceAxes()
						);
				stmfile.endGroup();
				}
			f->addSource(page);
			}
		else
			NVBOutputError(QString("Incomplete axes & sets info in page %1").arg(i));
		stmfile.endGroup();
		}

	return f;

}

virtual NVBFileInfo * TestSTMFileGenerator::loadFileInfo(const NVBAssociatedFilesInfo & info) const throw() {
	if (info.generator() != this) {
		NVBOutputError("Associated files provided by other generator");
		return 0;
		}

	if (info.count() == 0) {
		NVBOutputError("No associated files");
		return 0;
		}

	QSettings stmfile = QSettings(info.first(),QSettings::IniFormat);

	int npages = stmfile.value("NPages",0).toInt();
	if (npages < 1) return 0;

	NVBFileInfo * fi = 0;

	try {
		fi = new NVBFileInfo(info);
		}
	catch (...) {
		NVBOutputError("Memory allocation failed");
		return 0;
		}

	for (int i = 1; i <= npages; i+=1) {
		stmfile.beginGroup(QString("Page%1").arg(i));
		int nsets = stmfile.value("NSets",0).toInt();
		int naxes = stmfile.value("NAxes",0).toInt();
		if (naxes > 0 && nsets > 0) {
			QVector<axissize_t> sizes;
			for (int axis = 1; axis <= naxes; axis+=1) {
				stmfile.beginGroup(QString("Axis%1").arg(axis));
				sizes << stmfile.value("Length").toInt();
				stmfile.endGroup();
				}
			for (int set = 1; set <= nsets; set+=1) {
				stmfile.beginGroup(QString("Set%1").arg(set));
				fi->dataInfos << NVBDataInfo(
						stmfile.value("Name").toString(),
						stmfile.value("Dim").toString(),
						sizes,
						NVBDataComments()
						);
				stmfile.endGroup();
				}
			}
		else
			NVBOutputError(QString("Incomplete axes & sets info in page %1").arg(i));
		stmfile.endGroup();
		}

	return fi;
}

virtual QStringList TestSTMFileGenerator::availableInfoFields() const {
	return QStringList() \
					<< "FileComment" \
					;
}
