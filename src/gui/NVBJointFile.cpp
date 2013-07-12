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
#include "NVBJointFile.h"
#include "../core/NVBPages.h"
#include "../core/NVBDiscrColoring.h"

class NVBJointSpecPage : public NVBSpecPage {
public:
	NVBJointSpecPage(const NVBSpecDataSource * master) {
		if (!master) {
			NVBOutputError("The page is not a specpage");
			return;
			}
		pagename = master->name();
		_positions.append(master->positions());
		foreach(QwtData* cdata, master->getData())
			_data.append(copy_curve(cdata));
		_datasize = master->datasize();
		xd = master->xDim();
		yd = master->yDim();
		zd = master->zDim();
		td = master->tDim();
		setColorModel(new NVBRandomDiscrColorModel(_datasize.height()));
		}

	~NVBJointSpecPage() {
		}
		
	void addCurves(const NVBSpecDataSource* other) {
		_positions.append(other->positions());
		foreach(QwtData* cdata, other->getData())
			_data.append(copy_curve(cdata));
		_datasize.rheight() += other->datasize().height();
		dynamic_cast<NVBRandomDiscrColorModel*>(colors)->resize(_datasize.height());		
		}
		
	QwtData* copy_curve(const QwtData* other) {
		QwtArray<double> x, y;
		for(unsigned int i=0; i<other->size(); i++) {
			x.append(other->x(i));
			y.append(other->y(i));
			}
		return new QwtArrayData(x,y);
		}
};

NVBJointFile::NVBJointFile(const NVBAssociatedFilesInfo & sources, NVBFile* master)
	: NVBFile(sources) {
	master->use();
	for (int i = 0; i < master->rowCount(); i++) {
		NVBDataSource * page = master->data(master->index(i), PageRole).value<NVBDataSource*>();
		if (page->type() == NVB::SpecPage) {
			addSource(new NVBJointSpecPage(dynamic_cast<NVBSpecDataSource*>(page)));
			}
		else
			NVBOutputPMsg(QString("Skipping page %1").arg(page->name()));
		}
	master->release();
	}

bool NVBJointFile::addFile(NVBFile * other) {
	other->use();
	QList<NVBSpecDataSource*> goods;
	int i = 0;
	for (int j = 0; j < other->rowCount(); j++) {
		NVBDataSource * page = other->data(other->index(j), PageRole).value<NVBDataSource*>();
		if (page->type() == NVB::SpecPage) {
			NVBSpecDataSource * ssource = dynamic_cast<NVBSpecDataSource*>(page);
			if (!ssource) continue;
			NVBSpecDataSource * cmpsource = dynamic_cast<NVBSpecDataSource*>(pages.at(i));
			if (ssource->name() == cmpsource->name() && 
				  ssource->tDim().isComparableWith(cmpsource->tDim()) &&
				  ssource->zDim().isComparableWith(cmpsource->zDim()) &&
				  ssource->getData().first()->x(0) == cmpsource->getData().first()->x(0) &&
				  ssource->getData().first()->x(1) == cmpsource->getData().first()->x(1) &&
					ssource->datasize().width() == cmpsource->datasize().width()) {
				NVBOutputVPMsg(QString("Page %1 matched").arg(ssource->name()));
				goods.append(ssource);
				i += 1;
				if (i == pages.length()) break;
				}
			else
				NVBOutputVPMsg(QString("Page %1 didn't match (%1 %2 %3 %4) vs (%5 %6 %7 %8)").arg(ssource->name(),ssource->tDim().toStr(),ssource->zDim().toStr()).arg(ssource->datasize().width()).arg(cmpsource->name(),cmpsource->tDim().toStr(),cmpsource->zDim().toStr()).arg(cmpsource->datasize().width()));
			}
		else
			NVBOutputPMsg(QString("Skipping page %1").arg(page->name()));
		}
	
	if (goods.length() != pages.length()) {
		NVBOutputPMsg(QString("Not enough matching pages %1/%2").arg(goods.length()).arg(pages.length()));
		other->release();
		return false;
		}
	
	for(i--;i>=0;i-=1)
		dynamic_cast<NVBJointSpecPage*>(pages[i])->addCurves(goods.at(i));
	
	other->release();
	return true;
	}
