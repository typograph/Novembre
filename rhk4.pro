include(nvbfile.pri)

# Library headers
HEADERS += \
		src/rhk4.h
SOURCES += \
		src/rhk4.cpp

# comment out the next line for release
#DEFINES += RHK_SUBSTRACT_BIAS

TARGET = lib/files/rhk4

DESTDIR = .
