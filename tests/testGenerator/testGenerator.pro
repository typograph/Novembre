# Test harness for arbitrary file generators

GNAME = rhk
GCLASSNAME = RHKFileGenerator

HEADERS += ../../src/$${GNAME}.h
SOURCES += ../../src/$${GNAME}.cpp
QMAKE_CXXFLAGS += "-include ../../src/$${GNAME}.h"

DEFINES += TESTGENERATOR=$${GCLASSNAME}

# end of serviceable part

DEFINES += FILEGENERATOR_NO_GUI NVB_ENABLE_LOG

HEADERS += \
	../../src/NVBContColoring.h \
	../../src/NVBContColorModel.h \
	../../src/NVBDiscrColoring.h \
	../../src/NVBDiscrColorModel.h \
	../../src/NVBDataSource.h \
	../../src/NVBPages.h \
	../../src/NVBPageViewModel.h \
	../../src/NVBFileInfo.h \
	../../src/NVBFile.h \
	../../src/NVBLogger.h \
	../../src/NVBDimension.h \
	../../src/NVBVariant.h \
	../../src/NVBTokens.h \
	../../src/NVBMimeData.h \
	../../src/NVBSettings.h \
#	../../src/NVBToolsFactory.h \
	testGenerator.h

SOURCES += \
	../../src/NVBContColoring.cpp \
	../../src/NVBDiscrColoring.cpp \
	../../src/NVBDataSource.cpp \
	../../src/NVBPages.cpp \
	../../src/NVBPageViewModel.cpp \
	../../src/NVBFileInfo.cpp \
	../../src/NVBFile.cpp \
	../../src/NVBLogger.cpp \
	../../src/NVBDimension.cpp \
	../../src/NVBVariant.cpp \
	../../src/NVBSettings.cpp \
	../../src/NVBTokens.cpp \
	../../src/NVBMimeData.cpp \
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


