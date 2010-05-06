CONFIG += NVBLib

include(nvb.pri)

contains(CONFIG,NVB3DView) {
#  CONFIG += gl2ps // Uncomment if you want export to vector formats
  include(qwtplot3d.pro)
  QT += opengl
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
           src/NVBFileInfo.h \
           src/NVBPhysSpinBox.h \
           src/NVBPhysStepSpinBox.h

SOURCES += \
           src/NVBDimension.cpp \
           src/NVBVariant.cpp \
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

#target.path = $$NVB_PLUGIN_INSTALL_PATH

contains(CONFIG,NVBShared) {
  target.path = /usr/lib
  INSTALLS += target
}
