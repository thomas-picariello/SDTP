find_package(HUpnp QUIET NO_MODULE)

if (NOT HUPNP_FOUND) # Config file could not be found
	find_path(HUPNP_INCLUDE_DIR HUpnpCore/Hupnp)
	set(HUPNP_INCLUDE_DIRS ${HUPNP_INCLUDE_DIR})
	if (WIN32)
		find_library(HUPNP_LIB_RELEASE HUpnp)
		find_library(HUPNP_LIB_DEBUG HUpnp)
		
		if(HUPNP_LIB_RELEASE OR HUPNP_LIB_DEBUG)
			set(HUPNP_LIBRARIES "")
			if(HUPNP_LIB_RELEASE)
				list(APPEND HUPNP_LIBRARIES optimized ${HUPNP_LIB_RELEASE})
			endif()
			if(HUPNP_LIB_DEBUG)
				list(APPEND HUPNP_LIBRARIES debug ${HUPNP_LIB_DEBUG})
			endif()
		endif()
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(HUpnp DEFAULT_MSG HUPNP_INCLUDE_DIRS HUPNP_LIBRARIES)