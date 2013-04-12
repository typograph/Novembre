project(testGenerator)
cmake_minimum_required(VERSION 2.8)

# Test harness for arbitrary file generators

set(GNAME rhk)
set(GCLASSNAME RHKFileGenerator)

# end of serviceable part

set(CMAKE_BUILD_TYPE Debug)

# Add Qt
find_package(Qt4 4.6 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(NOVEMBRE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

include_directories(${NOVEMBRE_SOURCE_DIR})


set(testgen_HDRS ${NOVEMBRE_SOURCE_DIR}/${GNAME}.h)
set(testgen_SRCS ${NOVEMBRE_SOURCE_DIR}/${GNAME}.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-include ${NOVEMBRE_SOURCE_DIR}/${GNAME}.h")

add_definitions(-DTESTGENERATOR=${GCLASSNAME})

# include(testGenerator.cmake)
include(testGeneratorGUI.cmake)
