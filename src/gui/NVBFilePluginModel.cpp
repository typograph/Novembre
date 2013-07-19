//
// Copyright 2012 - 2013 Timofey <typograph@elec.ru>
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

#include "NVBFilePluginModel.h"

#include "NVBFileGenerator.h"
#include <QAction>

NVBFilePluginModel::NVBFilePluginModel()
	: QAbstractTableModel() {

	}

NVBFilePluginModel::~NVBFilePluginModel() {
	beginRemoveRows(QModelIndex(), 0, generators.count() - 1);

	while (generators.count()) {
		delete gactions.takeLast();
		gsource.takeLast();
		delete generators.takeLast();
		}

	endRemoveRows();
	}

int NVBFilePluginModel::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) return 0;

	return generators.count();
	}

int NVBFilePluginModel::columnCount(const QModelIndex &parent) const {
	if (parent.isValid()) return 0;

	return 4;
	}

QVariant NVBFilePluginModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) return QVariant();

	if (role != Qt::DisplayRole || index.column() != 0) return QVariant();

	switch(index.column()) {
		case 0: {
			if (role == Qt::CheckStateRole)
				return gactions.at(index.row())->isChecked() ? Qt::Checked : Qt::Unchecked;
			else
				return QVariant();
			}

		case 1: return generators.at(index.row())->moduleName();

		case 2: return generators.at(index.row())->moduleDesc();

		case 3: return gsource.at(index.row());

		default: return QVariant();
		}
	}

QVariant NVBFilePluginModel::headerData ( int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Vertical || role != Qt::DisplayRole) return QVariant();

	switch(section) {
		case 0: return "Use";

		case 1: return "Name";

		case 2: return "Description";

		case 3: return "Source";

		default: return QVariant();
		}

	}

bool NVBFilePluginModel::isGeneratorActive(int gindex) {
	if (gindex < 0 || gindex >= generators.count()) return false;

	return gactions.at(gindex)->isChecked();
	}

void NVBFilePluginModel::setGeneratorActive(int gindex, bool active) {
	if (gindex < 0 || gindex >= generators.count()) return;

	if (gactions.at(gindex)->isChecked() != active) {
		gactions.at(gindex)->setChecked(active);
		emit dataChanged(index(gindex, 0), index(gindex, 0));
		}
	}

void NVBFilePluginModel::toggleGenerator(int gindex) {
	if (gindex < 0 || gindex >= generators.count()) return;

	gactions.at(gindex)->setChecked(!gactions.at(gindex)->isChecked());
	emit dataChanged(index(gindex, 0), index(gindex, 0));
	}

bool NVBFilePluginModel::isGeneratorActive(NVBFileGenerator *generator) {
	return isGeneratorActive(generators.indexOf(generator));
	}

void NVBFilePluginModel::setGeneratorActive(NVBFileGenerator *generator, bool active) {
	setGeneratorActive(generators.indexOf(generator), active);
	}

void NVBFilePluginModel::toggleGenerator(NVBFileGenerator * generator) {
	toggleGenerator(generators.indexOf(generator));
	}

void NVBFilePluginModel::toggleGenerator(QObject *go) {
	NVBFileGenerator * generator = qobject_cast<NVBFileGenerator*>(go);
	Q_ASSERT(generator);
	toggleGenerator(generator);
	}

QList<NVBFileGenerator*> NVBFilePluginModel::activeGenerators() const {
	QList<NVBFileGenerator*> gs;

	for(int i = 0; i < generators.count(); i++)
		if (gactions.at(i)->isChecked())
			gs << generators.at(i);

	return gs;
	}

bool NVBFilePluginModel::addGenerator(NVBFileGenerator *generator, QString filename) {
	if (!generator) return false;

	beginInsertRows(QModelIndex(), generators.count(), generators.count());
	generators << generator;
	QAction * tAct = new QAction(generator->moduleName(), this);
	tAct->setToolTip(generator->moduleDesc());
	tAct->setCheckable(true);
	tAct->setChecked(true);
	actMapper.setMapping(tAct, (QObject*)generator);
	connect(tAct, SIGNAL(toggled(bool)), &actMapper, SLOT(map()));
	gactions << tAct;
	gsource << filename;
	endInsertRows();
	return true;
	}

Qt::ItemFlags NVBFilePluginModel::flags ( const QModelIndex & index ) const {
	if (index.isValid() && index.column() == 0) return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;

	return QAbstractTableModel::flags(index);
	}

bool NVBFilePluginModel::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (!index.isValid() || index.column() != 0 || role != Qt::CheckStateRole) return false;

	setGeneratorActive(index.row(), static_cast<Qt::CheckState>(value.toUInt()) == Qt::Checked);
	return true;
	}
