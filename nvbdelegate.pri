include(nvb.pri)

contains(CONFIG,NVBLog) {
    DEFINES += NVB_ENABLE_LOG
    contains(CONFIG,NVBVerboseLog) {
	DEFINES += NVB_VERBOSE_LOG
    }
    HEADERS += src/NVBLogger.h
    SOURCES += src/NVBLogger.cpp
}

HEADERS += \
          src/dimension.h \
	  src/NVBContColorModel.h \
	  src/NVBDiscrColorModel.h \
	  src/NVBDimension.h \
	  src/NVBVariant.h \
	  src/NVBDataSource.h \
	  src/NVBGeneralDelegate.h \
	  src/NVBDelegateProvider.h \
	  src/NVBPageToolbar.h \
	  src/NVBFilterDelegate.h \
	  src/NVBViewController.h

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
