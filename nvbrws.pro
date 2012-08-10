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

contains(CONFIG,NVBStatic) {

	contains(CONFIG,debug) {
		win32 : LIBS += -Ldebug/lib/files
	} else {
		win32 : LIBS += -Lrelease/lib/files
	}

	unix : LIBS += -Llib/files
	LIBS += \
		-lrhk \
		-lcreatec \
#		-lwinspm \
		-lnanonis
# libnvb should be included after everything, since everything depends on it

	LIBS -= -lnvb
	LIBS += -lnvb

# libopengl32 has to be included after libnvb
	contains(CONFIG, NVB3DView) {
		win32 : LIBS += -lopengl32 -lglu32
		unix : LIBS += -lGL -lGLU
	}
} else {
	DEFINES += NVB_PLUGINS=\\\"$$NVB_PLUGIN_INSTALL_PATH\\\"
}

# STMFile tools
HEADERS += \
	src/NVBFileBundle.h \
	src/NVBGeneratorsSettingsWidget.h \
	src/NVBFilePluginModel.h \
	src/NVBFileFactory.h
SOURCES += \
	src/NVBFileBundle.cpp \
	src/NVBGeneratorsSettingsWidget.cpp \
	src/NVBFilePluginModel.cpp \
	src/NVBFileFactory.cpp

# FileBrowser
HEADERS += \
	src/NVBGradientMenu.h \
	src/NVBMutableGradients.h \
	src/NVBPageInfoWidget.h \
  src/NVBPageInfoView.h \
  src/NVBFileListView.h \
  src/NVBColumnsModel.h \
  src/NVBColumnDialog.h \
  src/NVBFileFilterDialog.h \
  src/NVBDirModelHelpers.h \
  src/NVBDirModel.h \
  src/NVBDirView.h \
  src/NVBDirViewModel.h \
  src/NVBPosLabel.h \
  src/NVBSingle2DView.h \
  src/NVBPlotCurves.h \
  src/NVBPhysScaleDraw.h \
  src/NVBSingleGraphView.h \
  src/NVBSingleView.h \
	src/NVBBrowserHelpers.h \
	src/NVBBrowser.h

SOURCES += \
	src/NVBGradientMenu.cpp \
	src/NVBMutableGradients.cpp \
	src/NVBPageInfoWidget.cpp \
  src/NVBFileListView.cpp \
  src/NVBColumnsModel.cpp \
  src/NVBColumnDialog.cpp \
  src/NVBFileFilterDialog.cpp \
  src/NVBDirModelHelpers.cpp \
  src/NVBDirModel.cpp \
  src/NVBDirView.cpp \
  src/NVBDirViewModel.cpp \
  src/NVBPosLabel.cpp \
  src/NVBSingle2DView.cpp \
  src/NVBPlotCurves.cpp \
  src/NVBPhysScaleDraw.cpp \
  src/NVBSingleGraphView.cpp \
  src/NVBSingleView.cpp \
	src/NVBBrowserHelpers.cpp \
	src/NVBBrowser.cpp

# Application headers
HEADERS += \
  src/NVBStandardIcons.h \
  src/NVBSettings.h \
  src/NVBSettingsDialog.h \
  src/NVBMainWindow.h \
  src/NVBMainDocks.h \
  src/NVBCoreApplication.h \
  src/NvBrowserApp.h

SOURCES += \
  src/NVBStandardIcons.cpp \
  src/NVBSettings.cpp \
  src/NVBSettingsDialog.cpp \
  src/NVBMainWindow.cpp \
  src/NVBMainDocks.cpp \
  src/NVBCoreApplication.cpp \
  src/NvBrowserApp.cpp

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

target.path = $$NVB_BIN_INSTALL_PATH

INSTALLS += target
