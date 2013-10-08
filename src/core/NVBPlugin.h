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

#ifndef NVB_PLUGIN_H
#define NVB_PLUGIN_H

#include "NVBSettings.h"
#include <QtCore/QtPlugin>
#include <QtCore/QLibrary>

//
// Custom macro for plugin export, modelled after Q_EXPORT_PLUGIN2
// Without it, the plugin is not allowed any constructor arguments...
//

#define NVB_EXPORT_FILEPLUGIN(PLUGIN, PLUGINCLASS) \
	Q_PLUGIN_VERIFICATION_DATA \
	Q_EXTERN_C Q_DECL_EXPORT \
	const char * Q_STANDARD_CALL qt_plugin_query_verification_data() { return qt_plugin_verification_data; } \
	Q_EXTERN_C Q_DECL_EXPORT NVBFileGenerator * Q_STANDARD_CALL nvb_plugin_instance(NVBSettings settings) \
	{ return new PLUGINCLASS(settings.group(#PLUGIN)); }

#define NVB_IMPORT_FILEPLUGIN(PLUGIN) \
extern NVBFileGenerator *nvb_plugin_instance_##PLUGIN(); \
class Static##PLUGIN##PluginInstance{ \
	public: \
		Static##PLUGIN##PluginInstance() { \
			nvbRegisterStaticPluginInstanceFunction(nvb_plugin_instance_##PLUGIN); \
			} \
			}; \
			static Static##PLUGIN##PluginInstance static##PLUGIN##Instance;
           
class NVBFileGenerator;
class NVBLibraryPrivate;

typedef NVBFileGenerator *(*NVBFilePluginInstanceFunction)(NVBSettings);

class NVBFilePluginLoader {
	public:
		explicit NVBFilePluginLoader( const QString& fileName );
		~NVBFilePluginLoader();
		
		NVBFileGenerator * instance(NVBSettings settings);
		
		static QList<NVBFileGenerator*> staticInstances(NVBSettings settings);
			
		QString fileName() const;
		
		QString errorString() const;
			
	private:
		QLibrary *library;
		NVBFilePluginInstanceFunction function;
		Q_DISABLE_COPY(NVBFilePluginLoader)
	};         
#endif
