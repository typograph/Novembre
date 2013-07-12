include(nvbfile.pri)

# Comment the following line to forbid recognition
# of the artificial *.nns format
# NNS format has nothing to do with Nanonis and will be
# replaced later when scripting becomes available
DEFINES += WITH_SPEC_AGGR

# Library headers
HEADERS += \
	  src/files/nanonis.h
SOURCES += \
	  src/files/nanonis.cpp

TARGET = lib/files/nanonis

DESTDIR = .
