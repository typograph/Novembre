# Test harness for arbitrary file generators

GNAME = rhk
GCLASSNAME = RHKFileGenerator

HEADERS += ../../src/$${GNAME}.h
SOURCES += ../../src/$${GNAME}.cpp
QMAKE_CXXFLAGS += "-include ../../src/$${GNAME}.h"

DEFINES += TESTGENERATOR=$${GCLASSNAME}

# end of serviceable part

DEFINES += \
        NVB_DEBUG \
        NVB_ENABLE_LOG \
        NVB_VERBOSE_LOG

HEADERS += \
	../../src/NVBDataSource.h \
	../../src/NVBDataSourceModel.h \
	../../src/NVBFileInfo.h \
	../../src/NVBFile.h \
	../../src/NVBAxis.h \
	../../src/NVBAxisSelectorHelpers.h \
	../../src/NVBAxisSelector.h \
	../../src/NVBScaler.h \
	../../src/NVBAxisMaps.h \
	../../src/NVBColorMaps.h \
	../../src/NVBMap.h \
	../../src/NVBLogger.h \
	../../src/NVBDataTransforms.h \
	../../src/NVBDataCore.h \
	../../src/NVBUnits.h \
	../../src/NVBVariant.h \
        ../../src/NVBSettings.h \
        ../../src/NVBTokens.h \
	../../src/NVBDatasetIcons.h \
	../../src/NVBMimeData.h \
	../../src/NVBCoreApplication.h \
        testGeneratorGUI.h

SOURCES += \
	../../src/NVBDataSource.cpp \
	../../src/NVBDataSourceModel.cpp \
	../../src/NVBFileInfo.cpp \
	../../src/NVBFile.cpp \
	../../src/NVBAxisSelectorHelpers.cpp \
	../../src/NVBAxisSelector.cpp \
	../../src/NVBScaler.cpp \
	../../src/NVBColorMaps.cpp \
	../../src/NVBMap.cpp \
	../../src/NVBLogger.cpp \
	../../src/NVBDataTransforms.cpp \
	../../src/NVBDataCore.cpp \
	../../src/NVBUnits.cpp \
	../../src/NVBVariant.cpp \
        ../../src/NVBSettings.cpp \
        ../../src/NVBTokens.cpp \
	../../src/NVBDatasetIcons.cpp \
	../../src/NVBMimeData.cpp \
	../../src/NVBCoreApplication.cpp \
        testGeneratorGUI.cpp

HEADERS += \
        ../../src/NVBPosLabel.h \
        ../../src/NVBSettingsWidget.h \
        ../../src/NVBSingle2DView.h \
        ../../src/NVBPlotCurves.h \
        ../../src/NVBPhysScaleDraw.h \
        ../../src/NVBSingleGraphView.h \
        ../../src/NVBSingleView.h
		

SOURCES += \
        ../../src/NVBPosLabel.cpp \
        ../../src/NVBSettingsWidget.cpp \
        ../../src/NVBSingle2DView.cpp \
        ../../src/NVBPlotCurves.cpp \
        ../../src/NVBPhysScaleDraw.cpp \
        ../../src/NVBSingleGraphView.cpp \
        ../../src/NVBSingleView.cpp


CONFIG += qt debug

INCLUDEPATH += /usr/include/qwt6
INCLUDEPATH += ../../src
LIBS += -lqwt6

OBJECTS_DIR = buildGUI
MOC_DIR = buildGUI
DESTDIR = .


