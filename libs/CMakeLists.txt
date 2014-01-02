#define lib paths as global vars
set(LIBS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/libs" CACHE STRING "Libraries source directory")
set(LIBS_BINARY_DIR "${PROJECT_BINARY_DIR}/libs" CACHE STRING "Libraries binary directory")

#glob subdirectories
set(SUB_DIRS
    cryptopp
    #jthread
    #jrtplib
    #opus
)

foreach(DIR ${SUB_DIRS})
    add_subdirectory(${DIR})
endforeach()
