include(nvbdelegate.pri)

HEADERS += \
    src/NVBPageViewModel.h \
    src/NVBFile.h \
    src/NVBImportToolsProvider.h \
    src/NVBMultiSpecImportView.h

SOURCES += \
    src/NVBPageViewModel.cpp \
    src/NVBFile.cpp \
    src/NVBImportToolsProvider.cpp \
    src/NVBMultiSpecImportView.cpp


TARGET = lib/tools/nvbimport

DESTDIR = .

CONFIG -= dll
CONFIG += static
