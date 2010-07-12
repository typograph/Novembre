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
    src/NVBDimension.h \
    src/NVBVariant.h \
    src/NVBContColorModel.h \
    src/NVBContColoring.h \
    src/NVBDiscrColorModel.h \
    src/NVBDiscrColoring.h \
    src/NVBDataSource.h \
    src/NVBPages.h \
    src/NVBFileInfo.h \
    src/NVBPageViewModel.h \
    src/NVBFile.h \
    src/NVBFileGenerator.h

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

