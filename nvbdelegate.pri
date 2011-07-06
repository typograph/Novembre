include(nvb.pri)

TEMPLATE = lib
MOC_DIR = moc
OBJECTS_DIR = objPIC
CONFIG += plugin rtti
contains(CONFIG,NVBStatic) {
 CONFIG -= dll
 CONFIG += static
} else {
 CONFIG += dll
 target.path = $$NVB_PLUGIN_INSTALL_PATH/tools
 INSTALLS += target
}
