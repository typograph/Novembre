# Log
set(browser_HDRS src/NVBLogger.h)
set(browser_SRCS)

if (NOVEMBRE_LOG)
	set(browser_HDRS
		${browser_HDRS}
		src/NVBLogUtils.h
		)
	set(browser_SRCS
		${browser_SRCS}
		src/NVBLogger.cpp
		src/NVBLogUtils.cpp
		)
endif(NOVEMBRE_LOG)

# STMFile tools
set(browser_HDRS
	${browser_HDRS}
	src/NVBFileBundle.h
# 	src/NVBGeneratorsSettingsWidget.h
	src/NVBFilePluginModel.h
	src/NVBFileFactory.h
	)
set(browser_SRCS
	${browser_SRCS}
	src/NVBFileBundle.cpp
	src/NVBGeneratorsSettingsWidget.cpp
	src/NVBFilePluginModel.cpp
	src/NVBFileFactory.cpp
	)

# FileBrowser
set(browser_HDRS
	${browser_HDRS}
	src/NVBGradientMenu.h
	src/NVBMutableGradients.h
	src/NVBPageInfoWidget.h
	src/NVBPageInfoView.h
	src/NVBFileListView.h
# 	src/NVBColumnsModel.h
	src/NVBColumnDialog.h
	src/NVBFileFilterDialog.h
	src/NVBDirModelHelpers.h
	src/NVBDirModel.h
	src/NVBDirView.h
	src/NVBDirViewModel.h
	src/NVBPosLabel.h
	src/NVBSingle2DView.h
	src/NVBPlotCurves.h
	src/NVBPhysScaleDraw.h
	src/NVBSingleGraphView.h
	src/NVBSingleView.h
	src/NVBBrowserHelpers.h
	src/NVBBrowser.h
	)

set(browser_SRCS
	${browser_SRCS}
	src/NVBGradientMenu.cpp
	src/NVBMutableGradients.cpp
	src/NVBPageInfoWidget.cpp
	src/NVBFileListView.cpp
	src/NVBColumnsModel.cpp
	src/NVBColumnDialog.cpp
	src/NVBFileFilterDialog.cpp
	src/NVBDirModelHelpers.cpp
	src/NVBDirModel.cpp
	src/NVBDirView.cpp
	src/NVBDirViewModel.cpp
	src/NVBPosLabel.cpp
	src/NVBSingle2DView.cpp
	src/NVBPlotCurves.cpp
	src/NVBPhysScaleDraw.cpp
	src/NVBSingleGraphView.cpp
	src/NVBSingleView.cpp
	src/NVBBrowserHelpers.cpp
	src/NVBBrowser.cpp
	)

# Application headers
set(browser_HDRS
	${browser_HDRS}
# 	src/NVBStandardIcons.h
# 	src/NVBSettings.h
	src/NVBSettingsDialog.h
	src/NVBMainWindow.h
	src/NVBMainDocks.h
	src/NVBCoreApplication.h
	src/NvBrowserApp.h
	)

set(browser_SRCS
	${browser_SRCS}
	src/NVBStandardIcons.cpp
	src/NVBSettings.cpp
	src/NVBSettingsDialog.cpp
	src/NVBMainWindow.cpp
	src/NVBMainDocks.cpp
	src/NVBCoreApplication.cpp
	src/NvBrowserApp.cpp
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