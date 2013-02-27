# Enable logging ability
CONFIG += NVBLog

# Show verbose messages in log
# CONFIG += NVBVerboseLog

# Select views to use
CONFIG += NVB2DView \
          NVBGraphView \
          NVB3DView

# Compile plugins as shared libraries
CONFIG += NVBShared

# Compile plugins into Novembre (recomended for windows)
win32:CONFIG += NVBStatic

# Put docks on main window
# CONFIG += NVBGlobalDocks

# Don't compile debugging information in
CONFIG -= debug
CONFIG += release

ZLIB_INCLUDE =
ZLIB_LIB =

# Use qwt from these locations
unix { 
    INCLUDEPATH += /usr/include/qwt5
    LIBS += -lqwt
}
win32 { 
    INCLUDEPATH += -I../3rdparty/qwt5/src
    LIBS += -L../3rdparty/qwt5/lib
}

LIBS += -lqwt

# Installation paths for Novembre binaries
unix { 
    NVB_INSTALL_PATH = /opt/Novembre
    NVB_BIN_INSTALL_PATH = $$NVB_INSTALL_PATH/bin
    NVB_PLUGIN_INSTALL_PATH = $$NVB_INSTALL_PATH/lib
}
win32 { 
    NVB_INSTALL_PATH = "C:/Program Files/Novembre"
    NVB_BIN_INSTALL_PATH = $$NVB_INSTALL_PATH
    NVB_PLUGIN_INSTALL_PATH = $$NVB_INSTALL_PATH
}
OTHER_FILES += TODO.txt
