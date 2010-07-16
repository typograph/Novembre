include(nvb.pri)

TEMPLATE = lib
MOC_DIR = moc
OBJECTS_DIR = objPIC
CONFIG += plugin rtti

contains(CONFIG,NVBShared) {
 CONFIG += dll
 target.path = $$NVB_PLUGIN_INSTALL_PATH/files
 INSTALLS += target
} else {
 CONFIG -= dll
 CONFIG += static
}

