set(fileplugins createc nanonis rhk) #rhk4 winspm)

include(Files_Createc.cmake)
include(Files_Nanonis.cmake)
include(Files_RHK.cmake)
# include(Files_RHK4.cmake)
# include(Files_WinSPM.cmake)

set_target_properties(${fileplugins}
	PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY lib/files
	ARCHIVE_OUTPUT_DIRECTORY lib/files
	)

