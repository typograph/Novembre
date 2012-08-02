# pro file for building the makefile for qwtplot3d
#

CONFIG += qt opengl

QT += opengl

LIBS += -lGLU

# THIS HAS TO WAIT TO WIN32 TESTS
# win32:CONFIG         += dll exceptions
# win32:dll:DEFINES    += QT_DLL QWT3D_DLL QWT3D_MAKEDLL
# win32:QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_STL

# # Comment the next line, if you have zlib on your windows system
# win32:CONFIG -= zlib

# Input
SOURCES += src/qwtplot3d/qwt3d_axis.cpp \
           src/qwtplot3d/qwt3d_color.cpp \
           src/qwtplot3d/qwt3d_coordsys.cpp \
           src/qwtplot3d/qwt3d_drawable.cpp \
           src/qwtplot3d/qwt3d_mousekeyboard.cpp \
           src/qwtplot3d/qwt3d_movements.cpp \
           src/qwtplot3d/qwt3d_lighting.cpp \
           src/qwtplot3d/qwt3d_colorlegend.cpp \
           src/qwtplot3d/qwt3d_plot.cpp \
           src/qwtplot3d/qwt3d_label.cpp \
           src/qwtplot3d/qwt3d_types.cpp \
           src/qwtplot3d/qwt3d_autoscaler.cpp \
           src/qwtplot3d/qwt3d_io_reader.cpp \
           src/qwtplot3d/qwt3d_io.cpp \
           src/qwtplot3d/qwt3d_scale.cpp

SOURCES += src/qwtplot3d/qwt3d_gridmapping.cpp \
           src/qwtplot3d/qwt3d_parametricsurface.cpp \
           src/qwtplot3d/qwt3d_function.cpp

SOURCES += src/qwtplot3d/qwt3d_surfaceplot.cpp \
           src/qwtplot3d/qwt3d_gridplot.cpp \
           src/qwtplot3d/qwt3d_meshplot.cpp


HEADERS += src/qwtplot3d/qwt3d_color.h \
           src/qwtplot3d/qwt3d_global.h \
           src/qwtplot3d/qwt3d_types.h \
           src/qwtplot3d/qwt3d_axis.h \
           src/qwtplot3d/qwt3d_coordsys.h \
           src/qwtplot3d/qwt3d_drawable.h \
           src/qwtplot3d/qwt3d_helper.h \
           src/qwtplot3d/qwt3d_label.h \
           src/qwtplot3d/qwt3d_openglhelper.h \
           src/qwtplot3d/qwt3d_colorlegend.h \
           src/qwtplot3d/qwt3d_plot.h \
           src/qwtplot3d/qwt3d_autoscaler.h \
           src/qwtplot3d/qwt3d_autoptr.h \
           src/qwtplot3d/qwt3d_io.h \
           src/qwtplot3d/qwt3d_io_reader.h \
           src/qwtplot3d/qwt3d_scale.h \
           src/qwtplot3d/qwt3d_portability.h

HEADERS += src/qwtplot3d/qwt3d_mapping.h \
           src/qwtplot3d/qwt3d_gridmapping.h \
           src/qwtplot3d/qwt3d_parametricsurface.h \
           src/qwtplot3d/qwt3d_function.h

HEADERS += src/qwtplot3d/qwt3d_surfaceplot.h

contains(CONFIG,gl2ps) {
  DEFINES += WITH_GL2PS

  # gl2ps support
  HEADERS += src/gl2ps/gl2ps.h \
            src/qwtplot3d/qwt3d_io_gl2ps.h
  
  SOURCES += src/qwtplot3d/qwt3d_io_gl2ps.cpp \
            src/gl2ps/gl2ps.c


  CONFIG += zlib

  # zlib support for gl2ps
  zlib {
      DEFINES += GL2PS_HAVE_ZLIB
      win32 : LIBS += zlib.lib
      unix : LIBS += -lz
  }
}
