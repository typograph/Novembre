# Find the Qwt 6.x includes and library, linked to Qt4
#
# On Windows it makes these assumptions:
#    - the Qwt DLL is where the other DLLs for Qt are (QT_DIR\bin) or in the path
#    - the Qwt .h files are in QT_DIR\include\Qwt or in the path
#    - the Qwt .lib is where the other LIBs for Qt are (QT_DIR\lib) or in the path
#
# Qwt6_INCLUDE_DIR - where to find qwt.h if Qwt
# Qwt6_LIBRARY - The Qwt6 library linked against Qt4 (if it exists)
# Qwt6_FOUND   - Qwt6 was found and uses Qt4

# Copyright (c) 2007, Pau Garcia i Quiles, <pgquiles@elpauer.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Condition is "(A OR B) AND C", CMake does not support parentheses but it evaluates left to right
IF(Qwt6_Qt4_LIBRARY AND Qwt6_INCLUDE_DIR)
    SET(Qwt6_FIND_QUIETLY TRUE)
ENDIF(Qwt6_Qt4_LIBRARY AND Qwt6_INCLUDE_DIR)

IF(NOT QT4_FOUND)
	FIND_PACKAGE( Qt4 REQUIRED QUIET )
ENDIF(NOT QT4_FOUND)

IF( QT4_FOUND )
	# Is Qwt6 installed? Look for header files

	FILE(GLOB Qwt6_GLOB_PATH /usr/qwt* /usr/local/qwt* /usr/include/qwt*)

	FIND_PATH( Qwt6_INCLUDE_DIR qwt.h 
               PATHS ${QT_INCLUDE_DIR} ${Qwt6_GLOB_PATH}
               PATH_SUFFIXES qwt qwt6 qwt-qt4 qwt6-qt4 include qwt/include qwt6/include qwt-qt4/include qwt6-qt4/include ENV PATH)
	
	# Find Qwt version
	IF( Qwt6_INCLUDE_DIR )
		FILE( READ ${Qwt6_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H )
		STRING( REGEX MATCH "#define *QWT_VERSION *(0x06*)" QWT_IS_VERSION_6 ${QWT_GLOBAL_H})
		
		IF( QWT_IS_VERSION_6 )
		STRING(REGEX REPLACE ".*#define[\\t\\ ]+QWT_VERSION_STR[\\t\\ ]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\".*" "\\1" Qwt_VERSION "${QWT_GLOBAL_H}")

		# Find Qwt6 library linked to Qt4
		FIND_LIBRARY( Qwt6_TENTATIVE_LIBRARY NAMES qwt6-qt4 qwt-qt4 qwt6 qwt PATHS /usr/local/qwt/lib /usr/local/lib ${Qwt6_INCLUDE_DIR}/../lib /usr/lib ${QT_LIBRARY_DIR} )
		IF( UNIX AND NOT CYGWIN)
			IF( Qwt6_TENTATIVE_LIBRARY )
				EXECUTE_PROCESS( COMMAND "ldd" ${Qwt6_TENTATIVE_LIBRARY} OUTPUT_VARIABLE Qwt_LIBRARIES_LINKED_TO )
				STRING( REGEX MATCH ".*QtCore.*" Qwt6_IS_LINKED_TO_Qt4 ${Qwt_LIBRARIES_LINKED_TO})
				IF( Qwt6_IS_LINKED_TO_Qt4 )
					SET( Qwt6_LIBRARY ${Qwt6_TENTATIVE_LIBRARY} )
					SET( Qwt6_FOUND TRUE )
					IF (NOT Qwt6_FIND_QUIETLY)
						MESSAGE( STATUS "Found Qwt: ${Qwt6_LIBRARY}" )
					ENDIF (NOT Qwt6_FIND_QUIETLY)
				ENDIF( Qwt6_IS_LINKED_TO_Qt4 )
			ENDIF( Qwt6_TENTATIVE_LIBRARY )
		ELSE( UNIX AND NOT CYGWIN)
		# Assumes qwt.dll is in the Qt dir
			SET( Qwt6_LIBRARY ${Qwt6_TENTATIVE_LIBRARY} )
			SET( Qwt6_FOUND TRUE )
			IF (NOT Qwt6_FIND_QUIETLY)
				MESSAGE( STATUS "Found Qwt version ${Qwt_VERSION} linked to Qt4" )
			ENDIF (NOT Qwt6_FIND_QUIETLY)
		ENDIF( UNIX AND NOT CYGWIN)
		
		ENDIF( QWT_IS_VERSION_6 )
		
		MARK_AS_ADVANCED( Qwt6_INCLUDE_DIR Qwt6_LIBRARY)

        ELSE( Qwt6_INCLUDE_DIR )
		MESSAGE("Qwt 6.x include dir not found")
	ENDIF( Qwt6_INCLUDE_DIR )

   	IF (NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)
      		MESSAGE(FATAL_ERROR "Could not find Qwt 6.x")
   	ENDIF (NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)

ENDIF( QT4_FOUND )
