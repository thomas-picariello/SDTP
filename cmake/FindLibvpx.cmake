find_package(Libvpx QUIET NO_MODULE)

if (NOT LIBVPX_FOUND) # Config file could not be found
	find_path(LIBVPX_INCLUDE_DIR vpx/vpx_codec.h)
	set(LIBVPX_INCLUDE_DIRS ${LIBVPX_INCLUDE_DIR})
	if (WIN32)
		find_library(LIBVPX_LIB_RELEASE vpxmd)
		find_library(LIBVPX_LIB_DEBUG vpxmdd)
		
		if(LIBVPX_LIB_RELEASE OR LIBVPX_LIB_DEBUG)
			set(LIBVPX_LIBRARIES "")
			if(LIBVPX_LIB_RELEASE)
				list(APPEND LIBVPX_LIBRARIES optimized ${LIBVPX_LIB_RELEASE})
			endif()
			if(LIBVPX_LIB_DEBUG)
				list(APPEND LIBVPX_LIBRARIES debug ${LIBVPX_LIB_DEBUG})
			endif()
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LIBVPX DEFAULT_MSG LIBVPX_INCLUDE_DIRS LIBVPX_LIBRARIES)