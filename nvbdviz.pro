include(nvbdelegate.pri)

HEADERS += \
    src/NVBContColoring.h \
    src/NVBDiscrColoring.h \
    src/NVBIconVizDelegates.h \
    src/NVBDVizProvider.h \
    src/NVBTopoCurver.h
SOURCES += \
    src/NVBIconVizDelegates.cpp \
    src/NVBDVizProvider.cpp \
    src/NVBTopoCurver.cpp

contains(CONFIG,NVB2DView) {
HEADERS += \
    src/NVBGraphicsItems.h \
    src/NVB2DVizDelegates.h
SOURCES += src/NVB2DVizDelegates.cpp
}

contains(CONFIG,NVB3DView) {
HEADERS += src/NVB3DVizDelegates.h
SOURCES += src/NVB3DVizDelegates.cpp
}

contains(CONFIG,NVBGraphView) {
HEADERS += src/NVBGraphVizDelegates.h
SOURCES += src/NVBGraphVizDelegates.cpp
}


CONFIG -= dll
CONFIG += static

TARGET = lib/tools/nvbdviz

DESTDIR = .

