include(nvbfile.pri)

win32: INCLUDEPATH  += ../zlib/include
win32: LIBS         += -L../zlib/lib -lzlib

# Library headers
HEADERS += \
	  src/createc.h
SOURCES += \
	  src/createc.cpp

TARGET = lib/files/createc

DESTDIR = .
