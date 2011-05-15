include(nvb.pri)

# Log
HEADERS += src/NVBLogger.h

contains(CONFIG,NVBLog){
		HEADERS += \
						src/NVBLogUtils.h
		SOURCES += \
						src/NVBLogger.cpp \
						src/NVBLogUtils.cpp
}

# STMFile tools
HEADERS += \
					 src/NVBJointFile.h \
					 src/NVBFileBundle.h \
					 src/NVBFileFactory.h
SOURCES += \
					 src/NVBJointFile.cpp \
					 src/NVBFileBundle.cpp \
					 src/NVBFileFactory.cpp


# FileBrowser
HEADERS += \
					 src/NVBPageRefactorModel.h \
					 src/NVBListItemDelegate.h \
					 src/NVBPageInfoWidget.h \
					 src/NVBPageInfoView.h \
					 src/NVBProgress.h \
					 src/NVBColumnsModel.h \
					 src/NVBColumnDialog.h \
					 src/NVBFileFilterDialog.h \
					 src/NVBDirModelHelpers.h \
					 src/NVBDirModel.h \
					 src/NVBDirView.h \
					 src/NVBDirViewModel.h \
					 src/NVBBrowser.h

SOURCES += \
					 src/NVBPageRefactorModel.cpp \
					 src/NVBListItemDelegate.cpp \
					 src/NVBPageInfoWidget.cpp \
					 src/NVBColumnsModel.cpp \
					 src/NVBColumnDialog.cpp \
					 src/NVBFileFilterDialog.cpp \
					 src/NVBDirModelHelpers.cpp \
					 src/NVBDirModel.cpp \
					 src/NVBDirView.cpp \
					 src/NVBDirViewModel.cpp \
					 src/NVBBrowser.cpp

# Application headers
HEADERS += \
					 src/NVBSettings.h \
					 src/NVBMain.h \
					 src/NvBrowserApp.h

SOURCES += \
					 src/NVBSettings.cpp \
					 src/NVBMain.cpp \
					 src/NvBrowserApp.cpp

DEFINES += NVB_BROWSER_ONLY

CONFIG += qt
unix : CONFIG += x11
TARGET = bin/nvbrowser
MOC_DIR = moc
OBJECTS_DIR = obj
TEMPLATE = app
DESTDIR = .

QT += core gui

win32: RC_FILE = icons/novembre.rc

macx : ICON = icons/nvb.icns

contains(CONFIG,NVBStatic) {
} else {
DEFINES += NVB_PLUGINS=\\\"$$NVB_PLUGIN_INSTALL_PATH\\\"
}

target.path = $$NVB_BIN_INSTALL_PATH

INSTALLS += target