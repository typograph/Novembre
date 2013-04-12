if(WIN32)
  find_package(zlib REQUIRED)
  include_directories(${ZLIB_INCLUDE_DIR})
endif(WIN32)

set(createc_SRCS src/createc.cpp)
set(createc_HDRS src/createc.h)

qt4_wrap_cpp(createc_MOC_SRCS ${createc_HDRS})

# add_definitions(-DQT_PLUGIN)
add_library(createc ${PLUGIN_BUILD_MODE} ${createc_SRCS} ${createc_MOC_SRCS})
target_link_libraries(createc ${QT_LIBRARIES} ${Qwt6_LIBRARY} ${ZLIB_LIBRARIES} libnvb)
