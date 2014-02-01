find_package(Jrtplib QUIET NO_MODULE)

if (NOT JRTPLIB_FOUND) # Config file could not be found
        find_path(JRTPLIB_INCLUDE_DIR jrtplib/rtpsession.h)
	set(JRTPLIB_INCLUDE_DIRS ${JRTPLIB_INCLUDE_DIR})
	if (WIN32)
                find_library(JRTPLIB_LIB_RELEASE jrtplib)
                find_library(JRTPLIB_LIB_DEBUG jrtplibd)
		
		if(JRTPLIB_LIB_RELEASE OR JRTPLIB_LIB_DEBUG)
			set(JRTPLIB_LIBRARIES "")
			if(JRTPLIB_LIB_RELEASE)
				list(APPEND JRTPLIB_LIBRARIES optimized ${JRTPLIB_LIB_RELEASE})
			endif()
			if(JRTPLIB_LIB_DEBUG)
				list(APPEND JRTPLIB_LIBRARIES debug ${JRTPLIB_LIB_DEBUG})
			endif()
                        list(APPEND JRTPLIB_LIBRARIES wsock32 ws2_32)
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Jrtplib DEFAULT_MSG JRTPLIB_INCLUDE_DIRS JRTPLIB_LIBRARIES)
