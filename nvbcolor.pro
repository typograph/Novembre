include(nvbdelegate.pri)

contains(CONFIG,NVB2DView) {

HEADERS += \
# 			src/core/NVBGraphicsItems.h \
			src/filters/NVBDiscrColorPainter.h \
			src/filters/NVBCurveClassifier.h

SOURCES += \
			src/filters/NVBDiscrColorPainter.cpp \
			src/filters/NVBCurveClassifier.cpp

}

HEADERS += \
# 			src/core/NVBContColoring.h \
# 			src/core/NVBDiscrColoring.h \
			src/filters/NVBColorAdjustProvider.h \
			src/filters/NVBContColorScaler.h \
			src/filters/NVBColorButton.h

SOURCES += \
			src/filters/NVBColorAdjustProvider.cpp \
			src/filters/NVBContColorScaler.cpp

#          src/filters/NVBSlicePainter.h
#          src/filters/NVBSlicePainter.cpp

TARGET = lib/tools/nvbcolor

DESTDIR = .
