include(nvbdelegate.pri)

HEADERS += \
    src/NVBExportToolsProvider.h \
    src/NVBQtiExportView.h \
	src/NVBQtiTableDialog.h

SOURCES += \
    src/NVBExportToolsProvider.cpp \
    src/NVBQtiExportView.cpp \
	src/NVBQtiTableDialog.cpp


TARGET = lib/tools/nvbexport

DESTDIR = .



