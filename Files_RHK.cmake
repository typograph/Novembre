set(rhk_SRCS src/rhk.cpp)
set(rhk_HDRS src/rhk.h)

qt4_wrap_cpp(rhk_MOC_SRCS ${rhk_HDRS})

add_library(rhk ${PLUGIN_BUILD_MODE} ${rhk_SRCS} ${rhk_MOC_SRCS})
target_link_libraries(rhk ${QT_LIBRARIES} ${Qwt6_LIBRARY} libnvb)
