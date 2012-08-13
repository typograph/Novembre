include(nvbdelegate.pri)

contains(CONFIG,NVB2DView) {

HEADERS += \
          src/NVBGraphicsItems.h \
          src/NVBDiscrColorPainter.h

SOURCES += \
          src/NVBDiscrColorPainter.cpp

}

HEADERS += \
          src/NVBContColoring.h \
          src/NVBDiscrColoring.h \
          src/NVBColorAdjustProvider.h \
          src/NVBContColorScaler.h \
          src/NVBColorButton.h

SOURCES += \
          src/NVBColorAdjustProvider.cpp \
		  src/NVBContColorScaler.cpp

#          src/NVBSlicePainter.h
#          src/NVBSlicePainter.cpp

TARGET = lib/tools/nvbcolor

DESTDIR = .
