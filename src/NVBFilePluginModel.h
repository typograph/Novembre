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

#ifndef NVBFILEPLUGINMODEL_H
#define NVBFILEPLUGINMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSignalMapper>

class QAction;
class NVBFileGenerator;

class NVBFilePluginModel : public QAbstractTableModel {
		Q_OBJECT
	private:
		QList<NVBFileGenerator*> generators;
		QList<QAction*> gactions;
		QList<QString> gsource;
		QSignalMapper actMapper;
	public:
		NVBFilePluginModel();
		~NVBFilePluginModel();

		virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
		virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
		virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

		bool addGenerator(NVBFileGenerator* generator, QString filename = "Built-in");

		QList<NVBFileGenerator*> activeGenerators() const;
		const QList<NVBFileGenerator*> & availableGenerators() const { return generators; }

		const QList<QAction*> & actions() const { return gactions; }

		bool isGeneratorActive(NVBFileGenerator* generator);
		void setGeneratorActive(NVBFileGenerator* generator, bool active = true);

		bool isGeneratorActive(int index);
		void setGeneratorActive(int index, bool active = true);

	public slots:
		void toggleGenerator(QObject* generator);
		void toggleGenerator(NVBFileGenerator* generator);
		void toggleGenerator(int index);

	};

#endif // NVBFILEPLUGINMODEL_H
