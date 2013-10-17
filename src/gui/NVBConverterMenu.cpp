//
// Copyright 2013 Timofey <typograph@elec.ru>
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

#include "NVBConverterMenu.h"

#include "NVBDirViewModel.h"
#include "NVBDataTransforms.h"
#include "NVBDataCore.h"
#include "NVBAxisSelector.h"

class NVBShowFirstTopoPageConverter : public NVBFile2ImageConverter {
	public:
		NVBShowFirstTopoPageConverter() : NVBFile2ImageConverter() {
			selector.addCase().setDataType(NVBDataSet::Topography);
			selector.addCase().setDataMinAxes(2);
			}
	private:
		NVBAxisSelector selector;
		virtual QPixmap convertToImage(NVBFile * file) const {
			NVBSelectorDataInstance d = selector.instantiateOneDataset(file);
			if (!d.isValid())
				return QPixmap();

			return NVBDataColorInstance::colorize(d.matchingData());
			}
	};

class NVBShowFirstSpecPageConverter : public NVBFile2ImageConverter {
	public:
		NVBShowFirstSpecPageConverter() : NVBFile2ImageConverter() {
			selector.addCase().setDataType(NVBDataSet::Spectroscopy);
			selector.addCase().setDataMinAxes(1);
			}
	private:
		NVBAxisSelector selector;
		virtual QPixmap convertToImage(NVBFile * file) const {
			NVBSelectorDataInstance d = selector.instantiateOneDataset(file);
			if (!d.isValid())
				return QPixmap();

			return NVBDataColorInstance::colorize(d.matchingData());
			}
	};
	
/** \deprecated
 * This class is here purely for historical reasons. It is not used anywhere
 */
	
	
class NVBDataSet34Substractor : public NVBFile2ImageConverter {
	private:
		virtual QPixmap convertToImage(NVBFile * file) const {
			if (file->isEmpty() || file->first()->dataSets().isEmpty())
				return QPixmap();

			if (file->first()->dataSets().count() != 8 ||
			    file->first()->dataSets().at(2)->type() != NVBDataSet::Topography ||
			    file->first()->dataSets().at(3)->type() != NVBDataSet::Topography)
				return NVBDataColorInstance::colorize(file->first()->dataSets().first());

			NVBDataSet * d3 = file->first()->dataSets().at(2);
			NVBDataSet * d4 = file->first()->dataSets().at(3);
			NVBColorInstance * ci = new NVBColorInstance(d3->colorMap());
			axissize_t sz = prod(d3->sizes());
			double * data = (double*) malloc(sz * sizeof(double));
			QSize isize = QSize(d3->sizeAt(0), d3->sizeAt(1));
			memcpy(data, d3->data(), sz * sizeof(double));
//		axissize_t w = d3->sizeAt(0);
//		axissize_t mi = NVBMaxMinTransform::max_index(d4->data()+10, 1, &w)+10;
//		double factor = (d3->data()[mi] - (d3->data()[mi+9] + d3->data()[mi-10])/2)/(d4->data()[mi] - (d4->data()[mi+9] + d4->data()[mi-10])/2);
			axissize_t mi = NVBMaxMinTransform::max_index(d4->data(), 1, &sz);
			double factor = 0;

			if (mi < 10)
				factor = (d3->data()[mi] - d3->data()[mi + 9]) / (d4->data()[mi] - d4->data()[mi + 9] );
			else
				factor = (d3->data()[mi] - (d3->data()[mi + 9] + d3->data()[mi - 10]) / 2) / (d4->data()[mi] - (d4->data()[mi + 9] + d4->data()[mi - 10]) / 2);

			for(axissize_t i = 0; i < sz; i++)
				data[i] -= factor * d4->data()[i];

			ci->autoscale(data, sz);
			QPixmap result = ci->colorize(data, isize, QSize(256, 256));
			free(data);
			delete(ci);
			return result;
			}
	};

NVBConverterMenu::~NVBConverterMenu()
{
	foreach(NVBFile2ImageConverter * c, converters)
		delete c;

}

NVBConverterMenu::NVBConverterMenu(QWidget* parent)
{
	group = new QActionGroup(this);
	connect(&mapper,SIGNAL(mapped(int)),this,SLOT(selected(int)));
	addConverter("Show first page", 0)->setChecked(true); // Default converter, implemented in NVBDirViewModel.cpp
	addConverter("Show first topographic page", new NVBShowFirstTopoPageConverter());
	addConverter("Show first spectroscopic page", new NVBShowFirstSpecPageConverter());
// TODO allow custom converters in python
// 	addSeparator();
// 	addAction("More...",this,SLOT(createCustomConverter()));
	
}

QAction * NVBConverterMenu::addConverter(QString caption, NVBFile2ImageConverter* converter)
{
	QAction * a = addAction(caption, &mapper, SLOT(map()));
	a->setCheckable(true);
	group->addAction(a);
	mapper.setMapping(a,converters.count());
	converters << converter;
	return a;
}

void NVBConverterMenu::selected(int index)
{
	emit converterSelected(converters.at(index));
}

