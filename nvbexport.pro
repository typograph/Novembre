include(nvbdelegate.pri)

HEADERS += \
	src/filters/NVBExportToolsProvider.h \
	src/filters/NVBQtiExportView.h \
	src/filters/NVBQtiTableDialog.h

SOURCES += \
	src/filters/NVBExportToolsProvider.cpp \
	src/filters/NVBQtiExportView.cpp \
	src/filters/NVBQtiTableDialog.cpp


TARGET = lib/tools/nvbexport

DESTDIR = .



