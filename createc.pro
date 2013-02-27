include(nvbfile.pri)

INCLUDEPATH  += $${ZLIB_INCLUDE}
LIBS         += $${ZLIB_LIB}

# Library headers
HEADERS += \
	  src/createc.h
SOURCES += \
	  src/createc.cpp

TARGET = lib/files/createc

DESTDIR = .
