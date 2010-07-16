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

# Delegates
HEADERS += \
           src/NVBToolsFactory.h
SOURCES += \
           src/NVBToolsFactory.cpp

contains(CONFIG,debug) {
win32 : LIBS += -Ldebug/lib/tools -lnvbdviz
} else {
win32 : LIBS += -Lrelease/lib/tools -lnvbdviz
}
unix : LIBS += -Llib/tools -lnvbdviz

contains(CONFIG,NVBStatic) {
	LIBS += \
          -lnvbexport \
          -lnvbspec \
          -lnvbtopo \
          -lnvbcolor
 contains(CONFIG,debug) {
	win32 : LIBS += -Ldebug/lib/files
 } else {
	win32 : LIBS += -Lrelease/lib/files
 }
 unix : LIBS += -Llib/files
 LIBS += -lrhk -lcreatec
# libnvb should be included after everything, since everything depends on it
 LIBS -= -lnvb
 LIBS += -lnvb

 LIBS -= -lqwt
 LIBS += -lqwt
# libopengl32 has to be included after libnvb
 contains(CONFIG, NVB3DView) {
   win32 : LIBS += -lopengl32 -lglu32 -lqwt
   unix : LIBS += -lGL -lGLU -lqwt
}
}

# FileWindow

HEADERS += \
           src/NVBVizModel.h \
           src/QCollapsibleBox.h \
           src/NVBDelegateStackView.h \
           src/NVBWidgetStackModel.h \
           src/NVBDataView.h \
           src/NVBFileWindowLayout.h \
           src/NVBFileWindow.h
SOURCES += \
           src/NVBVizModel.cpp \
           src/QCollapsibleBox.cpp \
           src/NVBDelegateStackView.cpp \
           src/NVBWidgetStackModel.cpp \
           src/NVBFileWindowLayout.cpp \
           src/NVBFileWindow.cpp

contains(CONFIG,NVB2DView){
    HEADERS += \
           src/NVB2DPageView.h
    SOURCES += \
           src/NVB2DPageView.cpp
}

contains(CONFIG,NVBGraphView){
    HEADERS += \
           src/NVBGraphView.h
    SOURCES += \
           src/NVBGraphView.cpp
}

contains(CONFIG,NVB3DView){
    HEADERS += \
           src/NVB3DPageView.h
    SOURCES += \
           src/NVB3DPageView.cpp
    QT += opengl
}

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

contains(CONFIG,NVBGlobalDocs) {
  DEFINES += NVB_NO_FWDOCS
}

# Application headers
HEADERS += \
           src/NVBSettings.h \
           src/NVBMainDocks.h \
           src/NVBMain.h \
           src/Novembre.h

SOURCES += \
           src/NVBSettings.cpp \
           src/NVBMain.cpp \
           src/Novembre.cpp




CONFIG += qt
# CONFIG -= stl thread
# win32: CONFIG += windows
unix : CONFIG += x11
TARGET = bin/novembre
MOC_DIR = moc
OBJECTS_DIR = obj
TEMPLATE = app
DESTDIR = .

QT += core gui network

win32: RC_FILE = icons/novembre.rc

macx : ICON = icons/nvb.icns

contains(CONFIG,NVBStatic) {
} else {
DEFINES += NVB_PLUGINS=\\\"$$NVB_PLUGIN_INSTALL_PATH\\\"
}

target.path = $$NVB_BIN_INSTALL_PATH

INSTALLS += target
