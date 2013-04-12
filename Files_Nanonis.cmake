set(nanonis_SRCS src/nanonis.cpp)
set(nanonis_HDRS src/nanonis.h)

qt4_wrap_cpp(nanonis_MOC_SRCS ${nanonis_HDRS})

add_library(nanonis ${PLUGIN_BUILD_MODE} ${nanonis_SRCS} ${nanonis_MOC_SRCS})
target_link_libraries(nanonis ${QT_LIBRARIES} ${Qwt6_LIBRARY} libnvb)
