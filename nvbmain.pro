include(nvb.pri)

# Log HEADERS += src/core/NVBLogger.h

NVBLog {
	HEADERS += \
		src/gui/NVBLogUtils.h
	SOURCES += \
		# src/core/NVBLogger.cpp \ # Why is this here?
		src/gui/NVBLogUtils.cpp
}

# STMFile tools
HEADERS += \
#	src/core/NVBDimension.h \
##	src/core/NVBFile.h \
	src/gui/NVBJointFile.h \
	src/gui/NVBFileBundle.h \
	src/gui/NVBFileFactory.h
SOURCES += \
#	src/core/NVBDimension.cpp \ # Why is this here? It should be in the library
	src/gui/NVBJointFile.cpp \
	src/gui/NVBFileBundle.cpp \
	src/gui/NVBFileFactory.cpp

# Delegates
HEADERS += \
	src/gui/NVBToolsFactory.h
SOURCES += \
	src/gui/NVBToolsFactory.cpp

debug {
	win32 : LIBS += -Ldebug/lib/tools -lnvbdviz
} else {
	win32 : LIBS += -Lrelease/lib/tools -lnvbdviz
}

unix : LIBS += -Llib/tools -lnvbdviz

NVBStatic {

	LIBS += \
		-lnvbexport \
		-lnvbspec \
		-lnvbtopo \
		-lnvbcolor

	debug {
		win32 : LIBS += -Ldebug/lib/files
	} else {
		win32 : LIBS += -Lrelease/lib/files
	}

	unix : LIBS += -Llib/files
	LIBS += \
		-lrhk \
		-lrhk4 \
		-lcreatec \
		-lwinspm \
		-lnanonis
# libnvb should be included after everything, since everything depends on it

	LIBS -= -lnvb
	LIBS += -lnvb

# libopengl32 has to be included after libnvb
	contains(CONFIG, NVB3DView) {
		win32 : LIBS += -lopengl32 -lglu32
		unix : LIBS += -lGL -lGLU
	}
}

# FileWindow

HEADERS += \
	src/gui/NVBVizModel.h \
	src/gui/NVBUserPageViewModel.h \
	src/gui/NVBPageViewModel.h \
	src/gui/QCollapsibleBox.h \
	src/gui/NVBDelegateStackView.h \
	src/gui/NVBWidgetStackModel.h \
	src/gui/NVBDataView.h \
	src/gui/NVBFileWindowLayout.h \
	src/gui/NVBFileWindow.h
SOURCES += \
	src/gui/NVBVizModel.cpp \
	src/gui/NVBUserPageViewModel.cpp \
	src/gui/QCollapsibleBox.cpp \
	src/gui/NVBDelegateStackView.cpp \
	src/gui/NVBWidgetStackModel.cpp \
	src/gui/NVBFileWindowLayout.cpp \
	src/gui/NVBFileWindow.cpp

NVB2DView {
	HEADERS += \
		src/gui/NVB2DPageView.h
	SOURCES += \
		src/gui/NVB2DPageView.cpp
}

NVBGraphView {
	HEADERS += \
		src/gui/NVBPlotZoomer.h \
		src/gui/NVBGraphView.h
	SOURCES += \
		src/gui/NVBPlotZoomer.cpp \
		src/gui/NVBGraphView.cpp
}

NVB3DView {
	HEADERS += \
		src/gui/NVB3DPageView.h
	SOURCES += \
		src/gui/NVB3DPageView.cpp
	QT += opengl
}

# FileBrowser
HEADERS += \
	src/gui/NVBPageRefactorModel.h \
	src/gui/NVBListItemDelegate.h \
	src/gui/NVBPageInfoWidget.h \
	src/gui/NVBPageInfoView.h \
	src/gui/NVBColumnsModel.h \
	src/gui/NVBColumnDialog.h \
	src/gui/NVBFileFilterDialog.h \
	src/gui/NVBDirModelHelpers.h \
	src/gui/NVBDirModel.h \
	src/gui/NVBFileListView.h \
	src/gui/NVBDirView.h \
	src/gui/NVBDirViewModel.h \
	src/gui/NVBBrowserHelpers.h \
	src/gui/NVBBrowser.h

SOURCES += \
	src/gui/NVBPageRefactorModel.cpp \
	src/gui/NVBListItemDelegate.cpp \
	src/gui/NVBPageInfoWidget.cpp \
	src/gui/NVBColumnsModel.cpp \
	src/gui/NVBColumnDialog.cpp \
	src/gui/NVBFileFilterDialog.cpp \
	src/gui/NVBDirModelHelpers.cpp \
	src/gui/NVBDirModel.cpp \
	src/gui/NVBFileListView.cpp \
	src/gui/NVBDirView.cpp \
	src/gui/NVBDirViewModel.cpp \
	src/gui/NVBBrowserHelpers.cpp \
	src/gui/NVBBrowser.cpp

NVBGlobalDocks {
	DEFINES += NVB_NO_FWDOCS
}

# Application headers
HEADERS += \
	src/gui/NVBSettings.h \
	src/gui/NVBMainDocks.h \
	src/gui/NVBMain.h \
	src/Novembre.h

SOURCES += \
	src/gui/NVBSettings.cpp \
	src/gui/NVBMain.cpp \
	src/Novembre.cpp


CONFIG += qt
unix : CONFIG += x11

TARGET = bin/novembre
MOC_DIR = moc
OBJECTS_DIR = obj
TEMPLATE = app
DESTDIR = .

QT += core gui network

win32: RC_FILE = icons/novembre.rc

macx : ICON = icons/nvb.icns

!NVBStatic: DEFINES += NVB_PLUGINS=\\\"$$NVB_PLUGIN_INSTALL_PATH\\\"

target.path = $$NVB_BIN_INSTALL_PATH

INSTALLS += target


