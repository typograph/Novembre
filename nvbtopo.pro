include(nvbdelegate.pri)

contains(CONFIG,NVB2DView) {
HEADERS += \
#           src/core/NVBGraphicsItems.h \
          src/filters/NVBCircAverager.h

SOURCES += \
          src/filters/NVBCircAverager.cpp
}

HEADERS += \
#           src/core/NVBContColoring.h \
          src/filters/NVBTopoToolsProvider.h \
          src/filters/NVBTopoLeveler.h

SOURCES += \
          src/filters/NVBTopoToolsProvider.cpp \
          src/filters/NVBTopoLeveler.cpp

TARGET = lib/tools/nvbtopo

DESTDIR = .

