set(FIXED_POINT FALSE)
set(DISABLE_FLOAT_API FALSE)

include(celt_headers.cmake)
include(silk_headers.cmake)
include(opus_headers.cmake)

include(celt_sources.cmake)
include(silk_sources.cmake)
include(opus_sources.cmake)

if(FIXED_POINT)
    LIST(APPEND SILK_SOURCES ${SILK_SOURCES_FIXED})
    include_directories(silk/fixed)
else()
    LIST(APPEND SILK_SOURCES ${SILK_SOURCES_FLOAT})
    include_directories(silk/float)
endif()

if(!DISABLE_FLOAT_API)
    LIST(APPEND OPUS_SOURCES ${OPUS_SOURCES_FLOAT})
endif()

if(WIN32)
    #create version.h
    execute_process(COMMAND genversion.bat version.h PACKAGE_VERSION
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/win32)

    add_definitions(-DHAVE_CONFIG_H)
    set(OPUS_CONFIG
        win32/version.h
        win32/config.h
    )
endif()

include_directories(
	include
	celt
        silk
        win32
)

add_library(opus STATIC
    ${OPUS_CONFIG}
    ${OPUS_HEAD}
    ${OPUS_SOURCES}
    ${CELT_HEAD}
    ${CELT_SOURCES}
    ${SILK_HEAD}
    ${SILK_SOURCES}
)
