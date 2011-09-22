# Test harness for arbitrary file generators

HEADERS += ../nanonis.h
SOURCES += ../nanonis.cpp
QMAKE_CXXFLAGS += '-include ../nanonis.h'

DEFINES += TESTGENERATOR=NanonisFileGenerator

# end of serviceable part

DEFINES += \
	NVB_DEBUG \
	NVB_ENABLE_LOG \
	NVB_VERBOSE_LOG 

HEADERS += \
	../NVBDataSource.h \
	../NVBDataSourceModel.h \
	../NVBFileInfo.h \
	../NVBFile.h \
	../NVBAxis.h \
	../NVBAxisSelectorHelpers.h \
	../NVBAxisSelector.h \
	../NVBScaler.h \
	../NVBAxisMaps.h \
	../NVBColorMaps.h \
	../NVBMap.h \
	../NVBLogger.h \
	../NVBDataTransforms.h \
	../NVBDataCore.h \
	../NVBUnits.h \
	../NVBVariant.h \
	../NVBTokens.h \
	../NVBDatasetIcons.h \
	../NVBMimeData.h \
	../NVBCoreApplication.h \
	testGenerator.h

SOURCES += \
	../NVBDataSource.cpp \
	../NVBDataSourceModel.cpp \
	../NVBFileInfo.cpp \
	../NVBFile.cpp \
	../NVBAxisSelectorHelpers.cpp \
	../NVBAxisSelector.cpp \
	../NVBScaler.cpp \
	../NVBColorMaps.cpp \
	../NVBMap.cpp \
	../NVBLogger.cpp \
	../NVBDataTransforms.cpp \
	../NVBDataCore.cpp \
	../NVBUnits.cpp \
	../NVBVariant.cpp \
	../NVBTokens.cpp \
	../NVBDatasetIcons.cpp \
	../NVBMimeData.cpp \
	../NVBCoreApplication.cpp \
	testGenerator.cpp


HEADERS += \
	../NVBPosLabel.h \
	../NVBSingle2DView.h \
	../NVBPlotCurves.h \
	../NVBPhysScaleDraw.h \
	../NVBSingleGraphView.h \
	../NVBSingleView.h
		

SOURCES += \
	../NVBPosLabel.cpp \
	../NVBSingle2DView.cpp \
	../NVBPlotCurves.cpp \
	../NVBPhysScaleDraw.cpp \
	../NVBSingleGraphView.cpp \
	../NVBSingleView.cpp


LIBS += -lqwt6

CONFIG += qt debug

INCLUDEPATH += /usr/include/qwt6
LIBS += -lqwt6

MOC_DIR = .
DESTDIR = .

DEFINES += \
	NVB_DEBUG \
	NVB_ENABLE_LOG \
	NVB_VERBOSE_LOG 

