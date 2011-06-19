# Enable logging ability
CONFIG += NVBLog

# Show verbose messages in log
# CONFIG += NVBVerboseLog

# Select views to use
CONFIG +=
          NVB2DView				NVB2DView \
#          NVBGraphView \
#          NVB3DView


# Compile plugins as shared libraries
CONFIG += NVBShared

# Compile plugins into Novembre (recomended for windows)
win32:CONFIG += NVBStatic

# Put docks on main window
# CONFIG += NVBGlobalDocks

# Don't compile debugging information in
CONFIG -= debug
CONFIG += release

# Use qwt from these locations
unix {
    INCLUDEPATH += /usr/include/qwt6
    LIBS += -lqwt6
}
win32 { 
  INCLUDEPATH += ../qwt-6.0.0/src/
  LIBS += \
    -L../qwt-6.0.0/lib \
    -lqwt6
}

# Installation paths for Novembre binaries
unix { 
    NVB_INSTALL_PATH = /opt/Novembre
    NVB_BIN_INSTALL_PATH = $$NVB_INSTALL_PATH/bin
    NVB_PLUGIN_INSTALL_PATH = $$NVB_INSTALL_PATH/lib
}
win32 { 
    NVB_INSTALL_PATH = "C:\\Program\ Files\\Novembre"
    NVB_BIN_INSTALL_PATH = $$NVB_INSTALL_PATH
    NVB_PLUGIN_INSTALL_PATH = $$NVB_INSTALL_PATH
}
OTHER_FILES += TODO.txt
