//
// Copyright 2006 Timofey <typograph@elec.ru>
//
// This file is part of Novembre data analysis program.
//
// Novembre is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "nanonis.h"
#include "../core/NVBDiscrColoring.h"
#include "../core/NVBFileInfo.h"
#include "../core/NVBFile.h"
#include <QDebug>
#include <QTextStream>
#include <QStringList>

class NVBExpandableSpecPage : public NVBSpecPage {
	public:
		NVBExpandableSpecPage(QString name, QString taxis): NVBSpecPage() {
			_datasize = QSize(0, 0);
			pagename = name.left(name.lastIndexOf(' '));
			zd = NVBDimension(name.mid(name.lastIndexOf('(') + 1, name.lastIndexOf(')') - name.lastIndexOf('(') - 1));
			xd = NVBDimension("m");
			yd = NVBDimension("m");
			td = NVBDimension(taxis.mid(taxis.lastIndexOf('(') + 1, taxis.lastIndexOf(')') - taxis.lastIndexOf('(') - 1));
			setComment("X axis label", taxis.left(taxis.lastIndexOf(' ')));
			setComment("Y axis label", pagename);
			setColorModel(new NVBRandomDiscrColorModel());
			}

		void addNewSpecPoint(double x, double y, QwtData * pdata) {
			if (! _datasize.isNull() && (int)pdata->size() != _datasize.width()) {
				NVBOutputError(QString("Wrong size of added data: %1 when main size is %2").arg(pdata->size()).arg(_datasize.width()));
				return;
				};

			emit dataAboutToBeChanged();

			_positions << QPointF(x, -y);

			_data << pdata;

			_datasize.rheight() += 1;

			_datasize.rwidth() = pdata->size();

			getMinMax();

			emit dataChanged();
			};
	};

NVBFile * NanonisFileGenerator::loadSpectrum(const NVBAssociatedFilesInfo & info) const {
	QList<NVBExpandableSpecPage*> pages;

	QFile f(info.first());

	if (! f.open(QIODevice::ReadOnly)) {
		NVBOutputFileError(&f);
		return 0;
		}

	QTextStream specdata(&f);

	QString s = specdata.readLine();
	QMap<QString, NVBVariant> comments;
	
	double xpos, ypos;

//      double zpos = s.mid(s.indexOf('\t')).toDouble();
	while (s != "[DATA]") {
		if (specdata.atEnd()) {
			NVBOutputError("File format error: no DATA section");
			return 0;
			}
		else {
			QStringList dat_comment = s.split('\t');
			if (dat_comment.size() > 1) {
				QString name = dat_comment.first();
				QString value = dat_comment.at(1);
				if (name == "Date")
					comments.insert(name, QDateTime::fromString(value,"dd.MM.yyyy HH:mm:ss"));
				else if (name == "Sample Period (ms)")
					comments.insert("Sample Period",NVBPhysValue(value.toDouble(),NVBDimension("ms")));
				else if (name == "X (m)")
					xpos = value.toDouble();
				else if (name == "Y (m)")
					ypos = value.toDouble();
                else if (name.startsWith("Comment")) {
                    if (comments.contains("User comment"))
                        comments.insert("User comment",
                                        comments.value("User comment") + "\n" + value);
                    else
                        comments.insert("User comment", value);
                    }
				else
					comments.insert(name,value);
				}
			s = specdata.readLine();
			}
		}

	QStringList names = specdata.readLine().split('\t', QString::SkipEmptyParts);
    
/* If "save all" is selected in Nanonis,
 * several curves with names "prefixes [00001] suffixes"
 * exist. Every series should be collected to the same page. 
 */

    // Names of final datasets
    QStringList finalNames;
    
    // Indexes of final names from original names
    QList<int> nameIndex;
    
    QRegExp nameRE("(.*)\\s*\\[\\d{5}\\]\\s*(.*)");

    foreach(QString name, names) {
        if (nameRE.exactMatch(name)) {
            QString setName = nameRE.cap(1) + " " + nameRE.cap(2);
            int ni = finalNames.indexOf(setName);
            if (ni < 0) {
                nameIndex.append(finalNames.count());
                finalNames.append(setName);
                }
            else
                nameIndex.append(ni);
            }
        else {
            nameIndex.append(finalNames.count());
            finalNames.append(name);
            }
        }

    foreach (QString name, finalNames)
         // TODO We assume Bias Calc here
		pages << new NVBExpandableSpecPage(name, names.first());

	QList<QVector<double> > cdata;

	for (int i = 0; i < names.count(); i++)
		cdata << QVector<double>();

	while (!specdata.atEnd()) {
		QStringList vdata = specdata.readLine().split('\t', QString::SkipEmptyParts);

		if (vdata.count() > cdata.count()) {
			NVBOutputError(QString("Unexpected columns.\nExpected : %1\nGot : %2").arg(names.count()).arg(vdata.count()));

			for (int j = 0; j < cdata.size(); j++)
				cdata[j] << vdata.at(j).toDouble();
			}
		else if (vdata.count() < cdata.count()) {
			NVBOutputError(QString("Missing columns.\nExpected : %1\nGot : %2").arg(names.count()).arg(vdata.count()));

			for (int j = 0; j < vdata.size(); j++)
				cdata[j] << vdata.at(j).toDouble();

			for (int j = vdata.size(); j < cdata.size(); j++)
				cdata[j] << 0;
			}
		else
			for (int j = 0; j < vdata.size(); j++)
				cdata[j] << vdata.at(j).toDouble();
		}

    int bias_calc_index = -1;
    for (int i = 0; i < names.count(); i++)
        if (names.at(i) == "Bias calc (V)") {
            bias_calc_index = i;
            break;
            }
        else
            qDebug() << names.at(i);        
		
	QVector<double> tdata;
    if (bias_calc_index >= 0)
        tdata = cdata.at(bias_calc_index);
    else {
        tdata.reserve(cdata.at(0).count());
        for (int i = 0; i < cdata.at(0).count(); i++)
            tdata << i;
        }
            
    for (int i = 0; i < cdata.count(); i++) {
        pages[nameIndex.at(i)]->addNewSpecPoint(xpos, ypos, new QwtArrayData(tdata, cdata.at(i)));
    }

	// TODO Do something with z;

	NVBFile * result = new NVBFile(info);

	foreach(NVBExpandableSpecPage * p, pages)
		result->addSource(p);

	return result;
	}

NVBFileInfo * NanonisFileGenerator::loadSpectrumInfo(const NVBAssociatedFilesInfo & info) const {
	NVBFileInfo * fi = new NVBFileInfo(info);

	if (!fi) {
		NVBOutputError("NVBFileInfo creation failed");
		return 0;
		}

	QFile f(info.first());

	if (!(f.open(QIODevice::ReadOnly))) {
		NVBOutputFileError(&f);
		return 0;
		}

	QTextStream first(&f);
    QString s = first.readLine();
    QMap<QString, NVBVariant> comments;
    
    while (s != "[DATA]") {
        if (first.atEnd()) {
            NVBOutputError("File format error: no DATA section");
            return 0;
            }
        else {
            QStringList dat_comment = s.split('\t');
            if (dat_comment.size() > 1) {
                QString name = dat_comment.first();
                QString value = dat_comment.at(1);
                if (name == "Date")
                    comments.insert(name, QDateTime::fromString(value,"dd.MM.yyyy HH:mm:ss"));
                else if (name == "Sample Period (ms)")
                    comments.insert("Sample Period",NVBPhysValue(value.toDouble(),NVBDimension("ms")));
                else if (name == "Bias>Bias (V)")
                    comments.insert("Bias",NVBPhysValue(value.toDouble(),NVBDimension("V")));
                else if (name.startsWith("Comment")) {
                    if (comments.contains("User comment"))
                        comments.insert("User comment",
                                        comments.value("User comment") + "\n" + value);
                    else
                        comments.insert("User comment", value);
                    }
                else
                    comments.insert(name,value);
                }
            s = first.readLine();
            }
        }    


	QStringList names = first.readLine().split('\t', QString::SkipEmptyParts);
	int nxs = 0;

	while (!first.atEnd()) {
		first.readLine();
		nxs += 1;
		}

    // Names of final datasets
    QStringList finalNames;
    
    // Indexes of final names from original names
    QList<int> nameIndex;
    QList<int> nameCounts;
    
    QRegExp nameRE("(.*)\\s*\\[\\d{5}\\]\\s*(.*)");

    foreach(QString name, names) {
        if (nameRE.exactMatch(name)) {
            QString setName = nameRE.cap(1) + " " + nameRE.cap(2);
            int ni = finalNames.indexOf(setName);
            if (ni < 0) {
                nameIndex.append(finalNames.count());
                finalNames.append(setName);
                nameCounts.append(1);
                }
            else {
                nameIndex.append(ni);
                nameCounts[ni] += 1;
                }
            }
        else {
            nameIndex.append(finalNames.count());
            finalNames.append(name);
            nameCounts.append(1);
            }
        }		
		
	for (int i = 0; i < finalNames.count(); i++) {
//        qDebug() << nameCounts.at(i);
		fi->pages.append(NVBPageInfo(finalNames.at(i), NVB::SpecPage, QSize(nxs, nameCounts.at(i)), NVBPhysValue(nxs, NVBDimension()), NVBPhysValue(), comments));
    }

	f.close();

	return fi;

	}

