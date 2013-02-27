exists($${OUT_PWD}/Novembre.pri) {
	NVB_CONFILE = $${OUT_PWD}/Novembre.pri
} else {
	NVB_CONFILE = Novembre.pri
}

include($${NVB_CONFILE})

#
# No need to expose that to end users
#

# qwtplot3d has been changed from upstream -> do not use official version
INCLUDEPATH += src/qwtplot3d

NVBStatic {
 CONFIG -= NVBShared
} else {
 !NVBShared : CONFIG += NVBShared
}

NVBStatic {
  DEFINES += NVB_STATIC
  contains(MAKEFILE_GENERATOR,MINGW) : QMAKE_LFLAGS += -static-libgcc
}

debug {
  CONFIG += NVBLog NVBVerboseLog
  DEFINES += NVB_DEBUG
}

# Log
NVBLog {
    DEFINES += NVB_ENABLE_LOG
    contains(CONFIG,NVBVerboseLog){
        DEFINES += NVB_VERBOSE_LOG
    }
}

NVB2DView:    DEFINES += WITH_2DVIEW
NVBGraphView: DEFINES += WITH_GRAPHVIEW
NVB3DView:    DEFINES += WITH_3DVIEW

#
# Hack to have less includes
# nvblib.pro is the only file defining NVBLib
#
!NVBLib {
 LIBS += -Llib
 debug {
   LIBS += -Ldebug/lib
 } else {
   LIBS += -Lrelease/lib
 }
LIBS += -lnvb
}
