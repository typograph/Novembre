include(nvbfile.pri)
# Library headers
HEADERS += \
	  src/confile.h \
	  src/lispInt.h \
          src/textCfSTM.h
SOURCES += \
	  src/confile.cpp \
	  src/lispInt.cpp \
          src/textCfSTM.cpp

TARGET = lib/files/textSTM

DESTDIR = .


