project(novembre)
cmake_minimum_required(VERSION 2.8)

# Add Qt
find_package(Qt4 4.6 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# Add configuration options
include(NovembreConfig.cmake)

if (UNIX)
	set(NOVEMBRE_INSTALL_PREFIX "/opt/Novembre" CACHE PATH "Prefix prepended to
install directories")
elseif(WIN32)
	set(NOVEMBRE_INSTALL_PREFIX "C:/Program Files/Novembre" CACHE PATH "Prefix prepended to
install directories")
endif()

set(CMAKE_INSTALL_PREFIX "${NOVEMBRE_INSTALL_PREFIX}" CACHE INTERNAL "Prefix
prepended to install directories" FORCE)

# Add Qwt
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

if (UNIX)
	find_package(Qwt6 REQUIRED)
else(UNIX)
	include_directories(../qwt-6.0.0/src/)
	link_directories(../qwt-6.0.0/lib)
endif(UNIX)

include_directories(${Qwt6_INCLUDE_DIR})

if (NOVEMBRE_STATIC_PLUGINS)
	add_definitions(-DNVB_STATIC -DQT_STATICPLUGIN)
	set(LIBRARY_BUILD_MODE STATIC)
	set(PLUGIN_BUILD_MODE STATIC)
else(NOVEMBRE_STATIC_PLUGINS)
	set(LIBRARY_BUILD_MODE SHARED)
	set(PLUGIN_BUILD_MODE SHARED)
	if (UNIX)
		add_definitions(-DNVB_PLUGINS="${CMAKE_INSTALL_PREFIX}/lib")
	elseif (WIN32)
		add_definitions(-DNVB_PLUGINS="${CMAKE_INSTALL_PREFIX}")
	endif()
endif(NOVEMBRE_STATIC_PLUGINS)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set(NOVEMBRE_LOG ON)
	set(NOVEMBRE_VERBOSE_LOG ON)
	add_definitions(-DNVB_DEBUG)
endif()

# Log
if (NOVEMBRE_LOG)
    add_definitions(-DNVB_ENABLE_LOG)
    if (NOVEMBRE_VERBOSE_LOG)
        add_definitions(-DNVB_VERBOSE_LOG)
    endif(NOVEMBRE_VERBOSE_LOG)
endif(NOVEMBRE_LOG)

if (NOVEMBRE_2DVIEW)
	add_definitions(-DWITH_2DVIEW)
endif (NOVEMBRE_2DVIEW)

if (NOVEMBRE_GRAPHVIEW)
	add_definitions(-DWITH_GRAPHVIEW)
endif (NOVEMBRE_GRAPHVIEW)

if (NOVEMBRE_3DVIEW)
	add_definitions(-DWITH_3DVIEW)
endif (NOVEMBRE_3DVIEW)

# -----
# targets and libraries
# -----

# Add Library
include(NovembreLibrary.cmake)

# Add support for file formats
include(FilePlugins.cmake)

# include(ToolPlugins.cmake)

# Add Browser
include(NovembreBrowser.cmake)

# Add main UI
# include(NovembreUI.cmake)

if (NOVEMBRE_STATIC_PLUGINS)
	set(INSTALL_TARGETS nvbrowser libnvb ${fileplugins})
else()
	set(INSTALL_TARGETS nvbrowser)
endif()

if (UNIX)
	set(CMAKE_INSTALL_PREFIX "/opt/Novembre")
	install(TARGETS ${INSTALL_TARGETS}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
elseif (WIN32)
	set(CMAKE_INSTALL_PREFIX "C:/Program Files/Novembre")
	install(TARGETS ${INSTALL_TARGETS}
			RUNTIME DESTINATION .
			LIBRARY DESTINATION .
			ARCHIVE DESTINATION .)
endif()
