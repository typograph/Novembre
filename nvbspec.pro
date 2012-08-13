include(nvbdelegate.pri)

HEADERS += \
    src/NVBDiscrColoring.h \
    src/NVBSpecAverager.h \
    src/NVBAverageColor.h \
    src/NVBSpecToolsProvider.h \
    src/NVBCurveModel.h \
    src/NVBSpecSubstractor.h \
    src/NVBSpecMath.h \
    src/NVBSpecShift.h \
    src/NVBSpecExcluder.h \
    src/NVBSpecSmooth.h \
	src/NVBPhysSpinBox.h \
	src/NVBSpecSlicer.h

SOURCES += \
    src/NVBSpecAverager.cpp \
    src/NVBSpecToolsProvider.cpp \
    src/NVBCurveModel.cpp \
    src/NVBSpecSubstractor.cpp \
    src/NVBSpecMath.cpp \
    src/NVBSpecShift.cpp \
    src/NVBSpecExcluder.cpp \
	src/NVBSpecSmooth.cpp \
	src/NVBSpecSlicer.cpp

TARGET = lib/tools/nvbspec

DESTDIR = .

