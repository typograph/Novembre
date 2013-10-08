//
// Copyright 2013 Timofey <typograph@elec.ru>
//
// This file is part of Novembre utility library.
//
// Novembre utility library is free software: you can redistribute it
// and/or modify it  under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//
// Novembre is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "NVBPlugin.h"
#include "NVBLogger.h"

#ifdef Q_WS_WIN
# include <QtCore/qt_windows.h>
#endif
#include <QtCore/QLibrary>
#include <QtCore/QPointer>
#include <QtCore/QStringList>

#include "NVBFileGenerator.h"

// ----- -NVBFilePluginLoader

NVBFilePluginLoader::NVBFilePluginLoader(const QString& fileName)
	: library(0)
{
	library = new QLibrary(fileName);
	if (library)
		function = (NVBFilePluginInstanceFunction) library->resolve("nvb_plugin_instance");
}

NVBFilePluginLoader::~NVBFilePluginLoader() {
	if (library)
		delete library;
}

QString NVBFilePluginLoader::errorString() const {
	return (!library || library->errorString().isEmpty()) ? "Unknown error" : library->errorString();
}

QString NVBFilePluginLoader::fileName() const {
	if (library)
		return library->fileName();
	return QString();
}

NVBFileGenerator* NVBFilePluginLoader::instance(NVBSettings settings) {
	
	if (!library || !function)
		return 0;
	return (*function)(settings);
}

typedef QList<NVBFilePluginInstanceFunction> StaticInstanceFunctionList;
Q_GLOBAL_STATIC(StaticInstanceFunctionList, staticInstanceFunctionList)

void nvbRegisterStaticPluginInstanceFunction(NVBFilePluginInstanceFunction function)
{
	staticInstanceFunctionList()->append(function);
}

QList< NVBFileGenerator* > NVBFilePluginLoader::staticInstances(NVBSettings settings) {
	QList< NVBFileGenerator* > instances;
	StaticInstanceFunctionList *functions = staticInstanceFunctionList();
	if (functions) {
		for (int i = 0; i < functions->count(); ++i)
			instances.append((*functions)[i](settings));
	}
	return instances;
	
}
