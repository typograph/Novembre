include(nvbfile.pri)

INCLUDEPATH  += $${ZLIB_INCLUDE}
LIBS         += $${ZLIB_LIB}

# Library headers
HEADERS += \
	  src/files/createc.h
SOURCES += \
	  src/files/createc.cpp

TARGET = lib/files/createc

DESTDIR = .
