if(NOVEMBRE_3DVIEW)
#  set(WITH_GL2PS TRUE) // Uncomment if you want export to vector formats
  include(QwtPlot3D.cmake)
  set(QT_USE_OPENGL TRUE)
endif(NOVEMBRE_3DVIEW)

# Log

set(nvblib_HDRS src/NVBLogger.h)
set(nvblib_SRCS)

if (NOVEMBRE_LOG)
  set(nvblib_SRCS src/NVBLogger.cpp)
endif (NOVEMBRE_LOG)

# if (NOVEMBRE_2DVIEW)
# 	set(nvblib_HDRS
# 		${nvblib_HDRS}
# 		src/NVBGraphicsItems.h
# 		)
# 	set(nvblib_SRCS
# 		${nvblib_SRCS}
# 		src/NVBGraphicsItems.cpp
# 		)
# endif (NOVEMBRE_2DVIEW)

# Basic data structures

set(nvblib_HDRS
	${nvblib_HDRS}
	src/NVBMap.h
	src/NVBDatasetIcons.h
	src/NVBDataSource.h
	src/NVBDataSourceModel.h
	src/NVBFile.h
	)

set(nvblib_SRCS
	${nvblib_SRCS}
	src/NVBScaler.cpp
	src/NVBUnits.cpp
	src/NVBVariant.cpp
	src/NVBMap.cpp
	src/NVBDataCore.cpp
	src/NVBDataTransforms.cpp
	src/NVBDatasetIcons.cpp
	src/NVBDataSource.cpp
	src/NVBDataSourceModel.cpp
	src/NVBAxisSelectorHelpers.cpp
	src/NVBAxisSelector.cpp
	src/NVBTokens.cpp
	src/NVBFileInfo.cpp
	src/NVBMimeData.cpp
	src/NVBFile.cpp
	)

# Helpful objects for plugins

set(nvblib_HDRS
	${nvblib_HDRS}
	src/NVBPhysSpinBox.h
	src/NVBPhysStepSpinBox.h
	src/NVBColorWidgets.h
	src/NVBSettingsWidget.h
	)

set(nvblib_SRCS
	${nvblib_SRCS}
	src/NVBColorMaps.cpp
	src/NVBPhysSpinBox.cpp
	src/NVBPhysStepSpinBox.cpp
	src/NVBColorWidgets.cpp
	src/NVBSettings.cpp
	src/NVBSettingsWidget.cpp
	)

qt4_wrap_cpp(nvblib_MOC_SRCS ${nvblib_HDRS})

add_library(libnvb ${LIBRARY_BUILD_MODE} ${nvblib_SRCS} ${nvblib_MOC_SRCS} ${qwt3D_SRCS} ${qwt3D_MOC_SRCS})

set_target_properties(libnvb PROPERTIES VERSION 0.1.0 OUTPUT_NAME nvb)
set_target_properties(libnvb PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY lib
	ARCHIVE_OUTPUT_DIRECTORY lib
	)

target_link_libraries(libnvb ${QT_LIBRARIES} ${Qwt6_LIBRARY})

if(NOVEMBRE_3DVIEW)
	if (WIN32)
		target_link_libraries(libnvb -lopengl32 -lglu32)
	elseif (UNIX)
		target_link_libraries(libnvb -lGL -lGLU)
	endif(WIN32)
endif(NOVEMBRE_3DVIEW)

# if (NVBShared) {
#   unix {
#     target.path = /usr/lib
#   } else {
#     target.path = $$NVB_PLUGIN_INSTALL_PATH
#   }
#   INSTALLS += target
# }
