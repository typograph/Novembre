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

#ifndef NVBGENERATORSSETTINGSWIDGET_H
#define NVBGENERATORSSETTINGSWIDGET_H

class QListView;
class QPushButton;
class NVBFileFactory;

#include "NVBSettingsWidget.h"

/**
 * A widget for controlling the usage of file generators and their properties
 *
 *
 */

class NVBGeneratorsSettingsWidget : public NVBSettingsWidget {
	private:
		QListView * pluginList;

		NVBFileFactory * ffactory;

		NVBGeneratorsSettingsWidget * pluginWidget;

	private slots:
		void selectGenerator();

	public:
		NVBGeneratorsSettingsWidget();
		virtual ~NVBGeneratorsSettingsWidget() {;}

		/// Copy widget values from an existing QSettings
		virtual void init(QSettings * settings);
		/// Write out all changes into QSettings
		virtual bool write(QSettings * settings);


	};

#endif // NVBGENERATORSSETTINGSWIDGET_H
