#include the full library
file(GLOB HEADERS *.h)
file(GLOB SOURCES *.cpp)

ADD_DEFINITIONS(-DNDEBUG)
IF(CMAKE_BUILD_TYPE MATCHES "Debug" OR MSVC)
  SET(OPT_FLAG "-O3")
ELSE()
  SET(OPT_FLAG "")
ENDIF()

IF(WIN32 AND NOT MSVC)
    SET_TARGET_PROPERTIES(cryptopp_static PROPERTIES COMPILE_FLAGS "-mthreads -w -finline-functions ${OPT_FLAG} -D_WIN32_WINNT=0x0501")
ELSEIF(WIN32 AND MSVC)
    CMAKE_POLICY(SET CMP0008 NEW)

    enable_language(ASM_MASM)
    set(SOURCES
        ${SOURCES}
        x64dll.asm
        x64masm.asm
    )
    SET(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Oy /GL /GF /EHsc /MD /Gy /W3 /Zi")
    SET(CMAKE_CXX_FLAGS_DEBUG "/Od /Oi /D_DEBUG /EHsc /MDd /W3 /Zi")
ENDIF()

add_library(cryptopp STATIC ${HEADERS} ${SOURCES} )
