# Test harness for arbitrary file generators

GNAME = nanonis
GCLASSNAME = NanonisFileGenerator

HEADERS += ../../src/files/$${GNAME}.h
SOURCES += ../../src/files/$${GNAME}.cpp
QMAKE_CXXFLAGS += "-include ../../src/files/$${GNAME}.h"

DEFINES += TESTGENERATOR=$${GCLASSNAME}

# end of serviceable part

DEFINES += FILEGENERATOR_NO_GUI NVB_ENABLE_LOG

HEADERS += \
	../../src/core/NVBContColoring.h \
	../../src/core/NVBContColorModel.h \
	../../src/core/NVBDiscrColoring.h \
	../../src/core/NVBDiscrColorModel.h \
	../../src/core/NVBDataSource.h \
	../../src/core/NVBPages.h \
	../../src/core/NVBPageViewModel.h \
	../../src/core/NVBFileInfo.h \
	../../src/core/NVBFile.h \
	../../src/core/NVBLogger.h \
	../../src/core/NVBDimension.h \
	../../src/core/NVBVariant.h \
	../../src/core/NVBTokens.h \
	../../src/core/NVBMimeData.h \
	../../src/core/dimension.h \
	../../src/gui/NVBSettings.h \
#	../../src/gui/NVBToolsFactory.h \
	testGenerator.h

SOURCES += \
	../../src/core/NVBContColoring.cpp \
	../../src/core/NVBDiscrColoring.cpp \
	../../src/core/NVBDataSource.cpp \
	../../src/core/NVBPages.cpp \
	../../src/core/NVBPageViewModel.cpp \
	../../src/core/NVBFileInfo.cpp \
	../../src/core/NVBFile.cpp \
	../../src/core/NVBLogger.cpp \
	../../src/core/NVBDimension.cpp \
	../../src/core/NVBVariant.cpp \
	../../src/gui/NVBSettings.cpp \
	../../src/core/NVBTokens.cpp \
	../../src/core/NVBMimeData.cpp \
	../../src/core/dimension.cpp \
#	../../src/NVBToolsFactory.cpp \
	testGenerator.cpp

CONFIG += qt debug
QT = core gui

INCLUDEPATH += /usr/include/qwt5
INCLUDEPATH += ../../src
LIBS += -lqwt

OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = .


