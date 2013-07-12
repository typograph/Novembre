include(nvbdelegate.pri)

HEADERS += \
#     src/core/NVBContColoring.h \
#     src/core/NVBDiscrColoring.h \
    src/filters/NVBIconVizDelegates.h \
    src/filters/NVBDVizProvider.h \
    src/filters/NVBTopoCurver.h
SOURCES += \
    src/filters/NVBIconVizDelegates.cpp \
    src/filters/NVBDVizProvider.cpp \
    src/filters/NVBTopoCurver.cpp

contains(CONFIG,NVB2DView) {
HEADERS += \
#     src/core/NVBGraphicsItems.h \
    src/filters/NVB2DVizDelegates.h
SOURCES += src/filters/NVB2DVizDelegates.cpp
}

contains(CONFIG,NVB3DView) {
HEADERS += src/filters/NVB3DVizDelegates.h
SOURCES += src/filters/NVB3DVizDelegates.cpp
}

contains(CONFIG,NVBGraphView) {
HEADERS += src/filters/NVBGraphVizDelegates.h
SOURCES += src/filters/NVBGraphVizDelegates.cpp
}


CONFIG -= dll
CONFIG += static

TARGET = lib/tools/nvbdviz

DESTDIR = .

