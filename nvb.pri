include(Novembre.pri)

#
# No need to expose that to end users
#
INCLUDEPATH += src/qwtplot3d

contains(CONFIG,NVBStatic) {
 CONFIG -= NVBShared
} else {
 contains(CONFIG,NVBShared) {
 } else {
  CONFIG += NVBShared
 }
}

contains(CONFIG,NVBStatic) {
  DEFINES += NVB_STATIC
}

contains(CONFIG,debug) {
  CONFIG += NVBLog NVBVerboseLog
  DEFINES += NVB_DEBUG
}

contains(CONFIG,NVB2DView) {
  DEFINES += WITH_2DVIEW
}

contains(CONFIG,NVBGraphView) {
  DEFINES += WITH_GRAPHVIEW
}

contains(CONFIG,NVB3DView) {
  DEFINES += WITH_3DVIEW
}

#
# Hack to have less includes
# nvblib.pro is the only file defining NVBLib
#
contains(CONFIG,NVBLib) {
} else {
 contains(CONFIG,debug) {
	win32:  LIBS += -Ldebug/lib -lnvb
 } else {
	win32:  LIBS += -Lrelease/lib -lnvb
 }
 unix:   LIBS += -Llib -lnvb
}

