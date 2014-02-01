find_package(Cryptlib QUIET NO_MODULE)

if (NOT CRYPTLIB_FOUND) # Config file could not be found
        find_path(CRYPTLIB_INCLUDE_DIR cryptopp/cryptlib.h)
        set(CRYPTLIB_INCLUDE_DIRS ${CRYPTLIB_INCLUDE_DIR})
	if (WIN32)
                find_library(CRYPTLIB_LIB_RELEASE cryptlib)
                find_library(CRYPTLIB_LIB_DEBUG cryptlibd)
		
                if(CRYPTLIB_LIB_RELEASE OR CRYPTOPP_LIB_DEBUG)
                        set(CRYPTLIB_LIBRARIES "")
                        if(CRYPTLIB_LIB_RELEASE)
                                list(APPEND CRYPTLIB_LIBRARIES optimized ${CRYPTLIB_LIB_RELEASE})
			endif()
                        if(CRYPTLIB_LIB_DEBUG)
                                list(APPEND CRYPTLIB_LIBRARIES debug ${CRYPTLIB_LIB_DEBUG})
			endif()
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Cryptlib DEFAULT_MSG CRYPTLIB_INCLUDE_DIRS CRYPTLIB_LIBRARIES)
