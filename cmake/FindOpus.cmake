find_package(Opus QUIET NO_MODULE)

if (NOT OPUS_FOUND) # Config file could not be found
	find_path(OPUS_INCLUDE_DIR opus/opus.h)
	set(OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR})
	if (WIN32)
		find_library(OPUS_LIB_RELEASE opus)
		find_library(OPUS_LIB_DEBUG opusd)
		
		if(OPUS_LIB_RELEASE OR OPUS_LIB_DEBUG)
			set(OPUS_LIBRARIES "")
			if(OPUS_LIB_RELEASE)
				list(APPEND OPUS_LIBRARIES optimized ${OPUS_LIB_RELEASE})
			endif()
			if(OPUS_LIB_DEBUG)
				list(APPEND OPUS_LIBRARIES debug ${OPUS_LIB_DEBUG})
			endif()
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Opus DEFAULT_MSG OPUS_INCLUDE_DIRS OPUS_LIBRARIES)