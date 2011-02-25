CONFIG += NVBLib

include(nvb.pri)

contains(CONFIG,NVB3DView) {
#  CONFIG += gl2ps // Uncomment if you want export to vector formats
  include(qwtplot3d.pro)
  QT += opengl
}

# Log

HEADERS += NVBLogger.h

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
					 src/NVBDataTransform.h \
           src/NVBDataSource.h \
					 src/NVBAxisSelector.h \
           src/NVBGeneralDelegate.h \
           src/NVBViewController.h \
           src/NVBFilterDelegate.h \
           src/NVBDelegateProvider.h \
           src/NVBPageToolbar.h \
           src/NVBTokens.h \
           src/NVBFileInfo.h \
           src/NVBMimeData.h \
           src/NVBPageViewModel.h \
           src/NVBFile.h \

SOURCES += \
           src/NVBUnits.cpp \
           src/NVBVariant.cpp \
					 src/NVBMap.cpp \
					 src/NVBDataCore.cpp \
					 src/NVBDataTransform.cpp \
					 src/NVBDataSource.cpp \
					 src/NVBAxisSelector.cpp \
					 src/NVBPageToolbar.cpp \
           src/NVBPages.cpp \
           src/NVBTokens.cpp \
           src/NVBFileInfo.cpp \
           src/NVBMimeData.cpp \
           src/NVBPageViewModel.cpp \
					 src/NVBFile.cpp

# Helpful objects for plugins

HEADERS += \
					 src/NVBAxisMaps.h \
					 src/NVBColorMaps.h \
					 src/NVBPhysSpinBox.h \
					 src/NVBPhysStepSpinBox.h

SOURCES += \
					 src/NVBColorMaps.cpp \
					 src/NVBPhysSpinBox.cpp \
					 src/NVBPhysStepSpinBox.cpp

CONFIG += qt

contains(CONFIG,NVBStatic) {
  CONFIG -= dll
  CONFIG += static
} else {
  CONFIG += dll
}

TARGET = lib/nvb
MOC_DIR = moc
OBJECTS_DIR = objPIC
TEMPLATE = lib
DESTDIR = .

VERSION = 0.0.5

QT += core gui

contains(CONFIG,NVBShared) {
  unix {
    target.path = /usr/lib
  } else {
    target.path = $$NVB_PLUGIN_INSTALL_PATH
  }
  INSTALLS += target
}
