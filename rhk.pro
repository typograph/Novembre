include(nvbfile.pri)

# Library headers
HEADERS += \
	  src/rhk.h
SOURCES += \
	  src/rhk.cpp

# comment out the next line for release
DEFINES += RHK_SUBSTRACT_BIAS

TARGET = lib/files/rhk

DESTDIR = .
