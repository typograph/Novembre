# pro file for building the makefile for qwtplot3d
#

# Input
set(qwt3D_SRCS
	src/qwtplot3d/qwt3d_axis.cpp
	src/qwtplot3d/qwt3d_color.cpp
	src/qwtplot3d/qwt3d_coordsys.cpp
	src/qwtplot3d/qwt3d_drawable.cpp
	src/qwtplot3d/qwt3d_mousekeyboard.cpp
	src/qwtplot3d/qwt3d_movements.cpp
	src/qwtplot3d/qwt3d_lighting.cpp
	src/qwtplot3d/qwt3d_colorlegend.cpp
	src/qwtplot3d/qwt3d_plot.cpp
	src/qwtplot3d/qwt3d_label.cpp
	src/qwtplot3d/qwt3d_types.cpp
	src/qwtplot3d/qwt3d_autoscaler.cpp
	src/qwtplot3d/qwt3d_io_reader.cpp
	src/qwtplot3d/qwt3d_io.cpp
	src/qwtplot3d/qwt3d_scale.cpp

	src/qwtplot3d/qwt3d_gridmapping.cpp
	src/qwtplot3d/qwt3d_parametricsurface.cpp
	src/qwtplot3d/qwt3d_function.cpp

	src/qwtplot3d/qwt3d_surfaceplot.cpp
	src/qwtplot3d/qwt3d_gridplot.cpp
	src/qwtplot3d/qwt3d_meshplot.cpp
	)

set(qwt3D_HDRS
	src/qwtplot3d/qwt3d_color.h
	src/qwtplot3d/qwt3d_global.h
	src/qwtplot3d/qwt3d_types.h
	src/qwtplot3d/qwt3d_axis.h
	src/qwtplot3d/qwt3d_coordsys.h
	src/qwtplot3d/qwt3d_drawable.h
	src/qwtplot3d/qwt3d_helper.h
	src/qwtplot3d/qwt3d_label.h
	src/qwtplot3d/qwt3d_openglhelper.h
	src/qwtplot3d/qwt3d_colorlegend.h
	src/qwtplot3d/qwt3d_plot.h
	src/qwtplot3d/qwt3d_autoscaler.h
	src/qwtplot3d/qwt3d_autoptr.h
	src/qwtplot3d/qwt3d_io.h
	src/qwtplot3d/qwt3d_io_reader.h
	src/qwtplot3d/qwt3d_scale.h
	src/qwtplot3d/qwt3d_portability.h

	src/qwtplot3d/qwt3d_mapping.h
	src/qwtplot3d/qwt3d_gridmapping.h
	src/qwtplot3d/qwt3d_parametricsurface.h
	src/qwtplot3d/qwt3d_function.h

	src/qwtplot3d/qwt3d_surfaceplot.h
	)

if(WITH_GL2PS)
	add_definitions(WITH_GL2PS)

  # gl2ps support
	set(qwt3D_HDRS ${qwt3D_HDRS}
		src/gl2ps/gl2ps.h
		src/qwtplot3d/qwt3d_io_gl2ps.h
		)
  
	set(qwt3D_SRCS ${qwt3D_SRCS}
		src/qwtplot3d/qwt3d_io_gl2ps.cpp
		src/gl2ps/gl2ps.c
		)

	find_package(zlib REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIR})

	add_definitions(-DGL2PS_HAVE_ZLIB)
endif()