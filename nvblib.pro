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

HEADERS += \
           src/dimension.h \
           src/NVBDimension.h \
           src/NVBVariant.h \
           src/NVBQuadTree.h \
           src/NVBDataSource.h \
           src/NVBGeneralDelegate.h \
           src/NVBViewController.h \
           src/NVBFilterDelegate.h \
           src/NVBDelegateProvider.h \
           src/NVBPageToolbar.h \
           src/NVBContColoring.h \
           src/NVBDiscrColoring.h \
           src/NVBContColorModel.h \
           src/NVBDiscrColorModel.h \
           src/NVBPages.h \
           src/NVBTokens.h \
           src/NVBFileInfo.h \
           src/NVBMimeData.h \
           src/NVBPageViewModel.h \
           src/NVBFile.h \
           src/NVBPhysSpinBox.h \
           src/NVBPhysStepSpinBox.h

SOURCES += \
           src/NVBDimension.cpp \
           src/NVBVariant.cpp \
           src/NVBQuadTree.cpp \
           src/NVBDataSource.cpp \
           src/NVBPageToolbar.cpp \
           src/NVBContColoring.cpp \
           src/NVBDiscrColoring.cpp \
           src/NVBPages.cpp \
           src/NVBTokens.cpp \
           src/NVBFileInfo.cpp \
           src/NVBMimeData.cpp \
           src/NVBPageViewModel.cpp \
           src/NVBFile.cpp \
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
