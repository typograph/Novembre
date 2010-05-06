include(nvbdelegate.pri)

contains(CONFIG,NVB2DView) {
HEADERS += \
          src/NVBGraphicsItems.h \
          src/NVBCircAverager.h

SOURCES += \
          src/NVBCircAverager.cpp
}

HEADERS += \
          src/NVBContColoring.h \
          src/NVBTopoToolsProvider.h \
          src/NVBTopoLeveler.h

SOURCES += \
          src/NVBTopoToolsProvider.cpp \
          src/NVBTopoLeveler.cpp

TARGET = lib/tools/nvbtopo

DESTDIR = .

