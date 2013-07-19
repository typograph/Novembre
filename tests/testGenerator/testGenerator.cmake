set(QT_DONT_USE_QTGUI TRUE)

add_definitions(-DFILEGENERATOR_NO_GUI -DNVB_ENABLE_LOG)

set(testgen_HDRS ${testgen_HDRS}
	${NOVEMBRE_SOURCE_DIR}/NVBDataSource.h
	${NOVEMBRE_SOURCE_DIR}/NVBDataSourceModel.h
	${NOVEMBRE_SOURCE_DIR}/NVBFile.h
	${NOVEMBRE_SOURCE_DIR}/NVBMap.h
	${NOVEMBRE_SOURCE_DIR}/NVBLogger.h
	${NOVEMBRE_SOURCE_DIR}/NVBDatasetIcons.h
	${NOVEMBRE_SOURCE_DIR}/NVBCoreApplication.h
	testGenerator.h
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
	testGenerator.cpp
	)

qt4_wrap_cpp(testgen_MOC_SRCS ${testgen_HDRS})

add_executable(testGenerator ${testgen_SRCS} ${testgen_MOC_SRCS})
target_link_libraries(testGenerator ${QT_LIBRARIES})