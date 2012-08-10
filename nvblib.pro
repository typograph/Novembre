CONFIG += NVBLib

include(nvb.pri)

contains(CONFIG,NVB3DView) {
#  CONFIG += gl2ps // Uncomment if you want export to vector formats
  include(qwtplot3d.pro)
  QT += opengl
}

# Log

HEADERS += src/NVBLogger.h

contains(CONFIG,NVBLog){
  SOURCES += src/NVBLogger.cpp
}


contains(CONFIG,NVB2DView) {
HEADERS += \
  src/NVBGraphicsItems.h
SOURCES += \
  src/NVBGraphicsItems.cpp
}

# Basic data structures

HEADERS += \
  src/NVBScaler.h \
  src/NVBUnits.h \
  src/NVBVariant.h \
  src/NVBMap.h \
  src/NVBDataCore.h \
  src/NVBAxis.h \
  src/NVBDataTransforms.h \
  src/NVBDatasetIcons.h \
  src/NVBDataSource.h \
  src/NVBDataSourceModel.h \
  src/NVBAxisSelectorHelpers.h \
  src/NVBAxisSelector.h \
  src/NVBTokens.h \
  src/NVBFileInfo.h \
  src/NVBMimeData.h \
  src/NVBFile.h \

SOURCES += \
  src/NVBScaler.cpp \
  src/NVBUnits.cpp \
  src/NVBVariant.cpp \
  src/NVBMap.cpp \
  src/NVBDataCore.cpp \
  src/NVBDataTransforms.cpp \
  src/NVBDatasetIcons.cpp \
  src/NVBDataSource.cpp \
  src/NVBDataSourceModel.cpp \
  src/NVBAxisSelectorHelpers.cpp \
  src/NVBAxisSelector.cpp \
  src/NVBTokens.cpp \
  src/NVBFileInfo.cpp \
  src/NVBMimeData.cpp \
	src/NVBFile.cpp

# Helpful objects for plugins

HEADERS += \
	src/NVBAxisMaps.h \
	src/NVBColorMaps.h \
	src/NVBPhysSpinBox.h \
	src/NVBPhysStepSpinBox.h \
	src/NVBColorWidgets.h \
	src/NVBSettings.h \
	src/NVBSettingsWidget.h

SOURCES += \
	src/NVBColorMaps.cpp \
	src/NVBPhysSpinBox.cpp \
	src/NVBPhysStepSpinBox.cpp \
	src/NVBColorWidgets.cpp \
	src/NVBSettings.cpp \
	src/NVBSettingsWidget.cpp

CONFIG += qt

contains(CONFIG,NVBStatic) {
  CONFIG -= dll
  CONFIG += static
	OBJECTS_DIR = obj
} else {
  CONFIG += dll
	OBJECTS_DIR = objPIC
}

TARGET = lib/nvb
MOC_DIR = moc
TEMPLATE = lib
DESTDIR = .

VERSION = 0.1.0

QT += core gui

contains(CONFIG,NVBShared) {
  unix {
    target.path = /usr/lib
  } else {
    target.path = $$NVB_PLUGIN_INSTALL_PATH
  }
  INSTALLS += target
}
