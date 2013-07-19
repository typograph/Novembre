add_definitions(-DNVB_DEBUG -DNVB_ENABLE_LOG -DNVB_VERBOSE_LOG)

# Add Qwt
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

if (UNIX)
	find_package(Qwt6 REQUIRED)
else(UNIX)
	set(Qwt6_INCLUDE_DIR ../qwt-6.0.0/src/)
	set(Qwt6_LIBRARY ../qwt-6.0.0/lib)
endif(UNIX)

include_directories(${Qwt6_INCLUDE_DIR})

set(testgen_HDRS ${testgen_HDRS}
	${NOVEMBRE_SOURCE_DIR}/NVBDataSource.h
	${NOVEMBRE_SOURCE_DIR}/NVBDataSourceModel.h
	${NOVEMBRE_SOURCE_DIR}/NVBFile.h
	${NOVEMBRE_SOURCE_DIR}/NVBMap.h
	${NOVEMBRE_SOURCE_DIR}/NVBLogger.h
	${NOVEMBRE_SOURCE_DIR}/NVBDatasetIcons.h
	${NOVEMBRE_SOURCE_DIR}/NVBCoreApplication.h
	testGeneratorGUI.h
	)

set(testgen_SRCS ${testgen_SRCS}
	${NOVEMBRE_SOURCE_DIR}/NVBDataSource.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBDataSourceModel.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBFileInfo.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBFile.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBAxisSelectorHelpers.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBAxisSelector.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBScaler.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBColorMaps.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBMap.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBLogger.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBDataTransforms.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBDataCore.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBUnits.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBVariant.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBSettings.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBTokens.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBDatasetIcons.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBMimeData.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBCoreApplication.cpp
	testGeneratorGUI.cpp
	)

set(testgen_HDRS ${testgen_HDRS}
	${NOVEMBRE_SOURCE_DIR}/NVBPosLabel.h
	${NOVEMBRE_SOURCE_DIR}/NVBSettingsWidget.h
	${NOVEMBRE_SOURCE_DIR}/NVBSingle2DView.h
	${NOVEMBRE_SOURCE_DIR}/NVBPlotCurves.h
	${NOVEMBRE_SOURCE_DIR}/NVBPhysScaleDraw.h
	${NOVEMBRE_SOURCE_DIR}/NVBSingleGraphView.h
	${NOVEMBRE_SOURCE_DIR}/NVBSingleView.h
	)

set(testgen_SRCS ${testgen_SRCS}
	${NOVEMBRE_SOURCE_DIR}/NVBPosLabel.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBSettingsWidget.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBSingle2DView.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBPlotCurves.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBPhysScaleDraw.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBSingleGraphView.cpp
	${NOVEMBRE_SOURCE_DIR}/NVBSingleView.cpp
	)

qt4_wrap_cpp(testgen_MOC_SRCS ${testgen_HDRS})

add_executable(testGeneratorGUI ${testgen_SRCS} ${testgen_MOC_SRCS})
target_link_libraries(testGeneratorGUI ${QT_LIBRARIES} ${Qwt6_LIBRARY})