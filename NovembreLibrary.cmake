if(NOVEMBRE_3DVIEW)
#  set(WITH_GL2PS TRUE) // Uncomment if you want export to vector formats
  include(QwtPlot3D.cmake)
  set(QT_USE_OPENGL TRUE)
endif(NOVEMBRE_3DVIEW)

# Log

set(nvblib_HDRS src/core/NVBLogger.h)
set(nvblib_SRCS)

if (NOVEMBRE_LOG)
  set(nvblib_SRCS src/core/NVBLogger.cpp)
endif (NOVEMBRE_LOG)

# if (NOVEMBRE_2DVIEW)
# 	set(nvblib_HDRS
# 		${nvblib_HDRS}
# 		src/core/NVBGraphicsItems.h
# 		)
# 	set(nvblib_SRCS
# 		${nvblib_SRCS}
# 		src/core/NVBGraphicsItems.cpp
# 		)
# endif (NOVEMBRE_2DVIEW)

# Basic data structures

set(nvblib_HDRS
	${nvblib_HDRS}
	src/core/NVBMap.h
	src/core/NVBDatasetIcons.h
	src/core/NVBDataSource.h
	src/core/NVBDataSourceModel.h
	src/core/NVBFile.h
	)

set(nvblib_SRCS
	${nvblib_SRCS}
	src/core/NVBScaler.cpp
	src/core/NVBUnits.cpp
	src/core/NVBVariant.cpp
	src/core/NVBMap.cpp
	src/core/NVBDataCore.cpp
	src/core/NVBDataTransforms.cpp
	src/core/NVBDatasetIcons.cpp
	src/core/NVBDataSource.cpp
	src/core/NVBDataSourceModel.cpp
	src/core/NVBAxisSelectorHelpers.cpp
	src/core/NVBAxisSelector.cpp
	src/core/NVBTokens.cpp
	src/core/NVBFileInfo.cpp
	src/core/NVBMimeData.cpp
	src/core/NVBFile.cpp
	)

# Helpful objects for plugins

set(nvblib_HDRS
	${nvblib_HDRS}
	src/core/NVBPhysSpinBox.h
	src/core/NVBPhysStepSpinBox.h
	src/core/NVBColorWidgets.h
	src/core/NVBSettingsWidget.h
	)

set(nvblib_SRCS
	${nvblib_SRCS}
	src/core/NVBColorMaps.cpp
	src/core/NVBPhysSpinBox.cpp
	src/core/NVBPhysStepSpinBox.cpp
	src/core/NVBColorWidgets.cpp
	src/core/NVBSettings.cpp
	src/core/NVBSettingsWidget.cpp
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
