include(nvbdelegate.pri)

HEADERS += \
# 		src/core/NVBDiscrColoring.h \
		src/filters/NVBSpecAverager.h \
		src/filters/NVBAverageColor.h \
		src/filters/NVBSpecToolsProvider.h \
		src/filters/NVBCurveModel.h \
		src/filters/NVBSpecSubstractor.h \
		src/filters/NVBSpecMath.h \
		src/filters/NVBSpecShift.h \
		src/filters/NVBSpecExcluder.h \
		src/filters/NVBSpecSmooth.h \
		src/filters/NVBPhysSpinBox.h \
		src/filters/NVBSpecSlicer.h \
		src/filters/NVBSpecFFTFilter.h

SOURCES += \
		src/filters/NVBSpecAverager.cpp \
		src/filters/NVBSpecToolsProvider.cpp \
		src/filters/NVBCurveModel.cpp \
		src/filters/NVBSpecSubstractor.cpp \
		src/filters/NVBSpecMath.cpp \
		src/filters/NVBSpecShift.cpp \
		src/filters/NVBSpecExcluder.cpp \
		src/filters/NVBSpecSmooth.cpp \
		src/filters/NVBSpecSlicer.cpp \
		src/filters/NVBSpecFFTFilter.cpp

LIBS += -lfftw3 -lm

TARGET = lib/tools/nvbspec

DESTDIR = .

