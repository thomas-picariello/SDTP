find_package(Cryptopp QUIET NO_MODULE)

if (NOT CRYPTOPP_FOUND) # Config file could not be found
	find_path(CRYPTOPP_INCLUDE_DIR cryptopp/cryptlib.h)
	set(CRYPTOPP_INCLUDE_DIRS ${CRYPTOPP_INCLUDE_DIR})
	if (WIN32)
		find_library(CRYPTOPP_LIB_RELEASE cryptlib)
		find_library(CRYPTOPP_LIB_DEBUG cryptlibd)
		
		if(CRYPTOPP_LIB_RELEASE OR CRYPTOPP_LIB_DEBUG)
			set(CRYPTOPP_LIBRARIES "")
			if(CRYPTOPP_LIB_RELEASE)
				list(APPEND CRYPTOPP_LIBRARIES optimized ${CRYPTOPP_LIB_RELEASE})
			endif()
			if(CRYPTOPP_LIB_DEBUG)
				list(APPEND CRYPTOPP_LIBRARIES debug ${CRYPTOPP_LIB_DEBUG})
			endif()
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Cryptopp DEFAULT_MSG CRYPTOPP_INCLUDE_DIRS CRYPTOPP_LIBRARIES)