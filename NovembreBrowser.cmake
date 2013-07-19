# Log
# set(browser_HDRS src/gui/NVBLogger.h)
set(browser_HDRS)
set(browser_SRCS)

if (NOVEMBRE_LOG)
	set(browser_HDRS
		${browser_HDRS}
		src/gui/NVBLogUtils.h
		)
	set(browser_SRCS
		${browser_SRCS}
# 		src/gui/NVBLogger.cpp
		src/gui/NVBLogUtils.cpp
		)
endif(NOVEMBRE_LOG)

include_directories("src/core")

# STMFile tools
set(browser_HDRS
	${browser_HDRS}
	src/gui/NVBFileBundle.h
# 	src/gui/NVBGeneratorsSettingsWidget.h
	src/gui/NVBFilePluginModel.h
	src/gui/NVBFileFactory.h
	)
set(browser_SRCS
	${browser_SRCS}
	src/gui/NVBFileBundle.cpp
	src/gui/NVBGeneratorsSettingsWidget.cpp
	src/gui/NVBFilePluginModel.cpp
	src/gui/NVBFileFactory.cpp
	)

# FileBrowser
set(browser_HDRS
	${browser_HDRS}
	src/gui/NVBGradientMenu.h
	src/gui/NVBMutableGradients.h
	src/gui/NVBPageInfoWidget.h
	src/gui/NVBPageInfoView.h
	src/gui/NVBFileListView.h
# 	src/gui/NVBColumnsModel.h
	src/gui/NVBColumnDialog.h
	src/gui/NVBFileFilterDialog.h
	src/gui/NVBDirModelHelpers.h
	src/gui/NVBDirModel.h
	src/gui/NVBDirView.h
	src/gui/NVBDirViewModel.h
	src/gui/NVBPosLabel.h
	src/gui/NVBSingle2DView.h
	src/gui/NVBPlotCurves.h
	src/gui/NVBPhysScaleDraw.h
	src/gui/NVBSingleGraphView.h
	src/gui/NVBSingleView.h
	src/gui/NVBBrowserHelpers.h
	src/gui/NVBBrowser.h
	)

set(browser_SRCS
	${browser_SRCS}
	src/gui/NVBGradientMenu.cpp
	src/gui/NVBMutableGradients.cpp
	src/gui/NVBPageInfoWidget.cpp
	src/gui/NVBFileListView.cpp
	src/gui/NVBColumnsModel.cpp
	src/gui/NVBColumnDialog.cpp
	src/gui/NVBFileFilterDialog.cpp
	src/gui/NVBDirModelHelpers.cpp
	src/gui/NVBDirModel.cpp
	src/gui/NVBDirView.cpp
	src/gui/NVBDirViewModel.cpp
	src/gui/NVBPosLabel.cpp
	src/gui/NVBSingle2DView.cpp
	src/gui/NVBPlotCurves.cpp
	src/gui/NVBPhysScaleDraw.cpp
	src/gui/NVBSingleGraphView.cpp
	src/gui/NVBSingleView.cpp
	src/gui/NVBBrowserHelpers.cpp
	src/gui/NVBBrowser.cpp
	)

# Application headers
set(browser_HDRS
	${browser_HDRS}
# 	src/gui/NVBStandardIcons.h
# 	src/gui/NVBSettings.h
	src/gui/NVBSettingsDialog.h
	src/gui/NVBMainWindow.h
	src/gui/NVBMainDocks.h
	src/gui/NVBCoreApplication.h
	src/gui/NvBrowserApp.h
	)

set(browser_SRCS
	${browser_SRCS}
	src/gui/NVBStandardIcons.cpp
# 	src/gui/NVBSettings.cpp
	src/gui/NVBSettingsDialog.cpp
	src/gui/NVBMainWindow.cpp
	src/gui/NVBMainDocks.cpp
	src/gui/NVBCoreApplication.cpp
	src/gui/NvBrowserApp.cpp
	)

# icons
if(WIN32)
	set(browser_ICON icons/novembre.rc)
elseif(APPLE)
	set(browser_ICON icons/nvb.icns)
endif()

qt4_wrap_cpp(browser_MOC_SRCS ${browser_HDRS})
add_executable(nvbrowser ${browser_SRCS} ${browser_MOC_SRCS} ${browser_ICON})
target_link_libraries(nvbrowser ${QT_LIBRARIES} ${Qwt6_LIBRARY})
set_target_properties(nvbrowser PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

if (NOVEMBRE_STATIC_PLUGINS)
	# libnvb should come before the plugins (at least with GCC 4.6.3)
	# Otherwise, the static members of the generators are initialized
	# before the static members in the library, e.g. NVBUnits::recongnizedUnits
	# and this segfaults, since NVBFileGenerator::availableInfoFields needs it.
	target_link_libraries(nvbrowser libnvb createc nanonis rhk)
else(NOVEMBRE_STATIC_PLUGINS)
	target_link_libraries(nvbrowser libnvb)
endif(NOVEMBRE_STATIC_PLUGINS)