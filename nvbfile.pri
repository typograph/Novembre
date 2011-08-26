include(nvb.pri)

TEMPLATE = lib
MOC_DIR = moc
CONFIG += plugin rtti

contains(CONFIG,NVBShared) {
	CONFIG += dll
	target.path = $$NVB_PLUGIN_INSTALL_PATH/files
	INSTALLS += target
	OBJECTS_DIR = objPIC
} else {
	CONFIG -= dll
	CONFIG += static
	OBJECTS_DIR = obj
}

