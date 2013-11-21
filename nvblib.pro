CONFIG += NVBLib

include(nvb.pri)

NVB3DView {
#  CONFIG += gl2ps // Uncomment if you want export to vector formats
	include(qwtplot3d.pro)
	QT += opengl
}

# Log

HEADERS += src/core/NVBLogger.h

NVBLog: SOURCES += src/core/NVBLogger.cpp

NVB2DView {
HEADERS += \
					src/core/NVBGraphicsItems.h
SOURCES += \
					src/core/NVBGraphicsItems.cpp
}

HEADERS += \
					src/core/dimension.h \
					src/core/NVBDimension.h \
					src/core/NVBVariant.h \
					src/core/NVBQuadTree.h \
					src/core/NVBDataSource.h \
					src/core/NVBGeneralDelegate.h \
					src/core/NVBViewController.h \
					src/core/NVBFilterDelegate.h \
					src/core/NVBDelegateProvider.h \
					src/core/NVBPageToolbar.h \
					src/core/NVBContColoring.h \
					src/core/NVBDiscrColoring.h \
					src/core/NVBContColorModel.h \
					src/core/NVBDiscrColorModel.h \
					src/core/NVBPages.h \
					src/core/NVBTokens.h \
					src/core/NVBFileInfo.h \
					src/core/NVBMimeData.h \
					src/core/NVBPageViewModel.h \
					src/core/NVBFile.h \
					src/core/NVBPhysSpinBox.h \
					src/core/NVBPhysStepSpinBox.h

SOURCES += \
					src/core/dimension.cpp \
					src/core/NVBDimension.cpp \
					src/core/NVBVariant.cpp \
					src/core/NVBQuadTree.cpp \
					src/core/NVBDataSource.cpp \
					src/core/NVBPageToolbar.cpp \
					src/core/NVBContColoring.cpp \
					src/core/NVBDiscrColoring.cpp \
					src/core/NVBPages.cpp \
					src/core/NVBTokens.cpp \
					src/core/NVBFileInfo.cpp \
					src/core/NVBMimeData.cpp \
					src/core/NVBPageViewModel.cpp \
					src/core/NVBFile.cpp \
					src/core/NVBPhysSpinBox.cpp \
					src/core/NVBPhysStepSpinBox.cpp \
					src/core/NVBFilterDelegate.cpp

CONFIG += qt

NVBStatic {
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

NVBShared {
	unix {
		target.path = /usr/lib
	} else {
		target.path = $$NVB_PLUGIN_INSTALL_PATH
	}
	INSTALLS += target
}
