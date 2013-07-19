set(winspm_SRCS src/files/winspm.cpp)
set(winspm_HDRS src/files/winspm.h)

qt4_wrap_cpp(winspm_MOC_SRCS ${winspm_HDRS})

add_library(winspm ${PLUGIN_BUILD_MODE} ${winspm_SRCS} ${winspm_MOC_SRCS})
target_link_libraries(winspm ${QT_LIBRARIES} ${Qwt6_LIBRARY} libnvb)
